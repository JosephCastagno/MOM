#include <boost/asio.hpp>
#include <iostream>
#include <string>

#include "../msg/message.hpp"
#include "mw_server.hpp"


mw_server_t::mw_server_t(boost::asio::io_context &ctx, const int port) 
    : m_io_context(ctx), 
      m_acceptor(m_io_context, tcp::endpoint(tcp::v4(), port))
{
    std::cout << "initializing" << std::endl;
    start_accept();
}

mw_server_t::~mw_server_t() {
    m_acceptor.close();
    m_io_context.stop();

    for (auto &[topic, subscribers] : m_topic_to_subscribers) {
        for (auto &sock : subscribers) {
            if (sock->is_open()) {
                boost::system::error_code ec;
                sock->shutdown(tcp::socket::shutdown_both, ec);
                sock->close(ec);
            }
        }
    }
}

void mw_server_t::start_accept() {
    std::cout << " now accepting " << std::endl;
    socket_p new_socket = std::make_shared<tcp::socket>(m_io_context);
    std::cout << "created a new sock ptr" << std::endl;

    m_acceptor.async_accept(*new_socket,
        [this, new_socket](const boost::system::error_code &ec)
        {
            if (!ec) {
                handle_client(new_socket);
            }
            start_accept();
        }
    );
}

void mw_server_t::handle_client(socket_p sock) {
    auto msg_length_buf = 
        std::make_shared<std::array<char, sizeof(uint32_t)>>();
    auto msg_buf = 
        std::make_shared<std::vector<char>>();

    auto msg_callback = [this, msg_buf, sock]
        (const boost::system::error_code &ec, std::size_t _)
    {
        if (ec) {
            handle_disconnect(sock);
            return;
        }
        std::string msg(msg_buf->begin(), msg_buf->end());
        handle_msg(msg, sock);
        handle_client(sock);
    };

    auto length_callback = [this, msg_length_buf, msg_buf, msg_callback, sock]
        (const boost::system::error_code &ec, std::size_t _)
    {
        if (ec) {
            handle_disconnect(sock);
            return;
        }
        uint32_t msg_length_no = *reinterpret_cast<uint32_t*>(
            msg_length_buf->data());
        uint32_t msg_length = ntohl(msg_length_no);
        msg_buf->resize(msg_length);

        boost::asio::async_read(*sock,
                                boost::asio::buffer(*msg_buf),
                                msg_callback);
    };

    boost::asio::async_read(*sock,
                            boost::asio::buffer(*msg_length_buf),
                            length_callback);
}

void mw_server_t::handle_disconnect(socket_p sock) {
    for (auto &[topic, subscribers] : m_topic_to_subscribers) {
        subscribers.erase(sock);
        if (subscribers.empty()) {
            m_topic_to_subscribers.erase(topic);
        }
    }
}

void mw_server_t::handle_msg(const std::string &msg, socket_p sock)
{
    if (msg.starts_with("subscribe")) {
        const std::string topic = msg.substr(msg.find(',') + 1);
       m_topic_to_subscribers[topic].insert(sock);
        return;
    }

    if (msg.starts_with("unsubscribe")) {
        const std::string topic = msg.substr(msg.find(',') + 1);
        auto it = m_topic_to_subscribers.find(topic);
        if (it == m_topic_to_subscribers.end()) {
            return;
        }

        it->second.erase(sock);
        if (it->second.size() == 0) {
            m_topic_to_subscribers.erase(topic);
        }
        return;
    }

    forward_msg(msg);
}

void mw_server_t::forward_msg(const std::string &msg) {
    const std::string topic = message_t::deserialize(msg).m_topic;
    auto it = m_topic_to_subscribers.find(topic);
    if (it == m_topic_to_subscribers.end()) {
        return;
    }

    auto subscribers = it->second;
    for (auto sock : subscribers) {
        uint32_t msg_length_no = htonl(static_cast<uint32_t>(msg.size()));
        std::vector<boost::asio::const_buffer> bufs;
        bufs.emplace_back(
            boost::asio::buffer(&msg_length_no, sizeof(msg_length_no)));
        bufs.emplace_back(boost::asio::buffer(msg));
        boost::asio::async_write(*sock, bufs,
            [](const boost::system::error_code &ec, std::size_t _) {
                if (ec) {
                    std::cerr << "error sending msg: ";
                    std::cerr << ec.message() << std::endl;
                }
            }
        );
    }
}
