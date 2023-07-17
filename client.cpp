#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int client_socket;
    sockaddr_in server_address;
    char buffer[1024];
    int read_size;

    // create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "socket creation failed" << std::endl;
        return 1;
    }

    // set server address and port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // replace with server IP address
    server_address.sin_port = htons(8080); // replace with server port

    // connect to the server
    if (connect(client_socket, (sockaddr *)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "connect failed" << std::endl;
        return 1;
    }

    // send data to the server
    std::string message = "Hello, server!";
    if (send(client_socket, message.c_str(), message.length(), 0) == -1) {
        std::cerr << "send failed" << std::endl;
        return 1;
    }

    // receive data from the server
    read_size = recv(client_socket, buffer, sizeof(buffer), 0);
    if (read_size > 0) {
        std::cout << "received: " << buffer << std::endl;
    } else if (read_size == 0) {
        std::cout << "server disconnected" << std::endl;
    } else {
        std::cerr << "recv failed" << std::endl;
        return 1;
    }

    close(client_socket);

    return 0;
}
