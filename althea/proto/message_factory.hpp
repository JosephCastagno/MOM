#pragma once

#include <iostream>
#include <stdexcept>
#include "message.pb.h"
#include "timestamp.hpp"

namespace message_factory {

    inline messaging::envelope create_subscribe(const std::string &topic) {
        if (topic.empty()) throw new std::runtime_error("empty topic");

        messaging::envelope env;
        env.set_topic("subscribe");
        auto *sub_data = env.mutable_subscribe_data();
        sub_data->set_topic(topic);
        
        return env;
    }

    inline messaging::envelope create_unsubscribe(const std::string &topic) {
        if (topic.empty()) throw new std::runtime_error("empty topic");

        messaging::envelope env;
        env.set_topic("unsubscribe");
        auto *unsub_data = env.mutable_unsubscribe_data();
        unsub_data->set_topic(topic);
        
        return env;
    }

    inline messaging::envelope create_pulse() {
        messaging::envelope env;
        env.set_topic("pulse");
        auto *pulse_data = env.mutable_pulse_data();
        pulse_data->set_timestamp(timestamp::get_current_timestamp());

        return env;
    }

    inline messaging::envelope create_shutdown(const std::string &to,
                                               const std::string &from)
    {
        messaging::envelope env;
        env.set_topic("shutdown");
        auto *sdown_data = env.mutable_shutdown_data();
        sdown_data->set_to(to);
        sdown_data->set_from(from);

        return env;
    }

    inline messaging::envelope create_heartbeat(const int64_t pulse_ts)
    {
        messaging::envelope env;
        env.set_topic("heartbeat");
        auto *hbeat_data = env.mutable_heartbeat_data();
        hbeat_data->set_pulse_ts(pulse_ts);
        hbeat_data->set_hbeat_ts(timestamp::get_current_timestamp());

        return env;
    }

    inline messaging::envelope create_market(const std::string &symbol,
                                             const int32_t price,
                                             const int timestamp) // XXX create timestamp w/in?
    {
        messaging::envelope env;
        env.set_topic("market");
        auto *market_data = env.mutable_market_data();
        market_data->set_symbol(symbol);
        market_data->set_price(price);
        market_data->set_timestamp(timestamp);

        return env;
    }
    
    inline messaging::envelope create_order(const std::string &participant,
                                            const std::string &symbol,
                                            const int32_t price,
                                            const uint32_t quantity)
    {
        messaging::envelope env;
        env.set_topic("order");
        auto *order_data = env.mutable_order_data();
        order_data->set_participant(participant);
        order_data->set_symbol(symbol);
        order_data->set_price(price);
        order_data->set_quantity(quantity);

        return env;
    }

    inline messaging::envelope create_alert(const std::string &description) {
        messaging::envelope env;
        env.set_topic("alert");
        auto *alert_data = env.mutable_alert_data();
        alert_data->set_description(description);

        return env;
    }

    inline messaging::envelope create_disconnect() {
        messaging::envelope env;
        env.set_topic("disconnect");
        return env;
    }
} // namespace message_factory
