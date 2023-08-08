#include "../../include/session1/unix_socket.h"
void
UnixSocketServer::setupUnixSocketServer()
{
    // create a unix socket
    serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        throw std::runtime_error("Error opening socket");
    }
    // setup server address
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, "mysocket",
            sizeof(serverAddress.sun_path) - 1);
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress),
             server_length)
        == -1)
    {
        std::runtime_error("Failed to bind socket.");
    }
    if (listen(serverSocket, 1) == 1)
    {
        std::cerr << "Failed to listen for connections.\n";
    }
    std::cout << "Server is listening for connections\n";
    int clientSocket =
        accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress),
               &client_length);
    if (clientSocket == -1)
    {
        throw std::runtime_error("Failed to accept client connection.\n");
    }
    std::cout << "Client connected\n";
}
void
UnixSocketServer::executeUnixSocketServer()
{
    setupUnixSocketServer();
}

void
UnixSocketClient::setupUnixSocketClient()
{
    if ((clientSocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        throw std::runtime_error("Failed to create socket.\n");
    }
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, "mysocket",
            sizeof(serverAddress.sun_path) - 1);
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress),
                server_length)
        == -1)
    {
        throw std::runtime_error("Failed to connect to server\n");
    }
    std::cout << "Connected to server\n";
}

void
UnixSocketClient::executeUnixSocketClient()
{
    setupUnixSocketClient();
}

void
executeUnixSocket()
{
    UnixSocketClient unixsocketclient;
    UnixSocketServer unixsocketserver;
    pid_t pid_child = fork();
    if (pid_child < 0)
    {
        std::cerr << "Error fork\n";
    }
    else if (pid_child == 0)
    {
        try
        {
            unixsocketclient.executeUnixSocketClient();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Client Error: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        try
        {
            unixsocketserver.executeUnixSocketServer();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Server Error: " << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}