#include "order_placer.hpp"

int main() {
    const std::string MW_IP = "128.220.224.100";
    const int MW_PORT = 5000;

    const int THRESHOLD_ONE = 5;
    const int THRESHOLD_TWO = 8;

    const std::string PLACER_ONE = "one";
    const std::string PLACER_TWO = "two";

    order_placer_t op_1{MW_IP, MW_PORT, THRESHOLD_ONE, PLACER_ONE};
    order_placer_t op_2{MW_IP, MW_PORT, THRESHOLD_TWO, PLACER_TWO};

    op_1.start();
    op_2.start();

    while (op_1.is_running() || op_2.is_running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
