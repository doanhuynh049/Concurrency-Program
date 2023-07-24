#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>  // for wait()
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

void
childprocess(int* shared_data)
{
    // Child process
    (*shared_data)++;
    std::cout << "Child process: shared data = " << *shared_data << std::endl;
    exit(0);
}
void
implementShareMemory()
{
    int* shared_data;  // pointer to shared memory
    const char* SHM_NAME = "/my_shm";
    const int SHM_SIZE = sizeof(int);  // size of shared memory in bytes
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, SHM_SIZE);
    shared_data = static_cast<int*>(
        mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    close(shm_fd);
    if (shared_data == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    *shared_data = 10;
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        childprocess(shared_data);
    }
    else
    {
        // Parent process
        wait(NULL);
        (*shared_data) += 5;
        std::cout << "Parent process: shared data = " << *shared_data
                  << std::endl;
        munmap(shared_data, SHM_SIZE);
        shm_unlink("/my_shm");
    }
}

int
main()
{
    implementShareMemory();
}