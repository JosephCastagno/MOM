#pragma once

#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "tcp_conn.hpp"

using boost::asio::ip::tcp;

class mw_server_t {
 public:
     mw_server_t(const int);
     ~mw_server_t();

 private:
     boost::asio::io_context m_io_context;
     tcp::acceptor m_acceptor;
     std::unordered_map<std::string, 
                        std::unordered_set<std::shared_ptr<tcp_conn_t>>> 
         m_topic_to_subs;
     std::unordered_map<int, std::shared_ptr<tcp_conn_t>>
         m_id_to_conn;

     void start_accept();
     void handle_accept(std::shared_ptr<tcp_conn_t>,
                        const boost::system::error_code &);
     void read_msg(std::shared_ptr<tcp_conn_t>);
     void handle_msg(const messaging::envelope &, std::shared_ptr<tcp_conn_t>);
     void forward_msg(const messaging::envelope &);
     void disconnect(std::shared_ptr<tcp_conn_t>);
     void send(std::shared_ptr<tcp_conn_t>, const messaging::envelope &);
}; // mw_server_t

