#include <iostream>

#include "tcp_conn.hpp"

std::shared_ptr<tcp_conn_t> tcp_conn_t::create(
    boost::asio::io_context &ctx)
{
    return std::shared_ptr<tcp_conn_t>(new tcp_conn_t(ctx));
}

tcp::socket& tcp_conn_t::socket() {
    return m_socket;
}

tcp_conn_t::tcp_conn_t(boost::asio::io_context &ctx)
    : m_socket(ctx) {std::cout << "connection created: " << std::endl;}

void tcp_conn_t::write(const std::string &msg) {
    uint32_t msg_len_no = htonl(static_cast<uint32_t>(msg.size()));
    const std::vector<boost::asio::const_buffer> bufs {
        boost::asio::buffer(&msg_len_no, sizeof(msg_len_no)),
        boost::asio::buffer(msg) 
    };

    std::cout << "about to write " << msg << std::endl;

    auto write_callback = 
        [](const boost::system::error_code &ec, std::size_t n) {
            if (ec) {
                std::cerr << ec.message() << std::endl;
            } else {
                std::cout << n << " bytes written" << std::endl;
            }
        };

    boost::asio::async_write(m_socket, bufs, write_callback);
}

tcp_conn_t::~tcp_conn_t() {

    std::cout << "conn destroyed: " << std::endl;
}
