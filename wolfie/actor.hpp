#pragma once

#include <atomic>
#include <thread>

#include "message_queue.hpp"

class actor_t {
 private: // this will be protected
     std::string m_name;
     message_queue_t m_msg_queue;
     std::atomic<bool> m_running;
     std::thread m_worker;

     void operator()(const pulse_data_t& data) {
         std::cout << "received pulse " << m_name << std::endl;
     }

     void operator()(const shutdown_data_t& data) {
         std::cout << "received shutdown" << std::endl;
         m_running = false;
     }

     void consume() {
         while (m_running) {
             message_t msg = m_msg_queue.dequeue();
             std::visit([this](auto&& data) { this->operator()(data); }, 
                        msg.m_data);
         }
     }

 public:
     actor_t(std::string name) : m_name(std::move(name)), m_running(false) {}

     ~actor_t() {
         if (m_worker.joinable()) {
             m_worker.join();
         }
     }


     void start() {
        m_running = true;
        m_worker = std::thread([this] { consume(); });
     }

     void enqueue(message_t msg) {
         if (!m_running) {
             return;
         }

         m_msg_queue.enqueue(msg);
     }

     bool is_running() const {
         return m_running;
     }
}; // actor_t
