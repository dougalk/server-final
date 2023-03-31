#ifndef COMMS_H
#define COMMS_H

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <functional>
#include <atomic>


#define BUFFER_SIZE 4096

class Comms {
public:
    // Constructor.
    Comms(const std::string &address, int port, bool isServer);

    // Function to receive messages.
    void receiveMessage(int connection);

    // Function to send messages.
    void sendMessage(int connection);

    // Function to terminate the connectio
    void terminate();

    // Because of issues with threads, I have to use a wrapper function to call the receiveMessage function
    // receieveMessage cannot be used directly as a thread function because it is a member function of the Comms class
    struct ReceiveMessageWrapper {
        Comms *comms;
        int connection;

        //adding constructor
        ReceiveMessageWrapper(Comms *comms, int connection) : comms(comms), connection(connection) {}

        void operator()() {
            comms->receiveMessage(connection);
        }
    };

    //same as above but for sendMessage
    struct SendMessageWrapper {
        Comms *comms;
        int connection;

        //adding constructor
        SendMessageWrapper(Comms *comms, int connection) : comms(comms), connection(connection) {}

        void operator()() {
            comms->sendMessage(connection);
        }
    };

protected:
    // Function to create a socket.
    void createSocket();

    // Function to bind the socket.
    void bindSocket(int port);

    // Function to close the socket.
    void closeSocket();

    //shouldContinue 
    bool shouldContinue = true;

    //quit not working
    std::mutex shouldContinueMutex;
    std::condition_variable shouldContinueCond;

    //new atmomic approach
       // shouldContinue is used to determine whether the receiveMessage function should continue
    //shouldContinue is being accessed and modified by multiple threads concurrently
    std::atomic<bool> shouldContinueAtomic;

    int sock;
    bool isServer;

    std::string address;
    int port;
};

#endif