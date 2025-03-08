#pragma once

#include <boost/asio.hpp>

#include "message_queue.hpp"

class mw_provider_t { 
 private:
     message_queue_t &m_msg_queue;
     std::thread m_worker; 
     boost::asio::io_context m_io_context;
     boost::asio::ip::tcp::socket m_socket;
     boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
     std::atomic<bool> m_running;

     void receive_msgs(); 
     void read_msg();
     void send_msg(const messaging::envelope &msg);
     void send_shutdown_message();

 public:
     mw_provider_t(message_queue_t &msg_q, const std::string &ip, int port);
     ~mw_provider_t();

     void subscribe(const std::vector<std::string> &);
     void subscribe(const std::string &); 
     void unsubscribe(const std::string &);
     void publish(const messaging::envelope &);
     void shutdown();
}; // mw_provider_t
