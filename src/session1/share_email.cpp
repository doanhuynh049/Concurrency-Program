#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <sys/wait.h>  // for wait()
#include <unistd.h>

#include <sys/mman.h>  //used to map the shared memory object
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
int
main()
{
    // int i = 0;
    std::string filename = "../../email/email_file1.txt";
    std::ifstream input_file(filename);
    std::string message;

    if (input_file.is_open())
    {
        std::string line;
        while (getline(input_file, line))
        {
            std::cout << line << std::endl;
            message += line + "\n";
        }
        input_file.close();
    }
    else
    {
        std::cerr << "Failed to open file" << std::endl;
        return 1;
    }

    // const char* message = "Hello from child!";

    const size_t shm_size = message.size() + 1;  // add 1 for null terminator

    // Create shared memory
    int shm_id = shmget(IPC_PRIVATE, shm_size, 0666);

    // Attach to shared memory
    char* shared_mem = (char*)shmat(shm_id, NULL, 0);

    pid_t pid = fork();

    if (pid == 0)
    {  // Child process
        // Write string to shared memory
        strcpy(shared_mem, message.c_str());
        printf("Child wrote message to shared memory\n");
    }
    else
    {  // Parent process
        // Wait for child to write string
        wait(NULL);

        if (strlen(shared_mem) == 0)
        {
            printf("No data in shared memory\n");
        }
        else
        {
            // Read string from shared memory
            printf("Parent read from shared memory: %s\n", shared_mem);
        }

        // Detach shared memory
        shmdt(shared_mem);

        // Remove shared memory
        shmctl(shm_id, IPC_RMID, NULL);
    }
}