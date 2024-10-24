#pragma once 

#include <chrono>
#include <thread>

#include "message_queue.hpp"

class timeout_provider_t {
 private:
     message_queue_t &m_msg_queue;
     int m_ts;

     std::atomic<bool> m_running;
     std::atomic<bool> m_paused;
     std::thread m_worker;

 public:
     timeout_provider_t(message_queue_t &, const int);
     ~timeout_provider_t();

     void run();
     void pause();
     void resume();
     void shutdown();
}; // timeout_provider_t
