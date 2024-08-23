#pragma once

#include <atomic>
#include <thread>

#include "message_queue.hpp"

class actor_t {
 protected:
     std::string m_name;
     message_queue_t m_msg_queue;
     std::atomic<bool> m_running;
     std::thread m_worker;

     void handle_pulse(const pulse_data_t& data);
     void handle_shutdown(const shutdown_data_t& data);
     void consume();

 public:
     actor_t(std::string name);
     ~actor_t();

     void start();
     void enqueue(message_t msg);
     bool is_running() const;
}; // actor_t
