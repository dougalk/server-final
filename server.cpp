#include "server.h"
#include <arpa/inet.h>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


//quit not working
#include <mutex>

//quit not working version 3
#include <functional>
#include <chrono>


using namespace std;

// Server constructor.
Server::Server(int port) : Comms("", port, true) {
    createSocket();
    bindSocket(port);
    listenSocket();
}

// Server destructor.
Server::~Server() {
    closeSocket();
}

// Function to listen on the socket.
void Server::listenSocket() {
    if (listen(sock, SOMAXCONN) == -1) {
        throw runtime_error("Error listening on socket");
    }
}

// Server run function
void Server::run() {
    cout << "Server listening..." << endl;
    sockaddr_storage client;
    socklen_t clientSize = sizeof(client);

    int sock = this->sock; // Add this line to access the socket from the Comms class

    while (shouldContinueAtomic.load()) {
        int clientSocket = accept(sock, (sockaddr *)&client, &clientSize);
        if (clientSocket == -1) {
            throw runtime_error("Error accepting client connection");
        } else {
            cout << "Client connected" << endl;
            handleClient(clientSocket, this);
            cout << "Client disconnected" << endl;
        }
        close(clientSocket);

        if (!shouldContinueAtomic.load()) {
            cout << "Server> Exiting Server::run() loop" << endl;
            goto exit_loop;
        }
 
exit_loop:
    cout << "Server stopped listening" << endl;
    cout << "Server> Server::run() loop terminated" << endl;
}
}

// Function to handle the connected client
void Server::handleClient(int clientSocket, Comms *commsInstance) {
    Comms::ReceiveMessageWrapper receiveWrapper(commsInstance, clientSocket);
    Comms::SendMessageWrapper sendWrapper(commsInstance, clientSocket);

    std::thread sendThread(std::ref(sendWrapper));
    std::thread receiveThread(std::ref(receiveWrapper));

    sendThread.join();
    cout << "Server> sendMessage thread terminated" << endl;
    receiveThread.join();
    cout << "Server> receiveMessage thread terminated" << endl;
}








