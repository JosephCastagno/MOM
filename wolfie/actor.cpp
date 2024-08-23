#include "actor.hpp"

#include <iostream>

actor_t::actor_t(std::string name) 
    : m_name(std::move(name)), m_running(false) {}

actor_t::~actor_t() {
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

void actor_t::operator()(const pulse_data_t &data) {
    std::cout << "received pulse" << m_name << std::endl;
}

void actor_t::operator()(const shutdown_data_t &data) {
    std::cout << "received shutdown" << std::endl;
    m_running = false;
}

void actor_t::consume() {
    while (m_running) {
        message_t msg = m_msg_queue.dequeue();
        std::visit([this](auto&& data) { this->operator()(data); },
                   msg.m_data);
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
