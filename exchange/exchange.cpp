#include "../althea/msg/message.hpp"
#include "exchange.hpp"

static const std::string app_name = "exchange";

namespace topics {
    static const std::string shutdown = "shutdown";
    static const std::string heartbeat = "heartbeat";
    static const std::string pulse = "pulse";
    static const std::string order = "order";
    static const std::string timeout_alert = "alert";
    static const std::string market_data = "market_data";
} // namespace topics


exchange_t::exchange_t(const std::string &mw_ip, const int mw_port)
    : actor_t(app_name, mw_ip, mw_port), 
      m_timeout_pro(timeout_provider_t(m_msg_queue, 1000)),
      m_random_engine(std::random_device{}()),
      m_distribution(1, 10)
{}

void exchange_t::handle_message(const message_t &msg) {
    if (msg.m_topic == topics::pulse) {
        handle_pulse(msg);
    } else if (msg.m_topic == topics::timeout_alert) {
        handle_timeout_alert();
    } else if (msg.m_topic == topics::order) {
        handle_order_received(msg);
    } else {
        handle_shutdown();
    }
}

void exchange_t::handle_pulse(const message_t &msg) {
    // respond to pulse with heartbeat
    const pulse_data_t pulse = msg.m_data;
    const message_t heartbeat = message_t(
        topics::heartbeat,
        heartbeat_data_t(pulse.m_timestamp));
    m_mw_pro.publish(heartbeat);
}

void exchange_t::handle_timeout_alert() {
    // publish fake market data 
    const std::string symbol = "AAPL";
    const int random_ask_price = m_distribution(m_random_engine);
    const std::chrono::time_point<std::chrono::system_clock> ts = 
        msg_utils::parse_ts_w_ms(msg_utils::get_ts_w_ms());
    const message_t mkt_data = message_t(
        topics::market_data,
        market_data_t(symbol, random_ask_price, ts));
    m_mw_pro.publish(mkt_data);
}

void exchange_t::handle_order_received(const message_t &msg) {
    const order_data_t order = msg.m_data;
    const std::string participant = order.m_participant;
    m_participant_to_count[participant]++;

    std::cout << "Received order number " << m_participant_to_count.at(participant) 
              << " from " << participant << std::endl;

    if (order.m_participant == "two" && 
        m_participant_to_count.at(participant) == 5) {
        // shutdown participant 2 after receiving 5 orders
        const message_t shutdown_msg = message_t(
            topics::shutdown,
            shutdown_data_t(participant, app_name));
        m_mw_pro.publish(shutdown_msg);
    }
}


void exchange_t::mw_setup() {
    const std::vector<std::string> topics = {
        topics::pulse,
        topics::order
    };

    m_mw_pro.subscribe(topics);
}

void exchange_t::shutdown() {
    m_timeout_pro.shutdown();
    actor_t::shutdown();
}
