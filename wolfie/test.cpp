#include <iostream>
#include <string>
#include <optional>
#include <thread>
#include <chrono>

#include "actor.hpp"
#include "message_queue.hpp"
#include "../msg/message.hpp"

int main() {
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

    return 0;

}
