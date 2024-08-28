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
        throw; // maybe just don't do any error handling here
    }

    std::cout << "client created" << std::endl;
}

mw_provider_t::~mw_provider_t() {
    std::cout << "client destroyed" << std::endl;
    m_running = false;
    if (m_socket.is_open()) {
        m_socket.close();
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

        std::string message(msg_buf->begin(), msg_buf->end());
        message_t msg = message_t::deserialize(message);
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

        uint32_t msg_len_no = *reinterpret_cast<uint32_t*>(msg_len_buf->data());
        uint32_t msg_length = ntohl(msg_len_no);
        msg_buf->resize(msg_length);

        boost::asio::async_read(m_socket, 
                                boost::asio::buffer(*msg_buf), 
                                msg_callback);
    };

    boost::asio::async_read(m_socket, 
                            boost::asio::buffer(*msg_len_buf),
                            length_callback);
}

void mw_provider_t::subscribe(const std::string &topic) {
    const std::string sub = "subscribe," + topic;
    send_msg_with_header(sub);
}

void mw_provider_t::unsubscribe(const std::string &topic) {
    const std::string unsub = "unsubscribe," + topic;
    send_msg_with_header(unsub);
}

void mw_provider_t::publish(const message_t &msg) {
    const std::string msg_str = msg.serialize();
    send_msg_with_header(msg_str);
}

void mw_provider_t::send_msg_with_header(const std::string &msg) {
    uint32_t msg_length_no = htonl(static_cast<uint32_t>(msg.size()));
    std::vector<boost::asio::const_buffer> bufs;
    bufs.emplace_back(
        boost::asio::buffer(&msg_length_no, sizeof(msg_length_no)));
    bufs.emplace_back(boost::asio::buffer(msg));
    boost::asio::async_write(m_socket, bufs,
        [](const boost::system::error_code &ec, std::size_t _) {
            if (ec) {
                std::cerr << "error sending msg: ";
                std::cerr << ec.message() << std::endl;
            }
        }
    );
}
