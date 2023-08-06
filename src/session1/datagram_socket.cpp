#include "../../include/session1/datagram_socket.h"
void
DatagramSocket::setupSocketServer()
{
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        std::cerr << "Error oping socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    // initialize server address
    bzero((char*)&server_addr, sizeof(server_addr));
    portno = 12345;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portno);

    // bind socket to server address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Error binding socket\n";
        exit(EXIT_FAILURE);
    }
}
void
DatagramSocket::receiveData()
{
    bzero(buffer, 256);

    n = recvfrom(sockfd, buffer, 255, 0, (struct sockaddr*)&client_addr,
                 &client_length);
    if (n < 0)
    {
        std::cerr << "Error binding socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Received " << n << "bytes from "
              << inet_ntoa(client_addr.sin_addr) << ": " << buffer << std::endl;
}
void
DatagramSocket::sendData()
{
    // send a response back to the client
    n = sendto(sockfd, "Hello, client! ", 14, 0, (struct sockaddr*)&client_addr,
               client_length);
    if (n < 0)
    {
        std::cerr << "Error sendign data" << std::endl;
        exit(EXIT_FAILURE);
    }
}
void
DatagramSocket::implementDatagramSocket()
{
    setupSocketServer();
    // revieve data from a client
    client_length = sizeof(client_addr);
    std::cout << "line 45";

    while (true)
    {
        std::cout << "line 47";
        receiveData();
        //

        //     close(sockfd);
    }
}
