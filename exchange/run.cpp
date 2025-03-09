#include "exchange.hpp"

int main() {
    const std::string mw_ip = "127.0.0.1";
    const int mw_port = 5000;
    exchange_t exchange{mw_ip, mw_port};
    exchange.start();
    exchange.join();
    return 0;
}

