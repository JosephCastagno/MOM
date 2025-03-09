#include <iostream>
#include <string>
#include <optional>
#include <thread>
#include <chrono>

#include "actor.hpp"
#include "message_queue.hpp"
#include "../proto/message.pb.h"
#include "../proto/message_factory.hpp"

#include "mw_provider.hpp"

// changed to main2 when not being tested directly: avoid multiple entry
// points error
int main2() {
    const std::string ip = "127.0.0.1";
    const int port = 5000;


    message_queue_t msg_q1; 
    message_queue_t msg_q2; 
    std::cout << "initializing mw pro" << std::endl;
    mw_provider_t mw_pro_one = mw_provider_t(msg_q1, ip, port);
    mw_provider_t mw_pro_two = mw_provider_t(msg_q2, ip, port);
    
    messaging::envelope pulse = message_factory::create_pulse();

    mw_pro_one.subscribe("pulse");
    mw_pro_two.publish(pulse);

    while (msg_q1.empty()) {
        continue;
    }

    messaging::envelope msg = msg_q1.dequeue();
    std::cout << "subscriber received: " << std::endl;
    std::cout << msg.DebugString() << std::endl;

    std::cout << "non subscriber queue length: " << msg_q2.size() << std::endl;
    return 0;
}
