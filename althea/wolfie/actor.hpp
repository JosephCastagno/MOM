#pragma once

#include <atomic>
#include <thread>
#include <future>

#include "message_queue.hpp"
#include "mw_provider.hpp"

class actor_t {
 protected:
     std::string m_name;
     message_queue_t m_msg_queue;
     mw_provider_t m_mw_pro;
     std::thread m_worker;

     std::atomic<bool> m_running;

     void consume();
     void shutdown();
     void handle_shutdown();
     virtual void handle_message(const messaging::envelope &msg) = 0;
     virtual void mw_setup() = 0;
 public:
     actor_t(std::string name, const std::string &ip, const int port);
     virtual ~actor_t();

     void start();
     void enqueue(messaging::envelope msg);
     bool is_running() const;
     void join();
}; // actor_t
