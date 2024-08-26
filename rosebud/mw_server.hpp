#pragma once

#include <boost/asio.hpp>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>

using boost::asio::ip::tcp;
using socket_p = std::shared_ptr<tcp::socket>;

class mw_server_t {
 private:
     tcp::acceptor m_acceptor;
     std::unordered_map<std::string, std::unordered_set<socket_p>>
         m_topic_to_subscribers;
     boost::asio::io_context &m_io_context;

     void start_accept();
     void handle_client(socket_p);
     void handle_msg(const std::string &, socket_p);
     void forward_msg(const std::string &);
     void handle_disconnect(socket_p);

 public:
     mw_server_t(boost::asio::io_context &, const int);
     ~mw_server_t();
}; // mw_server_t
