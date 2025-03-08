#pragma once

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>

#include "../wolfie/message_queue.hpp"

using boost::asio::ip::tcp;

class tcp_conn_t : public std::enable_shared_from_this<tcp_conn_t> {
 private:
     tcp::socket m_socket;
     static std::atomic<int> global_id_counter;
     int m_id;

     tcp_conn_t(boost::asio::io_context &);
 
 public:
     static std::shared_ptr<tcp_conn_t> create(boost::asio::io_context &);
     tcp::socket& socket();
     bool is_open() const;
     const int get_id() const;
     ~tcp_conn_t();
}; // tcp_conn_t
