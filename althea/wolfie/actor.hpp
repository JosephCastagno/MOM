#pragma once

#include <atomic>
#include <thread>

#include "message_queue.hpp"
#include "mw_provider.hpp"

class actor_t {
 protected:
     std::string m_name;
     message_queue_t m_msg_queue;
     std::atomic<bool> m_running;
     std::thread m_worker;
     mw_provider_t m_mw_pro;

     void consume();
     void shutdown();
     virtual void handle_message(const message_t &msg) = 0;
     virtual void mw_setup() = 0;
 public:
     actor_t(std::string name, const std::string &ip, const int port);
     ~actor_t();

     void start();
     void enqueue(message_t msg);
     bool is_running() const;
}; // actor_t
