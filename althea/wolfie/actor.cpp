#include <iostream>

#include "actor.hpp"

actor_t::actor_t(std::string name, const std::string &ip, const int port) 
    : m_name(std::move(name)), 
      m_mw_pro(m_msg_queue, ip, port), 
      m_running(false)
{}

actor_t::~actor_t() {
    shutdown();
}

void actor_t::kill() {
    const messaging::envelope shutdown = message_factory::create_shutdown(
        m_name, 
        "kill");

    enqueue(shutdown);
}

void actor_t::shutdown() {
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

void actor_t::consume() {
    while (m_running) {
        messaging::envelope msg = m_msg_queue.dequeue();
        handle_message(msg);
    }
    m_mw_pro.shutdown();
}

void actor_t::start() {
    if (m_running) return;

    mw_setup();

    m_running = true;
    m_worker = std::thread([this] { consume(); });
}

void actor_t::enqueue(messaging::envelope msg) {
    if (!m_running) {
        return;
    }

    m_msg_queue.enqueue(msg);
}

bool actor_t::is_running() const {
    return m_running;
}

void actor_t::handle_shutdown() {
    m_running = false;
}
