#include "timeout_provider.hpp"

timeout_provider_t::timeout_provider_t(message_queue_t &msg_q, const int ts)
    : m_msg_queue(msg_q), m_ts(ts), m_paused(false), m_running(true)
{
    m_worker = std::thread([this] { run(); });
}

timeout_provider_t::~timeout_provider_t() {
    m_running = false;
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

void timeout_provider_t::run() {
    while (m_running) {
        if (m_paused) {
            continue;
        }

        message_t alert_msg = message_t("alert", "");
        m_msg_queue.enqueue(alert_msg);

        std::this_thread::sleep_for(std::chrono::milliseconds(m_ts));
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
