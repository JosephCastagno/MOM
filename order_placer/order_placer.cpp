#include "order_placer.hpp"

namespace topics {
    static const std::string pulse = "pulse";
    static const std::string heartbeat = "heartbeat";
    static const std::string market_data = "market_data";
    static const std::string order = "order";
    static const std::string shutdown = "shutdown";
} // namespace topics 

order_placer_t::order_placer_t(const std::string &mw_ip,
                               const int mw_port,
                               const int threshold,
                               const std::string &placer_name)
    : actor_t(placer_name, mw_ip, mw_port), 
      m_threshold(threshold), 
      m_orders_placed(0)
{}

void order_placer_t::handle_message(const message_t &msg) {
    if (msg.m_topic == topics::heartbeat) {
        handle_heartbeat(msg);
    } else if (msg.m_topic == topics::market_data) {
        handle_market_data(msg);
    } else if (msg.m_topic == topics::shutdown) {
        handle_shutdown(msg);
    }
}

void order_placer_t::handle_shutdown(const message_t &msg) {
    const shutdown_data_t sdown = msg.m_data;
    if (sdown.m_to == m_name) {
        if (sdown.m_from == "kill") {
            actor_t::handle_shutdown();
        } else {
            kill();
        }
    }
}

void order_placer_t::handle_heartbeat(const message_t &msg) {
    const heartbeat_data_t hbeat = msg.m_data;
    const auto now = std::chrono::time_point<std::chrono::system_clock>(
        std::chrono::system_clock::now());

    auto first_leg = std::chrono::duration_cast<std::chrono::milliseconds>(
        hbeat.m_hbeat_ts - hbeat.m_pulse_ts);
    auto second_leg = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - hbeat.m_hbeat_ts);
    std::cout << "Latency Stats Order Placer " << m_name << ":" << std::endl;
    std::cout << "placer --> exchange latency: " << first_leg.count() << " ms" << std::endl;
    std::cout << "exchange --> placer latency: " << second_leg.count() << " ms\n" << std::endl;
}

void order_placer_t::handle_market_data(const message_t &msg) {
    const market_data_t mkt_data = msg.m_data;

    if (mkt_data.m_price > m_threshold) {
        return;
    }

    const int QUANTITY = 1;
    const order_data_t order_data = order_data_t(m_name,
                                                 mkt_data.m_symbol,
                                                 mkt_data.m_price,
                                                 QUANTITY);

    const message_t new_order = message_t(topics::order, order_data); 
    m_mw_pro.publish(new_order);
    m_orders_placed++;

    if (m_orders_placed % 2 == 1) {
        // on odd numbered orders, pulse and test rtt with exchange
        const message_t pulse = message_t(
            topics::pulse,
            pulse_data_t{});
        m_mw_pro.publish(pulse);
    }

    std::cout << "Orders Placed " << m_name << ": " << m_orders_placed << "\n" << std::endl;
    if (m_orders_placed < 10) {
        return;
    }

    kill();
}

void order_placer_t::mw_setup() {
    const std::vector<std::string> topics = {
        topics::heartbeat,
        topics::market_data,
        topics::shutdown
    };

    m_mw_pro.subscribe(topics);
}
