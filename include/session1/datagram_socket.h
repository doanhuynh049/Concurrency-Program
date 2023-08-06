#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

class DatagramSocket
{
private:
    int sockfd, portno, n;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_length;
    char buffer[256];

public:
    void
    setupSocketServer();
    void
    implementDatagramSocket();
    void
    sendData();
    void
    receiveData();
};