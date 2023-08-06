#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>

void
server()
{
    // create a socket
    std::cout << "Socket\n";
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        std::cerr << "Error creating socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);  // use the same port as the server
    // std::cout << serverAddress.sin_family << std::endl;
    // return 2 which is the value of AF_INET constant
    std::cout << serverAddress.sin_port << std::endl;
}

void
implementStreamSocket()
{
    server();
}