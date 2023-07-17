#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
const char* PIPE_NAME = "my_pipe";

void child_process()
{
    int fd = open(PIPE_NAME, O_WRONLY);

    const char* message = "Hello, world!";
    write(fd, message, strlen(message));
    close(fd);
}

void parent_process()
{
    int fd = open(PIPE_NAME, O_RDONLY);

    char buffer[256];
    read(fd, buffer, 256);

    std::cout << "Received message: " << buffer << std::endl;

    close(fd);
}

int main()
{
    mkfifo(PIPE_NAME, 0666);

    pid_t pid = fork();

    if (pid == 0)
    {
        child_process();
    }
    else if (pid > 0)
    {
        parent_process();
    }
    else
    {
        std::cerr << "Error forking process" << std::endl;
        return 1;
    }

    unlink(PIPE_NAME);

    return 0;
}
