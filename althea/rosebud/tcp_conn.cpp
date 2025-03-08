#include <iostream>

#include "tcp_conn.hpp"
#include "../proto/message.pb.h"

std::atomic<int> tcp_conn_t::global_id_counter{0};

std::shared_ptr<tcp_conn_t> tcp_conn_t::create(
    boost::asio::io_context &ctx)
{
    return std::shared_ptr<tcp_conn_t>(new tcp_conn_t(ctx));
}

tcp::socket& tcp_conn_t::socket() {
    return m_socket;
}

bool tcp_conn_t::is_open() const {
    return m_socket.is_open();
}

const int tcp_conn_t::get_id() const {
    return m_id;
}

tcp_conn_t::tcp_conn_t(boost::asio::io_context &ctx)
    : m_socket(ctx), m_id(++global_id_counter)
{}

tcp_conn_t::~tcp_conn_t() {
    std::cout << "conn destroyed: " << m_id << std::endl;
}
