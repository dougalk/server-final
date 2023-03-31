#include "client.h"
#include <mutex>
#include <functional>

using namespace std;

// Constructor
Client::Client(const string &address, int port) : Comms(address, port, false) {
    createSocket();
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    service.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &service.sin_addr);
}

// Destructor
Client::~Client() {}

// Function to create a socket for the client.
void Client::createSocket() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        throw runtime_error("Error creating socket");
    }

    service.sin_family = AF_INET;
    service.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &service.sin_addr);
}

// Function to connect the client to the server.
void Client::connectToServer(){
    int connectRes = connect(sock, (sockaddr *)&service, sizeof(service));
    if (connectRes == -1) {
        // throw runtime_error("Error with connection");
        //new error message
        string error_msg_connecting = "Error with connection: " + string(strerror(errno));
        throw runtime_error(error_msg_connecting);
    }
}

// Client run function
void Client::run() {
    //atomic bool approach 
    try {
        connectToServer();
        cout << "Client> Connected to server" << endl;
        ReceiveMessageWrapper receiveWrapper(this, sock);
        SendMessageWrapper sendWrapper(this, sock);

        std::thread sendThread(std::ref(sendWrapper));
        std::thread receiveThread(std::ref(receiveWrapper));

        sendThread.join();
        cout << "Client> sendMessage thread terminated" << endl;
        receiveThread.join();
        cout << "Client> receiveMessage thread terminated" << endl;
        cout << "Client> Client::run() loop terminated" << endl;
        cout << "Client threads terminated" << endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }


}



