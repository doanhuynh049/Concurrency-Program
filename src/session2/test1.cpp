#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
pid_t
fork_child_process(struct sockaddr_in address)
{
    pid_t pid_child = fork();
    if (pid_child == 0)
    {
        int new_socket;
        // Child process
        // Connect to socket
        new_socket = socket(AF_INET, SOCK_STREAM, 0);
        connect(new_socket, (struct sockaddr*)&address, sizeof(address));

        // Send and receive data
        char send_buffer[] = "Hello from child!";
        send(new_socket, send_buffer, strlen(send_buffer), 0);

        char recv_buffer[1024];
        recv(new_socket, recv_buffer, 1024, 0);
        std::cout << "Received from parent " << recv_buffer << std::endl;
    }
    return pid_child;
}
int
main()
{
    int socket_fd, c;
    struct sockaddr_in address;

    // Create socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    // Bind socket
    bind(socket_fd, (struct sockaddr*)&address, sizeof(address));

    // Fork child process
    pid_t pid_child = fork_child_process(address);
    if (pid_child == -1)
    {
        std::cerr << "Eror forking\n";
        exit(EXIT_FAILURE);
    }
    else
    {
        // Parent process
        // Listen on socket
        listen(socket_fd, 3);

        // Accept connection
        c = accept(socket_fd, NULL, NULL);

        // Send and receive data
        char recv_buffer[1024];
        recv(c, recv_buffer, 1024, 0);
        std::cout << "Received from child " << recv_buffer << std::endl;
        char send_buffer[] = "Hello from parent!";
        send(c, send_buffer, strlen(send_buffer), 0);
    }
}