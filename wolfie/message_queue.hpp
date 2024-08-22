#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>

#include "message.hpp"

class message_queue_t {
 private:
     std::queue<message_t> m_queue;
     std::mutex m_mutex;

 public:
     void enqueue(const message_t& msg) {
         std::lock_guard<std::mutex> lock(m_mutex);
         m_queue.push(msg);
     }

     std::optional<message_t> dequeue() {
         std::lock_guard<std::mutex> lock(m_mutex);
         
         if (m_queue.empty()) {
             return std::nullopt; 
         }

         message_t msg = m_queue.front();
         m_queue.pop();
         return msg; 
     }
};
