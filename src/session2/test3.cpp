#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/wait.h>  // for wait()
#include <unistd.h>

#include <sys/mman.h>  //used to map the shared memory object
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
struct Email
{
    std::string sender;
    std::string receiver;
    std::string content;
};
void
parentProcess()
{
    const char* SHM_NAME = "/my_email_shm";
    const size_t SHM_SIZE = sizeof(Email);

    // Create shared memory for an Email struct
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, SHM_SIZE);
    Email* email_ptr = static_cast<Email*>(
        mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    if (email_ptr == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Initialize the Email struct
    email_ptr->sender = "me@example.com";
    email_ptr->receiver = "you@example.com";
    email_ptr->content = "Hello, world!";

    // Create a pipe for inter-process communication
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child process
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Child process: read from shared memory
        close(pipe_fd[1]);  // Close the write end of the pipe

        // Read the file descriptor from the pipe
        int child_shm_fd;
        read(pipe_fd[0], &child_shm_fd, sizeof(child_shm_fd));

        // Map the shared memory object to a pointer
        Email* child_email_ptr =
            static_cast<Email*>(mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE,
                                     MAP_SHARED, child_shm_fd, 0));
        if (child_email_ptr == MAP_FAILED)
        {
            perror("mmap");
            exit(EXIT_FAILURE);
        }

        // Print the contents of the Email struct
        std::cout << "Child process: sender = " << child_email_ptr->sender
                  << std::endl;
        std::cout << "Child process: receiver = " << child_email_ptr->receiver
                  << std::endl;
        std::cout << "Child process: content = " << child_email_ptr->content
                  << std::endl;

        // Unmap the shared memory object
        munmap(child_email_ptr, SHM_SIZE);
        exit(EXIT_SUCCESS);
    }
    else
    {
        // Parent process: write to pipe and wait for child process
        close(pipe_fd[0]);  // Close the read end of the pipe

        // Write the file descriptor to the pipe
        write(pipe_fd[1], &shm_fd, sizeof(shm_fd));

        // Wait for the child process to exit
        wait(NULL);

        // Unmap and unlink the shared memory object
        munmap(email_ptr, SHM_SIZE);
        shm_unlink(SHM_NAME);
        exit(EXIT_SUCCESS);
    }
}
int
main()
{
    parentProcess();
    return 0;
}