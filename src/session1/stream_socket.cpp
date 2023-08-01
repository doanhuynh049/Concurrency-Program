#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>

void
server()
{
    // create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);  // use the same port as the server
}

void
implementStreamSocket()
{
    server();
}