#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

#include "message.hpp"

class message_queue_t {
 private:
     std::queue<message_t> m_queue;
     std::mutex m_mutex;
     std::condition_variable m_cond_var;

 public:
     void enqueue(const message_t& msg) {
         {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(msg);
         }
         m_cond_var.notify_one();
     }

     message_t dequeue() {
         std::unique_lock<std::mutex> lock(m_mutex);
         
         m_cond_var.wait(lock, [this]() { return !this->m_queue.empty(); });

         message_t msg = m_queue.front();
         m_queue.pop();
         return msg; 
     }
};
