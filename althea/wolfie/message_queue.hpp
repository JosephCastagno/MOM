#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

#include "../proto/message_factory.hpp"
#include "../proto/message.pb.h"

class message_queue_t {
 private:
     std::queue<messaging::envelope> m_queue;
     std::mutex m_mutex;
     std::condition_variable m_cond_var;

 public:
     void enqueue(const messaging::envelope &msg) {
         {
            std::lock_guard<std::mutex> lock(m_mutex);
            // FIXME less rot
            if (msg.topic() == "shutdown") {
                auto sdown = msg.shutdown_data();
                if (sdown.from() == "kill") {
                    // "clear" the message queue
                    m_queue = std::queue<messaging::envelope>{};
                }
            }
            m_queue.push(msg);
         }
         m_cond_var.notify_one();
     }

     messaging::envelope dequeue() {
         std::unique_lock<std::mutex> lock(m_mutex);
         
         m_cond_var.wait(lock, [this]() { return !m_queue.empty();});

         messaging::envelope msg = m_queue.front();
         m_queue.pop();
         return msg;
     }

     bool empty() {
         std::lock_guard<std::mutex> lock(m_mutex);
         return m_queue.empty();
     }

     size_t size() {
         std::lock_guard<std::mutex> lock(m_mutex);
         return m_queue.size();
     }
             
};
