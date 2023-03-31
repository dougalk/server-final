#include "client.h"
#include <iostream>

int main() {
    int port = 5400;
    std::string address = "127.0.0.1";

    try {
        Client client(address, port);
        client.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
