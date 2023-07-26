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
int* shared_data;  // pointer to shared memory
const char* SHM_NAME = "/my_shm";
const int SHM_SIZE = sizeof(int);  // size of shared memory in bytes
void
childprocess()
{  // Child process
    (*shared_data)++;
    std::cout << "Child process: shared data = " << *shared_data << std::endl;
    exit(0);
}
void
parentprocess()
{
    // Parent process
    wait(NULL);
    (*shared_data) += 5;
    std::cout << "Parent process: shared data = " << *shared_data << std::endl;
    munmap(shared_data, SHM_SIZE);
    shm_unlink("/my_shm");
}
void
implementShareMemory()
{
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
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        childprocess();
    }
    else
    {
        parentprocess();
    }
}
int
main()
{
    *shared_data = 5;
    implementShareMemory();
}