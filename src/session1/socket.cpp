#include "../../include/session1/socket.h"

#include <sys/socket.h>
#include <sys/un.h>  // socket in Unix
#include <unistd.h>
#include <cstring>
#include <iostream>

// for print error message
#include <errno.h>
#include <string.h>
void
processChild()
{
    /* Open a client socket (same type as the server) */
    /**************************************************/
    int client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1)
    {
        printf("CLIENT: Socket error: %s\n", strerror(errno));
        exit(1);
    }

    /********************************************/
    /* Bind client to an address on file system */
    /********************************************/
    // Note: this binding could be skip if we want only send data to server
    // without receiving
    client_addr.sun_family = AF_UNIX;
    strcpy(client_addr.sun_path, CLIENT_SOCK_PATH);
    int len = sizeof(client_addr);

    unlink(CLIENT_SOCK_PATH);
    rc = bind(client_sock, (struct sockaddr*)&client_addr, len);
    if (rc == -1)
    {
        printf("CLIENT: Client binding error. %s\n", strerror(errno));
        close(client_sock);
        exit(1);
    }

    /****************************************/
    /* Set server address and connect to it */
    /****************************************/
    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, SERVER_SOCK_PATH);
    rc = connect(client_sock, (struct sockaddr*)&server_addr, len);
    if (rc == -1)
    {
        printf("CLIENT: Connect error. %s\n", strerror(errno));
        close(client_sock);
        exit(1);
    }
    printf("CLIENT: Connected to server.\n");

    /**************************/
    /* Send message to server */
    /**************************/
    memset(buf, 0, sizeof(buf));
    strcpy(buf, CLIENT_MSG);
    rc = send(client_sock, buf, sizeof(buf), 0);
    if (rc == -1)
    {
        printf("CLIENT: Send error. %s\n", strerror(errno));
        close(client_sock);
        exit(1);
    }
    printf("CLIENT: Sent a message to server.\n");

    /**************************************/
    /* Listen to the response from server */
    /**************************************/
    printf("CLIENT: Wait for respond from server...\n");
    memset(buf, 0, sizeof(buf));
    rc = recv(client_sock, buf, sizeof(buf), 0);
    if (rc == -1)
    {
        printf("CLIENT: Recv Error. %s\n", strerror(errno));
        close(client_sock);
        exit(1);
    }
    else
        printf("CLIENT: Message received: %s\n", buf);

    printf("CLIENT: Done!\n");

    close(client_sock);
    remove(CLIENT_SOCK_PATH);
}
int
fork_child_process()
{
    pid_t pid_child = fork();
    if (pid_child == 0)
    {
        std::cout << "Child process begin:\n";
        processChild();
        std::cout << "Child process end\n";
        exit(EXIT_SUCCESS);
    }
    return pid_child;
}
void
implementSocket()
{
    pid_t pid_child = fork_child_process();
    if (pid_child > 0)
    {
        std::cout << "Server process begin " << std::endl;
        int backlog = 10;
        int server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_sock == -1)
        {
            std::cout << "SERVER: Error when opening server socket.\n";
            exit(EXIT_FAILURE);
        }
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SERVER_SOCK_PATH);
        int len = sizeof(server_addr);
        unlink(SERVER_SOCK_PATH);
        rc = bind(server_sock, (struct sockaddr*)&server_addr, len);
        if (rc = -1)
        {
            std::cout << "SERVER: Server bind error: " << strerror(errno);
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        rc = listen(server_sock, backlog);
        if (rc == -1)
        {
            printf("SERVER: Listen error: %s\n", strerror(errno));
            close(server_sock);
            exit(1);
        }

        printf("SERVER: Socket listening...\n");
        int client_fd = accept(server_sock, (struct sockaddr*)&client_addr,
                               (socklen_t*)&len);
        if (client_fd == -1)
        {
            printf("SERVER: Accept error: %s\n", strerror(errno));
            close(server_sock);
            close(client_fd);
            exit(1);
        }
        printf("SERVER: Connected to client at: %s\n", client_addr.sun_path);
        printf("SERVER: Wating for message...\n");

        /********************/
        /* Listen to client */
        /********************/
        memset(buf, 0, 256);
        int byte_recv = recv(client_fd, buf, sizeof(buf), 0);
        if (byte_recv == -1)
        {
            printf("SERVER: Error when receiving message: %s\n",
                   strerror(errno));
            close(server_sock);
            close(client_fd);
            exit(1);
        }
        else
            printf("SERVER: Server received message: %s.\n", buf);

        /**********************/
        /* Response to client */
        /**********************/
        printf("SERVER: Respond to the client...\n");
        memset(buf, 0, 256);
        strcpy(buf, SERVER_MSG);
        rc = send(client_fd, buf, strlen(buf), 0);
        if (rc == -1)
        {
            printf("SERVER: Error when sending message to client.\n");
            close(server_sock);
            close(client_fd);
            exit(1);
        }
        printf("SERVER: Done!\n");

        close(server_sock);
        close(client_fd);
        remove(SERVER_SOCK_PATH);
    }
}
