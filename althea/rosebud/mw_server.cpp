#include <boost/asio.hpp>

#include "../msg/message.hpp"
#include "mw_server.hpp"
#include "tcp_conn.hpp"

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

        std::string msg(msg_buf->begin(), msg_buf->end());
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

void mw_server_t::handle_msg(const std::string &msg, 
                             std::shared_ptr<tcp_conn_t> conn)
{
    // log message received by mw server
    std::cout << msg << std::endl;
    if (msg.starts_with("subscribe")) {
        const std::string topic = msg.substr(msg.find(',') + 1);
        m_topic_to_subs[topic].insert(conn);
        return;
    }

    if (msg.starts_with("unsubscribe")) {
        const std::string topic = msg.substr(msg.find(',') + 1);
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

    const message_t converted_msg = message_t(msg);
    if (converted_msg.m_topic == "shutdown") {
        const shutdown_data_t shutdown_data = converted_msg.m_data;
        // client disconnect
        if (shutdown_data.m_to == "self" && shutdown_data.m_from == "self") {
            const int id = conn->get_id();
            for (auto &[_, subs] : m_topic_to_subs) {
                subs.erase(conn);
            }
            m_id_to_conn.erase(id);
            return;
        }
    }

    forward_msg(converted_msg);
}

void mw_server_t::forward_msg(const message_t &msg) {
    const std::string topic = msg.m_topic;
    const auto &it = m_topic_to_subs.find(topic);
    if (it == m_topic_to_subs.end()) {
        return;
    }

    auto &subs = it->second;
    for (const auto &sub : subs) {
        sub->write(msg.serialize());
    }
}
