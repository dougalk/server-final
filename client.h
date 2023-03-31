#ifndef CLIENT_H
#define CLIENT_H

#include "comms.h"

// Client class derived from the Comms class.
class Client : public Comms {
public:
    // Constructor for the Client class.
    Client(const std::string &address, int port);

    // Destructor for the Client class.
    ~Client();

    // Function to connect the client to the server.
    void connectToServer();

    // Function to run the client.
    void run();

    // Function to create a socket for the client.
    void createSocket();

private:
    // Define service (which was defined in the main function before
    sockaddr_in service;

};

#endif
