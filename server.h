#ifndef SERVER_H
#define SERVER_H

#include "comms.h"

class Server : public Comms {
public:
    // Constructor.
    Server(int port);

    // Destructor.
    ~Server();

    // Function to run the server.
    void run();

private:

    // Function to listen on the socket.
    void listenSocket();

    // Function to handle the connected client.
    void handleClient(int clientSocket, Comms *commsInstance);

};

#endif
