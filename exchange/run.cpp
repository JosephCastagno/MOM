#include "exchange.hpp"

std::atomic<bool> stop(false);

void keyboard_input() {
    char input;
    while (true) {
        std::cin >> input;
        if (input == 'q') {
            stop = true;
            break;
        }
    }
}

int main() {
    const std::string mw_ip = "127.0.0.1";
    const int mw_port = 5000;
    exchange_t exchange{mw_ip, mw_port};
    exchange.start();

    std::thread input_thread(keyboard_input);

    while (exchange.is_running()) {
        if (stop) {
            exchange.kill();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if (input_thread.joinable()) {
        input_thread.join();
    }

    return 0;
}

