#include <iostream>

#include "actor.hpp"

actor_t::actor_t(std::string name, const std::string &ip, const int port) 
    : m_name(std::move(name)), 
      m_mw_pro(m_msg_queue, ip, port), 
      m_running(false) 
{}

actor_t::~actor_t() {
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

void actor_t::shutdown() {
    m_running = false;
}

void actor_t::consume() {
    while (m_running) {
        message_t msg = m_msg_queue.dequeue();
        handle_message(msg);
    }
}

void actor_t::start() {
    if (m_running) {
        return;
    }

    m_running = true;
    m_worker = std::thread([this] { consume(); });
}

void actor_t::enqueue(message_t msg) {
    if (!m_running) {
        return;
    }

    m_msg_queue.enqueue(msg);
}

bool actor_t::is_running() const {
    return m_running;
}
