#pragma once

#include <boost/asio.hpp>
#include <mutex>

#include "message_queue.hpp"

class mw_provider_t { 
 private:
     message_queue_t &m_msg_queue;
     std::thread m_worker; 
     boost::asio::io_context m_io_context;
     boost::asio::ip::tcp::socket m_socket;
     bool m_running;
     std::mutex m_socket_mutex;

     void receive_msgs(); 
     void read_msg();
     void send_msg_with_header(const std::string &msg);

 public:
     mw_provider_t(message_queue_t &msg_q, const std::string &ip, int port);
     ~mw_provider_t();

     void subscribe(const std::vector<std::string> &topics);
     void subscribe(const std::string &topic); 
     void unsubscribe(const std::string &data);
     void publish(const message_t &msg);
}; // mw_provider_t
