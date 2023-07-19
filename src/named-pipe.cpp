#include "../include/named-pipe.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

void
NamedPipe::child_process()
{
    int fd = open(PIPE_NAME.c_str(), O_WRONLY);
    if (fd == -1)
    {
        std::cerr << "Error opening pipe for writing" << std::endl;
        exit(EXIT_FAILURE);
    }
    const char* message = "Hello world!";
    ssize_t nbytes = write(fd, message, strlen(message));
    if (nbytes == -1)
    {
        std::cerr << "Error writing to pipe" << std::endl;
        exit(EXIT_FAILURE);
    }
    close(fd);
}
void
NamedPipe::parent_process(int timeout_ms)
{
    int fd = open(PIPE_NAME.c_str(), O_RDONLY);
    if (fd == -1)
    {
        std::cerr << "Error opening pipe for reading" << std::endl;
        exit(EXIT_FAILURE);
    }
    char buffer[256];
    ssize_t nbytes = read(fd, buffer, 256);
    // std ::cout << "Size of pipe  " << nbytes << std::endl;
    if (nbytes == -1)
    {
        std::cerr << "Error reading from pipe" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (nbytes == 0)
    {
        std::cerr << "No data received from pipe" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Received message: " << buffer << std::endl;
    close(fd);
}
void
NamedPipe::implementPipe()
{
    int fifo = mkfifo(PIPE_NAME.c_str(), 0666);
    if (fifo == -1)
    {
        std::cerr << "Error creating fifl" << std::endl;
        exit(EXIT_FAILURE);
    }
    pid_t c_pid = fork();
    if (c_pid == -1)
    {
        std::cerr << "Error forking process " << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (c_pid == 0)
    {
        // child process
        child_process();
    }
    else
    {
        // parrent process
        parent_process();
    }
    unlink(PIPE_NAME.c_str());
}