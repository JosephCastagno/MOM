#include "order_placer.hpp"

int main() {
    const std::string MW_IP = "127.0.0.1";
    const int MW_PORT = 5000;

    const int THRESHOLD_ONE = 5;
    const int THRESHOLD_TWO = 8;

    const std::string PLACER_ONE = "one";
    const std::string PLACER_TWO = "two";

    order_placer_t op_1{MW_IP, MW_PORT, THRESHOLD_ONE, PLACER_ONE};
    order_placer_t op_2{MW_IP, MW_PORT, THRESHOLD_TWO, PLACER_TWO};

    op_1.start();
    op_2.start();

    op_1.join();
    op_2.join();

    return 0;
}
