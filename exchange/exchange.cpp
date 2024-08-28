
#include "../althea/msg/message.hpp"
#include "exchange.hpp"

static const std::string app_name = "exchange";

namespace topics {
    static const std::string shutdown = "shutdown";
    static const std::string pulse = "pulse";
    static const std::string order = "order";
} // namespace topics


exchange_t::exchange_t(const std::string &mw_ip, const int mw_port)
    : actor_t(app_name, mw_ip, mw_port), m_timeout_pro(m_msg_queue, 1000) 
{
    mw_setup();
}

void parser_t::handle_message(const message_t &msg) {
    if (msg.m_topic == topics::shutdown) {
        if (msg.m_trigger == "order_placer") {
            shutdown();
        }
        return;
    }

    if (msg.m_topic == topics::pulse) {
        // publish heartbeat
        return;
    }

    if (msg.m_topic == topics::order) {
        handle_market_data(market_data_t(msg.m_data));
    }
}

void exchange_t::mw_setup() {
    const std::vector<std::string> topics = {
        topics::shutdown,
        topics::pulse,
        topics::order
    };

    mw_pro.subscribe(topics);
}
