#include "../../include/session1/fork.h"

#include <fcntl.h>
#include <sys/mman.h>  //used to map the shared memory object
#include <sys/wait.h>  // for wait()
#include <unistd.h>
#include <cstring>
#include <iostream>
void
implementFork()
{
    pid_t c_pid = fork();
    if (c_pid == -1)
    {
        std::cerr << "Faild to fork process. " << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (c_pid == 0)
    {
        // child process; pid is zero
        std::cout << "Printed from child process \tPID: " << getpid()
                  << "\t fork() return: " << c_pid << std::endl;
    }
    else
    {
        // parent process; fork return child's process id
        std::cout << "Printed from parent process \tPID: " << getpid()
                  << "\t fork() return: " << c_pid << std::endl;
    }
}

void
implementMultiFork()
{
    fork();
    fork();
    std::cout << "Doan Huynh Quat\n";
}

void
executeProcess()
{
    pid_t id1 = fork();
    pid_t id2 = fork();
    if (id1 > 0 && id2 > 0)
    {
        wait(NULL);
        wait(NULL);
        std::cout << "Parrent Terminated\n";
    }
    else if (id1 == 0 && id2 > 0)
    {
        sleep(2);
        wait(NULL);
        std::cout << "1st chuild Terminated\n";
    }
    else if (id1 > 0 && id2 == 0)
    {
        sleep(1);
        std::cout << "2nd child Terminated\n";
    }
    else
    {
        std::cout << "Grand child termnated\n";
    }
}

void
childProcess(int id, int* data)
{
}
void
implementFork1()
{
    int data_size = 4;
    int* data = new int[data_size];
    data[0] = 5;
    // child processes
    for (int i = 1; i < data_size; i++)
    {
        pid_t c_pid = fork();
        if (c_pid == -1)
        {
            std::cerr << "Failed to fork process." << std::endl;
            exit(EXIT_FAILURE);
        }
        else if (c_pid == 0)
        {
            data[i] = i;
            std::cout << "Child process " << i << ": " << data[0] << " "
                      << data[1] << " " << data[2] << " " << data[3] << " "
                      << std::endl;
            ;
            exit(EXIT_SUCCESS);
        }
    }
    // Wait for child processes to terminate
    for (int i = 0; i < 3; i++)
    {
        wait(NULL);
    }
    std::cout << "Parent process data: ";
    for (int i = 0; i < data_size; i++)
    {
        std::cout << data[i] << " ";
    }
    delete[] data;
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
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    {
        // Child process
        (*shared_data)++;
        std::cout << "Child process: shared data = " << *shared_data
                  << std::endl;
        exit(0);
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

static int globalvar = 0;

void
implementFork12()
{
    int data_size = 3;
    // child processes
    for (int i = 1; i < data_size; i++)
    {
        pid_t c_pid = fork();
        if (c_pid == -1)
        {
            std::cerr << "Failed to fork process." << std::endl;
            exit(EXIT_FAILURE);
        }
        else if (c_pid == 0)
        {
            globalvar += 5;
            std::cout << "Process child data " << i
                      << ": Address: " << &globalvar << " Value: " << globalvar
                      << std::endl;
            ;
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Wait for child processes to terminate
            wait(NULL);
            std::cout << "Parent process data: ";

            globalvar += 15;
            std::cout << "Address: " << &globalvar << " Value: " << globalvar
                      << std::endl;
            ;
        }
    }
}