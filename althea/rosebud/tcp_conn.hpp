#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "../wolfie/message_queue.hpp"

using boost::asio::ip::tcp;

class tcp_conn_t : public std::enable_shared_from_this<tcp_conn_t> {
 private:
     tcp::socket m_socket;

     tcp_conn_t(boost::asio::io_context &);
 
 public:
     static std::shared_ptr<tcp_conn_t> create(boost::asio::io_context &);
     void write(const std::string &);
     tcp::socket& socket();
     ~tcp_conn_t();
}; // tcp_conn_t
