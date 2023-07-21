#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

// pid_t
// fork_child_process()
// {
//     pid_t child_pid = fork();
//     if (child_pid == 0)
//     {
//         std::cout << "Child process start\n";
//         process_email_data();
//         std::cout << "Child process end\n";

//         exit(EXIT_FAILURE);
//     }

//     return child_pid;
// }
int
main()
{
    int socket_fd, new_socket, c;
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
    pid_t pid = fork();

    if (pid == 0)
    {
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