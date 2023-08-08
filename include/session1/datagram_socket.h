#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#define BUFFER_SIZE 1024
#define PORT 8080
#define SERVER_IP "127.0.0.1"
class DatagramSocket
{
private:
    int sockfd, portno, n;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_length;
    char buffer[BUFFER_SIZE];

public:
    void
    setupSocketServer();
    void
    implementDatagramServer();
    void
    sendData();
    void
    receiveData();
};

class ClientDatagramSocket
{
private:
    int clientSocket;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    socklen_t server_length;

public:
    void
    setupSocketClient();
    void
    sendDataClient();
    void
    receiveDataClient();
    void
    implementDatagramClient();
};

void
implementDatagramSocket();