#include <iostream>
#include <string>
#include <optional>
#include <thread>
#include <chrono>

#include "actor.hpp"
#include "message_queue.hpp"
#include "../msg/message.hpp"

#include "mw_provider.hpp"

int main2() {
    if (false) {
    auto first = std::make_shared<actor_t>("first");
    auto second = std::make_shared<actor_t>("second");

    first->start();
    second->start();

    int count = 0;
    while (first->is_running() || second->is_running()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        first->enqueue(message_t("pulse", pulse_data_t().serialize()));
        second->enqueue(message_t("pulse", pulse_data_t().serialize()));
        if (count++ == 10) {
            first->enqueue(
                message_t("shutdown", shutdown_data_t("main").serialize()));
        }
        if (count++ > 20) {
            second->enqueue(
                message_t("shutdown", shutdown_data_t("main").serialize()));
        }
    }
    }

    const message_t msg = message_t("shutdown", shutdown_data_t("main").serialize());
    std::cout << msg << std::endl;
    std::string serialized = msg.serialize();

    const message_t deserial = message_t::deserialize(serialized);
    std::cout << deserial << std::endl;

    return 0;

}


int main() {
    const std::string ip = "127.0.0.1";
    const int port = 8080;


    message_queue_t msg_q;
    std::cout << "initializing mw pro" << std::endl;
    mw_provider_t mw_pro = mw_provider_t(msg_q, ip, port);
    const message_t msg = message_t("shutdown", shutdown_data_t("main").serialize());
    mw_pro.publish(msg);
    mw_pro.subscribe("new_topic");
    return 0;
}
