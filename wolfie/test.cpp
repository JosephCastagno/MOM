#include <iostream>
#include <string>
#include <optional>
#include <thread>
#include <chrono>

#include "actor.hpp"
#include "message_queue.hpp"
#include "../msg/message.hpp"

#include "mw_provider.hpp"

int main() {
    const std::string ip = "127.0.0.1";
    const int port = 8080;


    message_queue_t msg_q1;
    message_queue_t msg_q2;
    std::cout << "initializing mw pro" << std::endl;
    mw_provider_t mw_pro_one = mw_provider_t(msg_q1, ip, port);
    mw_provider_t mw_pro_two = mw_provider_t(msg_q2, ip, port);
    
    message_t pulse = message_t("pulse", pulse_data_t());

    mw_pro_one.subscribe("pulse");
    mw_pro_two.publish(pulse);

    while (msg_q1.empty()) {
        continue;
    }

    message_t msg = msg_q1.dequeue();
    std::cout << "subscriber received: " << std::endl;
    std::cout << msg.serialize() << std::endl;

    std::cout << "non subscriber queue length: " << !msg_q2.empty() << std::endl;
    return 0;
}
