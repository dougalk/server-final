#include "server.h"
#include <iostream>

int main() {
    int port = 5400;

    try {
        Server server(port);
        server.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
