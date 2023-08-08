#include "../../include/session1/datagram_socket.h"
// Server-side implementation
void
DatagramSocket::setupSocketServer()
{
    // Open a socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        std::cerr << "Error opening socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    bzero(&server_addr, sizeof(server_addr));
    // memset(&server_addr, 0 , sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    client_length = sizeof(client_addr);

    // Bind socket to server address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Error binding socket\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port" << PORT << std::endl;
}

void
DatagramSocket::receiveData()
{
    bzero(buffer, BUFFER_SIZE);

    // Receive data from a client
    int n = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                     (struct sockaddr*)&client_addr, &client_length);
    if (n < 0)
    {
        std::cerr << "Error receiving data" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Server received " << n << " bytes from "
              << inet_ntoa(client_addr.sin_addr) << ": " << buffer << std::endl;
}

void
DatagramSocket::sendData()
{
    // Send a response back to the client
    int n = sendto(sockfd, "Hello, client! ", 14, 0,
                   (struct sockaddr*)&client_addr, client_length);
    if (n < 0)
    {
        std::cerr << "Error sending data" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void
DatagramSocket::implementDatagramServer()
{
    setupSocketServer();

    // Receive data from a client and send a response
    while (true)
    {
        receiveData();
        sendData();
    }
    close(sockfd);
}

// Client-side implementation
void
ClientDatagramSocket::setupSocketClient()
{
    // Create a socket
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
    {
        std::cerr << "Failed to create socket\n";
        exit(EXIT_FAILURE);
    }

    // Set server address details
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_length = sizeof(server_addr);

    // Convert IP address from string to binary form
    if (inet_pton(AF_INET, SERVER_IP, &(server_addr.sin_addr)) <= 0)
    {
        std::cerr << "Invalid address / Address not supported." << std::endl;
        exit(EXIT_FAILURE);
    }
}

void
ClientDatagramSocket::sendDataClient()
{
    // Send data to the server
    strcpy(buffer, "Hello Server ");
    int n = sendto(clientSocket, buffer, strlen(buffer), 0,
                   (struct sockaddr*)&server_addr, server_length);
    if (n < 0)
    {
        std::cerr << "Failed to send data" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void
ClientDatagramSocket::receiveDataClient()
{
    bzero(buffer, BUFFER_SIZE);
    // Receive data from the server
    int n = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0,
                     (struct sockaddr*)&server_addr, &server_length);
    if (n < 0)
    {
        std::cerr << "Error receiving data" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Client Received " << n << " bytes: " << buffer << std::endl;
}

void
ClientDatagramSocket::implementDatagramClient()
{
    setupSocketClient();

    // Send data to the server and receive a response
    while (true)
    {
        sendDataClient();
        receiveDataClient();
    }
    close(clientSocket);
}

void
implementDatagramSocket()
{
    DatagramSocket datagramsocket;
    ClientDatagramSocket clientdatagramsocket;

    // Create a child process using fork()
    pid_t pid_child = fork();
    if (pid_child < 0)
    {
        std::cerr << "Error fork\n";
        exit(EXIT_FAILURE);
    }

    if (pid_child == 0)
    {
        // Child process runs the client code
        clientdatagramsocket.implementDatagramClient();
    }
    else
    {
        // Parent process runs the server code
        datagramsocket.implementDatagramServer();
    }
}