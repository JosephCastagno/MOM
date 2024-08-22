#include <iostream>
#include <string>
#include <optional>

#include "message.hpp"
#include "message_queue.hpp"

int main() {
    message_t msg("pulse", pulse_data_t());
    message_queue_t mq;
    mq.enqueue(msg);
    auto retval = mq.dequeue();
    if (retval) {
        std::cout << retval.value() << std::endl;
    }
    return 0;

}
