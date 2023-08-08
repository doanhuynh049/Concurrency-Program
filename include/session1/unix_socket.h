#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#define BUFFER_SIZE 1024
class UnixSocketServer
{
private:
    int serverSocket;
    struct sockaddr_un serverAddress;
    struct sockaddr_un clientAddress;

    socklen_t server_length = sizeof(serverAddress);
    socklen_t client_length = sizeof(clientAddress);
    std::string buffer;

public:
    void
    setupUnixSocketServer();
    void
    receiveDataServer();
    void
    sendDataServer();
    void
    executeUnixSocketServer();
};

class UnixSocketClient
{
private:
    int clientSocket;
    struct sockaddr_un serverAddress;
    struct sockaddr_un clientAddress;
    socklen_t server_length = sizeof(serverAddress);
    socklen_t client_length = sizeof(clientAddress);

public:
    void
    setupUnixSocketClient();
    void
    executeUnixSocketClient();
};
void
executeUnixSocket();