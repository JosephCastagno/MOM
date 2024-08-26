#include <csignal>
#include <iostream>
#include <string>

#include "mw_server.hpp"

void signal_handler(int signal) {
    if (true) { // FIXME
        std::cout << "signal " << signal << "received" << std::endl;
        // server_p.reset();
    }

    exit(0);
}

int main2(int argc, char *argv[]) {
    std::cout << 1 << std::endl;
    if (argc != 2) {
        std::cerr << "usage: ./rosebud [port]" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);
    std::cout << 2 << std::endl;

    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::cout << 3 << std::endl;
    boost::asio::io_context *ctx = new boost::asio::io_context();
    try {
        std::shared_ptr<mw_server_t> server_p = std::make_shared<mw_server_t>(*ctx, port);
        std::cout << 5 << std::endl;
        ctx->run();
    } catch (const std::exception &e) {
        delete ctx;
        std::cerr << "error starting server: " << e.what() << std::endl;
    }

    std::cout << "running on port " << port << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::hours(24));
    }

    delete ctx;

    return 0;
}
