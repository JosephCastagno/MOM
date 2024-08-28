#include "order_placer.hpp"

static const std::string app_name = "order_placer";

namespace topics {
    static const std::string pulse = "pulse";
    static const std::string heartbeat = "heartbeat";
    static const std::string market_data = "market_data";
    static const std::string order = "order";
    static const std::string shutdown = "shutdown";
} // namespace topics 

order_placer_t::order_placer_t(const std::string &mw_ip,
                               const int mw_port,
                               const int treshold)
    : actor_t(mw_ip, mw_port), m_threshold(threshold), m_orders_placed(0)
{
    mw_setup();
}

void order_placer_t::handle_message(const message_t &msg) {
    if (msg.m_topic == topics::pulse) {
        handle_pulse(pulse_data_t(msg.m_data));
    } else if (msg.m_topic == topics::market_data) {
        handle_market_data(market_data_t(msg.m_data));
    } else {
        std::cerr "unrecognized msg w/ topic: " << msg.m_topic << std::endl;
    }
}

void order_placer_t::handle_market_data(const market_data_t &mkt_data) {
    if (mkt_data.m_price > m_threshold) {
        return;
    }


    constexpr QUANTITY = 1;
    const order_data_t order_data = order_data_t(app_name,
                                                 mkt_data.m_symbol,
                                                 mkt_data.m_price,
                                                 QUANTITY);

    const message_t new_order = message_t(topics::order, order_data); 
    m_mw_pro.publish(new_order);
    if (m_orders_placed < 10) {
        return;
    }

    const std::string reason = "order goal reached";
    const shutdown_data_t sdown = shutdown_data_t(app_name,
                                                  reason);
    const message_t shutdown_msg = message_t(topics::shutdown, sdown);
    m_mw_pro.publish(shutdown_msg);

    shutdown();
}


void order_placer_t::handle_pulse(const pulse_data_t &pulse_data) {
    const message_t msg = message_t(topics::heartbeat, 
                                    heartbeat_data_t(pulse_data.m_timestamp));
    m_mw_pro.publish(msg);
}

void order_placer_t::mw_setup() {
    const std::vector<std::string> topics = {
        topics::pulse,
        topics::market_data
    };

    m_mw_pro.subscribe(topics);
}
