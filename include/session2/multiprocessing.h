#ifndef MULTIPROCESSING_H
#define MULTIPROCESSING_H
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/mman.h>  //used to map the shared memory object
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>  // for wait()
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#define PORT 8080
#define BUFFER_SIZE 4096

struct Email
{
    std::string sender;
    std::string receiver;
    std::string content;
};

class EmailClass
{
public:
    // void
    // display_emails(std::vector<Email> emails);
    std::string message;
    Email
    exstractMessage();

    // void
    // read_email_file(std::vector<Email>& emails);
    void
    readContent(int i);
    void
    process_email_archive();
    // Email
    // read_email_csv(std::string filename);
};

class MultiProcessing
{
private:
    int server_fd, client_fd, new_socket, valread;
    struct sockaddr_in address;
    struct sockaddr_in server_addr;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = { 0 };

public:
    EmailClass emailclass;

    bool
    check_child_alive(int socket_fd);
    void
    setupShareMemory();
    void
    setupParentSocket();
    void
    setupChildSocket();
    int
    child_process_fork();
    void
    child_process();
    void
    parent_process();
    void
    executeMultiProcessing();
};

#endif