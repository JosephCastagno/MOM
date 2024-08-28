#include <csignal>
#include <iostream>
#include <string>

#include "mw_server.hpp"

int main(int argc, char* argv[]) {
    try {
        const int port = std::stoi(argv[1]);
        mw_server_t serv = mw_server_t(port);

    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
