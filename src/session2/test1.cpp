#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define PORT "8080"
#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024
#define SHM_SIZE 1024 * 1024

struct Email
{
    std::string sender;
    std::string receiver;
    std::string content;
};

void
handle_sigterm(int sig)
{
    exit(0);
}

int
main(int argc, char* argv[])
{
    std::vector<Email> emails;
    std::string email_archive_file = "email_archive.csv";
    std::string first_line = "From;To;Content\n";
    int shared_mem_fd, sock_fd, conn_fd;
    void* shared_mem_ptr;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    bool is_alive = false;

    // Handle SIGTERM signal
    signal(SIGTERM, handle_sigterm);

    // Read data from import files
    for (int i = 1; i <= 5; i++)
    {
        std::string filename = "import_file_" + std::to_string(i) + ".txt";
        std::ifstream file(filename);
        if (file.is_open())
        {
            std::string line;
            Email email;
            while (std::getline(file, line))
            {
                if (line.find("From:") != std::string::npos)
                {
                    email.sender = line.substr(6);
                }
                else if (line.find("To:") != std::string::npos)
                {
                    email.receiver = line.substr(4);
                }
                else
                {
                    email.content += line + "\n";
                }
            }
            emails.push_back(email);
            file.close();
        }
    }

    // Create shared memory
    shared_mem_fd = shm_open("/email_archive", O_CREAT | O_RDWR, 0666);
    if (shared_mem_fd == -1)
    {
        std::cerr << "Failed to create shared memory\n";
        return 1;
    }
    ftruncate(shared_mem_fd, SHM_SIZE);
    shared_mem_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                          shared_mem_fd, 0);
    if (shared_mem_ptr == MAP_FAILED)
    {
        std::cerr << "Failed to map shared memory\n";
        return 1;
    }

    // Create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1)
    {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    // Bind socket to port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(std::stoi(PORT));
    if (bind(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))
        == -1)
    {
        std::cerr << "Failed to bind socket to port\n";
        return 1;
    }

    // Listen for connections
    if (listen(sock_fd, MAX_CLIENTS) == -1)
    {
        std::cerr << "Failed to listen for connections\n";
        return 1;
    }

    // Fork process 2
    pid_t pid = fork();
    if (pid == -1)
    {
        std::cerr << "Failed to fork process\n";
        return 1;
    }
    else if (pid == 0)
    {
        // Process 2 (Child process)
        while (1)
        {
            // Wait for "is_alive" message from Process 1
            char buffer[MAX_BUFFER_SIZE];
            int bytes_received = recv(conn_fd, buffer, MAX_BUFFER_SIZE, 0);
            if (bytes_received > 0)
            {
                std::string message(buffer, bytes_received);
                if (message == "is_alive")
                {
                    // Send "alive" message to Process 1
                    send(conn_fd, "alive", 5, 0);

                    // Read shared memory and write to email archive file
                    std::string email_archive_data;
                    if (strlen((char*)shared_mem_ptr) > 0)
                    {
                        Email* email = (Email*)shared_mem_ptr;
                        while (email->sender != "")
                        {
                            email_archive_data += email->sender + ";"
                                                  + email->receiver + ";"
                                                  + email->content + "\n";
                            email++;
                        }
                        std::ofstream outfile(email_archive_file,
                                              std::ios_base::app);
                        if (outfile.is_open())
                        {
                            outfile << email_archive_data;
                            outfile.close();
                        }
                    }
                }
                else if (message == "shutdown")
                {
                    close(conn_fd);
                    exit(0);
                }
            }
        }
    }
    else
    {
        // Process 1 (Parent process)
        for (auto email : emails)
        {
            // Send "is_alive" message to Process 2
            sock_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (sock_fd == -1)
            {
                std::cerr << "Failed to create socket\n";
                return 1;
            }
            if (connect(sock_fd, (struct sockaddr*)&server_addr,
                        sizeof(server_addr))
                == -1)
            {
                // Process 2 is not alive, fork new Process 2
                pid = fork();
                if (pid == -1)
                {
                    std::cerr << "Failed to fork process\n";
                    return 1;
                }
                else if (pid == 0)
                {
                    // Process 2 (Child process)
                    while (1)
                    {
                        // Wait for "is_alive" message from Process 1
                        char buffer[MAX_BUFFER_SIZE];
                        int bytes_received =
                            recv(conn_fd, buffer, MAX_BUFFER_SIZE, 0);
                        if (bytes_received > 0)
                        {
                            std::string message(buffer, bytes_received);
                            if (message == "is_alive")
                            {
                                // Send "alive" message to Process 1
                                send(conn_fd, "alive", 5, 0);

                                // Read shared memory and write to email archive
                                // file
                                std::string email_archive_data;
                                if (strlen((char*)shared_mem_ptr) > 0)
                                {
                                    Email* email = (Email*)shared_mem_ptr;
                                    while (email->sender != "")
                                    {
                                        email_archive_data +=
                                            email->sender + ";"
                                            + email->receiver + ";"
                                            + email->content + "\n";
                                        email++;
                                    }
                                    std::ofstream outfile(email_archive_file,
                                                          std::ios_base::app);
                                    if (outfile.is_open())
                                    {
                                        outfile << email_archive_data;
                                        outfile.close();
                                    }
                                }
                            }
                            else if (message == "shutdown")
                            {
                                close(conn_fd);
                                exit(0);
                            }
                        }
                    }
                }
                else
                {
                    close(sock_fd);
                }
            }
            else
            {
                // Process 2 is alive, send data to shared memory
                is_alive = true;
                Email* email_ptr = (Email*)shared_mem_ptr;
                while (email_ptr->sender != "")
                {
                    email_ptr++;
                }
                email_ptr->sender = email.sender;
                email_ptr->receiver = email.receiver;
                email_ptr->content = email.content;
                close(sock_fd);
            }
        }

        // Send "shutdown" message to Process 2
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd == -1)
        {
            std::cerr << "Failed to create socket\n";
            return 1;
        }
        if (connect(sock_fd, (struct sockaddr*)&server_addr,
                    sizeof(server_addr))
            == -1)
        {
            std::cerr << "Failed to connect to socket\n";
            return 1;
        }
        else
        {
            send(sock_fd, "shutdown", 9, 0);
            char buffer[MAX_BUFFER_SIZE];
            int bytes_received = recv(sock_fd, buffer, MAX_BUFFER_SIZE, 0);
            if (bytes_received > 0)
            {
                std::string message(buffer, bytes_received);
                if (message == "alive")
                {
                    // Process 2 is still alive, wait for 1 second and try again
                    sleep(1);
                    send(sock_fd, "shutdown", 9, 0);
                    bytes_received = recv(sock_fd, buffer, MAX_BUFFER_SIZE, 0);
                    if (bytes_received > 0)
                    {
                        std::string message(buffer, bytes_received);
                        if (message == "alive")
                        {
                            close(sock_fd);
                            std::cerr << "Failed to shutdown process 2\n";
                            return 1;
                        }
                    }
                }
            }
            close(sock_fd);
        }

        // Write to email archive file if shared memory still has data
        std::string email_archive_data;
        if (strlen((char*)shared_mem_ptr) > 0)
        {
            Email* email = (Email*)shared_mem_ptr;
            while (email->sender != "")
            {
                email_archive_data += email->sender + ";" + email->receiver
                                      + ";" + email->content + "\n";
                email++;
            }
            std::ofstream outfile(email_archive_file, std::ios_base::app);
            if (outfile.is_open())
            {
                outfile << email_archive_data;
                outfile.close();
            }
        }

        // Terminate program
        close(shared_mem_fd);
        munmap(shared_mem_ptr, SHM_SIZE);
        shm_unlink("/email_archive");
        return 0;
    }
}