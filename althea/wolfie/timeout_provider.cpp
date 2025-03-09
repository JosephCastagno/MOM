#include "timeout_provider.hpp"

timeout_provider_t::timeout_provider_t(message_queue_t &msg_q, const int ts)
    : m_msg_queue(msg_q), 
      m_ts(ts), 
      m_paused(false), 
      m_running(true)
{
    m_worker = std::thread([this] { run(); });
}

timeout_provider_t::~timeout_provider_t() {
    shutdown();
}

void timeout_provider_t::run() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_running) {

        // if paused, wait until resumed or shutdown
        m_cv.wait(lock, [this]() { return !m_paused || !m_running; });
        if (!m_running) break;

        // wait for m_ts milliseconds, wake early if paused or shutdown
        auto predicate = [this](){ return m_paused || !m_running; };
        bool timeout = !m_cv.wait_for(lock, 
                                      std::chrono::milliseconds(m_ts),
                                      predicate);
        if (!m_running) break;
        if (!m_paused && timeout) {
            const messaging::envelope msg = message_factory::create_alert(
            "timeout provider");
            m_msg_queue.enqueue(msg);
        }
    }
}

void timeout_provider_t::pause() {
    if (!m_paused) {
        m_paused = true;
    }
}

void timeout_provider_t::resume() {
    if (m_paused) {
        m_paused = false;
    }
}

void timeout_provider_t::shutdown() {
    if (!m_running) return;

    m_running = false;
    if (m_worker.joinable()) {
        m_worker.join();
    }
}
