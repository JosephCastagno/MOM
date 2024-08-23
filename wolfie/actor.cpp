#include <iostream>

#include "actor.hpp"

actor_t::actor_t(std::string name) 
    : m_name(std::move(name)), m_running(false) {}

actor_t::~actor_t() {
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

void actor_t::handle_pulse(const pulse_data_t &data) {
    std::cout << "received pulse" << m_name << std::endl;
}

void actor_t::handle_shutdown(const shutdown_data_t &data) {
    std::cout << "received shutdown" << std::endl;
    m_running = false;
}

void actor_t::consume() {
    while (m_running) {
        message_t msg = m_msg_queue.dequeue();
        if (msg.m_topic == "pulse") {
            handle_pulse(msg_utils::deserialize_msg<pulse_data_t>(msg.m_data));
        } else if (msg.m_topic == "shutdown") {
            handle_shutdown(
                msg_utils::deserialize_msg<shutdown_data_t>(msg.m_data));
        }
    }
}

void actor_t::start() {
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
