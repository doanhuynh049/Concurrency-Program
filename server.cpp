#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

constexpr int MAX_CLIENTS = 10;

// function to handle a client connection
void handle_client(int client_socket) {
    char buffer[1024];
    int read_size;
    while ((read_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        std::cout << "received: " << buffer << std::endl;
        send(client_socket, buffer, read_size, 0);
        std::memset(buffer, 0, sizeof(buffer));
    }
    if (read_size == 0) {
        std::cout << "client disconnected" << std::endl;
    } else if (read_size == -1) {
        std::cerr << "recv failed" << std::endl;
    }
    close(client_socket);
}

int main(int argc, char *argv[]) {
    int server_socket, client_socket;
    sockaddr_in server_address, client_address;
    socklen_t address_size = sizeof(sockaddr_in);
    pid_t child_pid;

    // create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "socket creation failed" << std::endl;
        return 1;
    }

    // set server address and port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // bind the socket to the address and port
    if (bind(server_socket, (sockaddr *)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "bind failed" << std::endl;
        return 1;
    }

    // listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        std::cerr << "listen failed" << std::endl;
        return 1;
    }

    // handle incoming connections in separate processes
    while (true) {
        client_socket = accept(server_socket, (sockaddr *)&client_address, &address_size);
        if (client_socket == -1) {
            std::cerr << "accept failed" << std::endl;
            continue;
        }
        child_pid = fork();
        if (child_pid == 0) {
            // child process
            close(server_socket);
            handle_client(client_socket);
            exit(0);
        } else if (child_pid > 0) {
            // parent process
            close(client_socket);
        } else {
            std::cerr << "fork failed" << std::endl;
        }
        // reap zombie children
        while (waitpid(-1, NULL, WNOHANG) > 0) {}
    }

    return 0;
}
