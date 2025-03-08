#include <boost/asio.hpp>

#include "mw_server.hpp"
#include "tcp_conn.hpp"

#include "../proto/message_factory.hpp"
#include "../proto/message.pb.h"

mw_server_t::mw_server_t(const int port) 
    : m_io_context(), 
      m_acceptor(m_io_context, tcp::endpoint(tcp::v4(), port))
{
    start_accept();
    m_io_context.run();
}

mw_server_t::~mw_server_t() {
    m_acceptor.close();
    m_io_context.stop(); 
}

void mw_server_t::start_accept() {
    std::shared_ptr<tcp_conn_t> new_conn = tcp_conn_t::create(m_io_context);
    m_id_to_conn[new_conn->get_id()] = new_conn;

    auto handler = [this, new_conn](const boost::system::error_code &ec) {
        handle_accept(new_conn, ec);
    };

    m_acceptor.async_accept(new_conn->socket(), handler);
}

void mw_server_t::handle_accept(std::shared_ptr<tcp_conn_t> new_conn,
                                const boost::system::error_code &ec) 
{
    if (!ec) {
        read_msg(new_conn);
    }
    start_accept();
}

void mw_server_t::read_msg(std::shared_ptr<tcp_conn_t> conn) {
    auto msg_len_buf = std::make_shared<std::array<char, sizeof(uint32_t)>>();
    auto msg_buf = std::make_shared<std::vector<char>>();

    auto msg_callback = [this, msg_buf, conn]
        (const boost::system::error_code &ec, std::size_t _)
    {
        if (ec) {
            return;
        }

        messaging::envelope msg;
        std::string raw_msg(msg_buf->begin(), msg_buf->end());
        if (!msg.ParseFromString(raw_msg)) {
            std::cerr << "failed to parse protobuf message" << std::endl;
            return;
        }

        handle_msg(msg, conn);
        read_msg(conn);
    };

    auto len_callback = [this, msg_len_buf, msg_buf, msg_callback, conn]
        (const boost::system::error_code &ec, std::size_t _)
    {
        if (ec) {
            return;
        }

        uint32_t msg_len_no = *reinterpret_cast<uint32_t*>(
            msg_len_buf->data());
        uint32_t msg_len = ntohl(msg_len_no);

        msg_buf->resize(msg_len);

        boost::asio::async_read(conn->socket(),
                                boost::asio::buffer(*msg_buf),
                                msg_callback);
    };

    boost::asio::async_read(conn->socket(),
                            boost::asio::buffer(*msg_len_buf),
                            len_callback);
}

void mw_server_t::handle_msg(const messaging::envelope &msg, 
                             std::shared_ptr<tcp_conn_t> conn)
{
    // log message received by mw server
    std::cout << msg.DebugString() << std::endl;
    if (msg.topic() == "subscribe") {
        const std::string &topic = msg.subscribe_data().topic();
        m_topic_to_subs[topic].insert(conn);
        return;
    }

    if (msg.topic() == "unsubscribe") { 
        const std::string &topic = msg.unsubscribe_data().topic();
        const auto &it = m_topic_to_subs.find(topic);
        if (it == m_topic_to_subs.end()) {
            return;
        }

        it->second.erase(conn);
        if (it->second.size() == 0) {
            m_topic_to_subs.erase(topic);
        }
        return;
    }

    if (msg.topic() == "shutdown") {
        auto shutdown_data = msg.shutdown_data();
        // client disconnect
        if (shutdown_data.to() == "self" && shutdown_data.from() == "self") {
            const int id = conn->get_id();
            for (auto &[_, subs] : m_topic_to_subs) {
                subs.erase(conn);
            }
            m_id_to_conn.erase(id);
            return;
        }
    }

    forward_msg(msg);
}

void mw_server_t::forward_msg(const messaging::envelope &msg) {
    const std::string topic = msg.topic();
    const auto &it = m_topic_to_subs.find(topic);
    if (it == m_topic_to_subs.end()) {
        return;
    }

    auto &subs = it->second;
    for (const auto &sub : subs) {
        send(sub, msg);
    }
}

void mw_server_t::send(std::shared_ptr<tcp_conn_t> sub, 
                       const messaging::envelope &msg) 
{
    std::string serialized_msg;
    if (!msg.SerializeToString(&serialized_msg)) {
        std::cerr << "failed to serialize protobuf message" << std::endl;
        return;
    }

    uint32_t msg_len = htonl(static_cast<uint32_t>(serialized_msg.size()));

    const std::vector<boost::asio::const_buffer> bufs {
        boost::asio::buffer(&msg_len, sizeof(msg_len)),
        boost::asio::buffer(serialized_msg)
    };

    auto write_callback = 
        [](const boost::system::error_code &ec, std::size_t n) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                std::cout << n << " bytes written" << std::endl;
            }
        };

    boost::asio::async_write(sub->socket(), bufs, write_callback);
}
