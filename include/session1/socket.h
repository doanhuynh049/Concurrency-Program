#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <sys/un.h>  //socket in Unix
#define SERVER_SOCK_PATH "unix_sock.server"
#define CLIENT_SOCK_PATH "unix_sock.client"
#define SERVER_MSG "HELLO FROM SERVER"
#define CLIENT_MSG "HELLO FROM CLIENT"
struct sockaddr_un server_addr;
struct sockaddr_un client_addr;
memset(&server_addr, 0, sizeof(server_addr));
memset(&client_addr, 0, sizeof(client_addr));
int rc;
char buf[256];
void
processChild();
int
fork_child_process();
void
implementSocket();
#endif