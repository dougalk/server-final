#include "comms.h"
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <functional>
#include <thread>
using namespace std;

// constructor with atomic bool
Comms::Comms(const std::string &address, int port, bool server)
    : address(address), port(port), isServer(server), shouldContinue(true), shouldContinueAtomic(true) {}

//Function to send messages..
void Comms::sendMessage(int connection) {
    string message;
    // Send messages until the connection is closed
    while (true) {
        getline(cin, message);
        int sendRes = send(connection, message.c_str(), message.size() + 1, 0);
        if (sendRes == -1) {
            throw runtime_error("Error sending message");
        }        
        cout << (isServer ? "Server> " : "Client> ") << "Sent: " << message << endl;
        if (!isServer && message == "QUIT") {
            terminate();
            // shouldContinueAtomic.store(false);
            cout << (isServer ? "Server> " : "Client> ") << "Messages can no longer be sent" << endl;
            break;
        }


    }
}

//function to receive messages
void Comms::receiveMessage(int connection) {
    char buffer[BUFFER_SIZE];

    // Set timeout for recv() to 1 second
    timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(connection, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));

    // Receive messages until the connection is closed
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(connection, buffer, BUFFER_SIZE, 0);
        if (bytesReceived == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // Timeout occurred, check shouldContinueAtomic and continue
                if (!shouldContinueAtomic.load()) {
                    cout << (isServer ? "Server> " : "Client> ") << "receiveMessage() loop terminated due to shouldContinueAtomic" << endl;
                    break;
                }
                continue;
            }
            throw runtime_error("Error receiving message");
        }
        if (bytesReceived == 0) {
            cout << (isServer ? "Server> " : "Client> ") << "Connection closed" << endl;
            shouldContinueAtomic.store(false);
            terminate();
            break;
        }
        string receivedMessage = string(buffer, bytesReceived);
        cout << (isServer ? "Server> " : "Client> ") << "Received: " << receivedMessage << endl;

        if (isServer && receivedMessage == "QUIT") {
            shouldContinueAtomic.store(false);
            cout << "Server> Client disconnected, setting shouldContinueAtomic to false" << endl;
            terminate();
            break;
        }
    }
}

// Function to create a socket
void Comms::createSocket() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw runtime_error("Error with creating socket");
    }
}

// Function to bind the socket
void Comms::bindSocket(int port) {
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(port);
    service.sin_addr.s_addr = INADDR_ANY;
    //use global bind function from socket API to bind socket to port
    if (::bind(sock, (sockaddr *)&service, sizeof(service)) == -1) {
        // throw runtime_error("Error binding socket");
        string error_msg = "Error binding socket: " + string(strerror(errno));
        throw runtime_error(error_msg);
    }
}

// Function to close the socket
void Comms::closeSocket() {
    close(sock);
}

//Function to terminate using atomic bool
void Comms::terminate() {
    shouldContinueAtomic.store(false);
    cout << (isServer ? "Server> " : "Client> ") << "Terminating..." << endl;
    close(sock);
}
