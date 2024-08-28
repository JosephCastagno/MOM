#include <boost/asio.hpp>

#include "../msg/message.hpp"
#include "mw_server.hpp"
#include "tcp_conn.hpp"

const message_t msg = message_t("shutdown", shutdown_data_t("test").serialize());

mw_server_t::mw_server_t(const int port) 
    : m_io_context(), 
      m_acceptor(m_io_context, tcp::endpoint(tcp::v4(), port))
{
    std::cout << "server constructed" << std::endl;
    start_accept();
    m_io_context.run();
}

mw_server_t::~mw_server_t() {
    std::cout << "server destroyed" << std::endl;
    m_acceptor.close();
    m_io_context.stop(); 
}

void mw_server_t::start_accept() {
    std::shared_ptr<tcp_conn_t> new_conn = tcp_conn_t::create(m_io_context);

    auto handler = [this, new_conn](const boost::system::error_code &ec) {
        this->handle_accept(new_conn, ec);
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
    std::cout << 4 << std::endl;
    auto msg_len_buf = std::make_shared<std::array<char, sizeof(uint32_t)>>();
    auto msg_buf = std::make_shared<std::vector<char>>();

    auto msg_callback = [this, msg_buf, conn]
        (const boost::system::error_code &ec, std::size_t _)
    {
        if (ec) {
            disconnect(conn); // if there is an error, should we disconnect the client?
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
            disconnect(conn);
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
    std::cout << 3 << std::endl;
    if (msg.starts_with("subscribe")) {
        const std::string topic = msg.substr(msg.find(',') + 1);
        m_topic_to_subs[topic].insert(conn);
        return;
    }

    if (msg.starts_with("unsubscribe")) {
        const std::string topic = msg.substr(msg.find(',') + 1);
        auto it = m_topic_to_subs.find(topic);
        if (it == m_topic_to_subs.end()) {
            return;
        }

        it->second.erase(conn);
        if (it->second.size() == 0) {
            m_topic_to_subs.erase(topic);
        }
        return;
    }

    forward_msg(msg);
}

void mw_server_t::forward_msg(const std::string &msg) {
    std::cout << 2 << std::endl;
    const std::string topic = message_t::deserialize(msg).m_topic;
    auto it = m_topic_to_subs.find(topic);
    if (it == m_topic_to_subs.end()) {
        return;
    }

    auto subs = it->second;
    for (auto sub : subs) {
        sub->write(msg);
    }
}

void mw_server_t::disconnect(std::shared_ptr<tcp_conn_t> conn) {
    return;
    std::cout << 1 << std::endl;
    for (auto &[topic, subs] : m_topic_to_subs) {
        subs.erase(conn);
        if (subs.empty()) {
            m_topic_to_subs.erase(topic);
        }
    }
}
