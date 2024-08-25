#include <boost/program_options.hpp>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;

    po::options_description desc("allowed options");
    desc.add_options()
        ("help,h", "show help message",
         "port,p", po::value<int>()->required(), "middleware port");

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }

        po::notify(vm);
    } catch (const po::error &e) {
        std::cerr << "err: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    int port = vm["port"].as<int>();

    // start middleware with port
}
