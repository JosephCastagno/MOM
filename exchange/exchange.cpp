#include "../althea/msg/message.hpp"
#include "exchange.hpp"

static const std::string app_name = "exchange";

namespace topics {
    static const std::string shutdown = "shutdown";
    static const std::string heartbeat = "heartbeat";
    static const std::string pulse = "pulse";
    static const std::string order = "order";
    static const std::string timeout_alert = "alert";
    static const std::string market_data = "market";
} // namespace topics


exchange_t::exchange_t(const std::string &mw_ip, const int mw_port)
    : actor_t(app_name, mw_ip, mw_port), 
      m_timeout_pro(timeout_provider_t(m_msg_queue, 1000)),
      m_random_engine(std::random_device{}()),
      m_distribution(1, 10)
{}

void exchange_t::handle_message(const messaging::envelope &msg) {
    if (msg.topic() == topics::pulse) {
        handle_pulse(msg);
    } else if (msg.topic() == topics::timeout_alert) {
        handle_timeout_alert();
    } else if (msg.topic() == topics::order) {
        handle_order_received(msg);
    } else {
        handle_shutdown();
    }
}

void exchange_t::handle_pulse(const messaging::envelope &msg) {
    // respond to pulse with heartbeat
    const auto pulse = msg.pulse_data();
    const messaging::envelope hbeat = message_factory::create_heartbeat(
        pulse.timestamp());
    m_mw_pro.publish(hbeat);
}

void exchange_t::handle_timeout_alert() {
    // publish fake market data 
    const std::string symbol = "AAPL";
    const int random_ask_price = m_distribution(m_random_engine);

    int64_t ts = timestamp::get_current_timestamp();
    const messaging::envelope mkt_data = message_factory::create_market(
        symbol,
        random_ask_price,
        ts);
    m_mw_pro.publish(mkt_data);
}

void exchange_t::handle_order_received(const messaging::envelope &msg) {
    const auto order = msg.order_data();
    const std::string participant = order.participant();
    m_participant_to_count[participant]++;

    std::cout << "Received order number " << m_participant_to_count.at(participant) 
              << " from " << participant << std::endl;

    if (participant == "two" && 
        m_participant_to_count.at(participant) == 5) {
        // shutdown participant 2 after receiving 5 orders
        const messaging::envelope shutdown = message_factory::create_shutdown(
            participant,
            app_name);
        m_mw_pro.publish(shutdown);
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
