#include <iostream>
#include <string>
#include <optional>

#include "message.hpp"
#include "message_queue.hpp"

int main() {
    message_t msg("user_login", login_data_t("admin", 1).to_xml());
    message_queue_t mq;
    mq.enqueue(msg);
    auto &retval = mq.dequeue();
    if (retval) {
        std::cout << retval.value() << std::endl;
    }
    return 0;

}
