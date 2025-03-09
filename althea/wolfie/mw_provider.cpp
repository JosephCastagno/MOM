#include <boost/asio.hpp>
#include <iostream>
#include <string>

#include "mw_provider.hpp"

using boost::asio::ip::tcp;

mw_provider_t::mw_provider_t(message_queue_t &msg_q, 
                             const std::string &ip, 
                             const int port)
    : m_msg_queue(msg_q), 
      m_io_context(), 
      m_socket(m_io_context), 
      m_strand(boost::asio::make_strand(m_io_context)),
      m_running(true)
{
    try {
        tcp::resolver resolver(m_io_context);
        auto endpoints = resolver.resolve(ip, std::to_string(port));
        boost::asio::connect(m_socket, endpoints);
        m_worker = std::thread([this] { receive_msgs(); });
    } catch (const std::exception &e) {
        std::cerr << "mw_provider_t initialization failed: ";
        std::cerr << e.what() << std::endl;
        throw; 
    }
    std::cout << "connected to middlware" << std::endl;
}

mw_provider_t::~mw_provider_t() {
    shutdown();
}

void mw_provider_t::shutdown() {
    if (!m_running) return;

    m_running = false;
    
    send_disconnect();

    // cancel pending asio operations, causes event loop to exit
    boost::system::error_code ec;
    m_socket.cancel(ec);
    if (ec) {
        std::cerr << "error cancelling operations: " 
                  << ec.message() << std::endl;
    }

    if (m_socket.is_open()) {
        m_socket.close(ec);
    }

    m_io_context.stop();
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

void mw_provider_t::receive_msgs() {
    m_io_context.reset();
    read_msg();
    m_io_context.run();
}

void mw_provider_t::read_msg() {
    auto msg_len_buf = std::make_shared<std::array<char, sizeof(uint32_t)>>();
    auto msg_buf = std::make_shared<std::vector<char>>();

    // lambda callback for message read 
    auto msg_callback = [this, msg_buf]
        (boost::system::error_code ec, std::size_t _)
    {
        if (ec) {
            std::cerr << "error reading msg: " << ec.message() << std::endl;
            return;
        }

        // parse protobuf message
        messaging::envelope msg;
        std::string raw_msg{msg_buf->data(), msg_buf->size()};
        if (!msg.ParseFromString(raw_msg)) {
            std::cerr << "failed to parse protobuf msg" << std::endl;
            return;
        }

        m_msg_queue.enqueue(msg);
        read_msg();
    };

    // lambda callback for length read 
    auto length_callback = [this, msg_len_buf, msg_buf, msg_callback]
        (boost::system::error_code ec, std::size_t _) 
    {
        if (ec) {
            std::cerr << "error reading msg len: " << ec.message() << std::endl;
            return;
        }

        uint32_t msg_len = ntohl(*reinterpret_cast<uint32_t*>(
            msg_len_buf->data()));

        msg_buf->resize(msg_len);
        boost::asio::async_read(m_socket, 
                                boost::asio::buffer(*msg_buf), 
                                boost::asio::bind_executor(m_strand, 
                                                           msg_callback));
    };

    boost::asio::async_read(m_socket, 
                            boost::asio::buffer(*msg_len_buf),
                            boost::asio::bind_executor(m_strand, 
                                                       length_callback));
}

void mw_provider_t::subscribe(const std::vector<std::string> &topics) {
    for (auto &topic : topics) {
        subscribe(topic);
    }
}

void mw_provider_t::subscribe(const std::string &topic) {
    const messaging::envelope sub = message_factory::create_subscribe(topic);
    send_msg(sub);
}

void mw_provider_t::unsubscribe(const std::string &topic) {
    const messaging::envelope unsub = message_factory::create_unsubscribe(
        topic);
    send_msg(unsub);
}

void mw_provider_t::publish(const messaging::envelope &msg) {
    send_msg(msg);
}

void mw_provider_t::send_msg(const messaging::envelope &msg) {
    std::string serialized_msg;
    if (!msg.SerializeToString(&serialized_msg)) {
        std::cerr << "failed to serialize protobuf message" << std::endl;
        return;
    }

    uint32_t msg_len = htonl(static_cast<uint32_t>(serialized_msg.size()));
    std::vector<boost::asio::const_buffer> bufs {
        boost::asio::buffer(&msg_len, sizeof(msg_len)),
        boost::asio::buffer(serialized_msg)
    };

    auto write_callback = 
        [](const boost::system::error_code &ec, std::size_t _) {
            if (ec) {
                std::cerr << "error sending msg: " 
                          << ec.message() << std::endl;
            }
        };

    boost::asio::async_write(m_socket, 
                             bufs, 
                             boost::asio::bind_executor(m_strand, 
                                                        write_callback));
}

void mw_provider_t::send_disconnect() {
    // disconnect from mw
    const messaging::envelope msg = message_factory::create_disconnect();
    send_msg(msg);
}
