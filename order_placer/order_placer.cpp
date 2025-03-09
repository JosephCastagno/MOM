#include "order_placer.hpp"
#include "../althea/proto/message_factory.hpp"
#include "../althea/proto/message.pb.h"

namespace topics {
    static const std::string pulse = "pulse";
    static const std::string heartbeat = "heartbeat";
    static const std::string market = "market";
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

void order_placer_t::handle_message(const messaging::envelope &msg) {
    if (msg.topic() == topics::heartbeat) {
        handle_heartbeat(msg);
    } else if (msg.topic() == topics::market) {
        handle_market_data(msg);
    } else if (msg.topic() == topics::shutdown) {
        handle_shutdown(msg);
    }
}

void order_placer_t::handle_shutdown(const messaging::envelope &msg) {
    const auto sdown = msg.shutdown_data();
    // FIXME shutdown broadcasted to all actors, not just recipient
    if (sdown.to() == m_name) {
        actor_t::shutdown();
    }
}

void order_placer_t::handle_heartbeat(const messaging::envelope &msg) {
    const auto hbeat = msg.heartbeat_data();

    auto now = std::chrono::system_clock::now();
    auto pulse_ts = timestamp::timestamp_to_timepoint(hbeat.pulse_ts());
    auto hbeat_ts = timestamp::timestamp_to_timepoint(hbeat.hbeat_ts());

    auto first_leg = std::chrono::duration_cast<std::chrono::milliseconds>(
        hbeat_ts - pulse_ts);
    auto second_leg = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - hbeat_ts);
    std::cout << "Latency Stats Order Placer " << m_name << ":" << std::endl;
    std::cout << "placer --> exchange latency: " << first_leg.count() << " ms" << std::endl;
    std::cout << "exchange --> placer latency: " << second_leg.count() << " ms\n" << std::endl;
}

void order_placer_t::handle_market_data(const messaging::envelope &msg) {
    const auto mkt_data = msg.market_data();

    if (mkt_data.price() > m_threshold) {
        return;
    }

    const int QUANTITY = 1;
    const messaging::envelope order = message_factory::create_order(
        m_name,
        mkt_data.symbol(),
        mkt_data.price(),
        QUANTITY);

    m_mw_pro.publish(order);
    m_orders_placed++;

    if (m_orders_placed % 2 == 1) {
        // on odd numbered orders, pulse and test rtt with exchange
        m_mw_pro.publish(message_factory::create_pulse());
    }

    std::cout << "Orders Placed " << m_name << ": " << m_orders_placed << "\n" << std::endl;
    if (m_orders_placed >= 10) {
        actor_t::shutdown();
    }
}

void order_placer_t::mw_setup() {
    const std::vector<std::string> topics = {
        topics::heartbeat,
        topics::market,
        topics::shutdown
    };

    m_mw_pro.subscribe(topics);
}
