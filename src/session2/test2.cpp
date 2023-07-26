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

#define PORT 8080
#define BUFFER_SIZE 4096
int server_fd, new_socket, valread;
struct sockaddr_in address;
struct sockaddr_in server_addr;
int opt = 1;
int addrlen = sizeof(address);
#define EMAILS_CAPACITY 5
// #define SHM_SIZE 1024 * 1024
// Email* email_ptr;

const size_t shm_size = 500;

// Create shared memory
int shm_id = shmget(IPC_PRIVATE, shm_size, 0666);

// Attach to shared memory
char* shared_mem = (char*)shmat(shm_id, NULL, 0);

const char* SHM_NAME = "/my_shm";
int* shared_data;
const int SHM_SIZE = sizeof(int);

std::string message;

struct Email
{
    std::string sender;
    std::string receiver;
    std::string content;
};
class EmailClass
{
public:
    void
    display_emails(std::vector<Email> emails)
    {
        for (auto email : emails)
        {
            std::cout << "From: " << email.sender << std::endl;
            std::cout << "To: " << email.receiver << std::endl;
            std::cout << "Content: " << std::endl;
            std::cout << email.content << std::endl;
            std::cout << std::endl;
        }
    }

    Email
    exstractMessage(int i)
    {
        std::string filename =
            "../../email/email_file" + std::to_string(i) + ".txt";

        std::ifstream file(filename);
        std::string line;
        Email email;
        if (file.is_open())
        {
            getline(file, line);
            email.sender = line.substr(line.find(":") + 2);
            getline(file, line);
            email.receiver = line.substr(line.find(":") + 2);
            std::stringstream buffer;
            buffer << file.rdbuf();
            email.content = buffer.str();
            file.close();
        }
        return email;
    }

    void
    read_email_file(std::vector<Email>& emails)
    {
        for (int i = 1; i <= 5; i++)
        {
            std::string filename =
                "../../email/email_file" + std::to_string(i) + ".txt";

            std::ifstream file(filename);
            std::string line;
            Email email;
            if (file.is_open())
            {
                getline(file, line);
                email.sender = line.substr(line.find(":") + 2);
                getline(file, line);
                email.receiver = line.substr(line.find(":") + 2);
                std::stringstream buffer;
                buffer << file.rdbuf();
                email.content = buffer.str();
                file.close();
            }
            emails.push_back(email);
            file.close();
        }
        // display_emails(emails);
    }
    void
    readContent(int i)
    {
        message = "";
        std::string filename =
            "../../email/email_file" + std::to_string(i) + ".txt";
        std::ifstream file(filename);
        if (file.is_open())
        {
            std::string line;
            while (getline(file, line))
            {
                // std::cout << line << std::endl;
                message += line + "\n";
            }
            file.close();
        }
        else
        {
            std::cerr << "Failed to open file" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void
    process_email_archive(int i)
    {
        // std::cout << message;
        Email email = exstractMessage(i);
        const std::string filename = "../../email/email_archive.csv";
        std::ifstream file(filename);
        if (!file.good())
        {
            std::ofstream newfile(filename);
            newfile << "From;To;Content\n";
            newfile.close();
        }
        else
        {
            std::string line;
            std::getline(file, line);
            if (line != "From;To;Content")
            {
                std::stringstream ss;
                ss << "From;To;Content" << line << "\n";
                file.close();
                std::ofstream newfile1(filename);
                newfile1 << ss.str();
                newfile1.close();
            }
            else
            {
                file.close();
            }
        }
        std::ofstream file_open(filename, std::ofstream::app);
        if (file_open.is_open())
        {
            file_open << email.sender << ";" << email.receiver << ";"
                      << email.content << std::endl;
            file_open.close();
        }
    }
};

// Email
// read_email_csv(std::string filename)
// {
//     Email emails;
//     std::ifstream file(filename);
//     std::string line;
//     // int i = std::cout << std::getline(file, line) << std::endl;
//     getline(file, line);
//     while (getline(file, line))
//     {
//         std::stringstream ss(line);
//         std::string sender, receiver, content;
//         getline(ss, sender, ';');
//         getline(ss, receiver, ';');
//         getline(ss, content, ';');
//         emails.sender.push_back(sender);
//         emails.receiver.push_back(receiver);
//         emails.content.push_back(content);
//     }

//     return emails;
// }

bool
check_child_alive(pid_t child_pid, int socket_fd)
{
    std::string message = "is_alive";
    send(socket_fd, message.c_str(), message.length(), 0);
    char response[BUFFER_SIZE];
    int bytes_received = read(socket_fd, response, BUFFER_SIZE);
    // int bytes_received = recv(socket_fd, response, BUFFER_SIZE,
    // MSG_DONTWAIT);
    std::cout << "Parrent process received response: " << response << std::endl;
    if (bytes_received > 0)
    {
        response[bytes_received] = '\0';
        return strcmp(response, "alive") == 0;
    }
    else
        return false;
}

int
child_process_fork()
{
    pid_t pid_child = fork();
    if (pid_child == 0)
    {
        std::cout << "Child process fork\n";
        int status, valread, client_fd;

        const char* message = "Hello from client";
        char buffer[BUFFER_SIZE] = { 0 };
        if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            std::cerr << "Socket creation error\n";
            exit(EXIT_FAILURE);
        }
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
        {
            std::cerr << "Invalid address/ Address not supported \n";
            exit(EXIT_FAILURE);
        }
        if ((status = connect(client_fd, (struct sockaddr*)&server_addr,
                              sizeof(server_addr)))
            < 0)
        {
            std::cerr << "Connection failed\n";
            exit(EXIT_FAILURE);
        }
        send(client_fd, message, strlen(message), 0);
        std::cout << "Child process: Hello message sent\n";
        // 1. wait for (“is_alive”) message response (“alive”) immediately
        valread = read(client_fd, buffer, BUFFER_SIZE);
        if (valread > 0)
        {
            buffer[valread] = '\0';
            std::string message(buffer);
            if (message == "is_alive")
            {
                std::cout << "Child process: received message: " << buffer
                          << std::endl;
                std::string response = "alive";
                send(client_fd, response.c_str(), response.length(), 0);
                // std::cout << *shared_data << std::endl;
            }
        }
        EmailClass emailclass;
        while (1)
        {
            // 2. detect shared memory is having data and read it
            if (strlen(shared_mem) > 0)
            {
                std::cout
                    << "Child process: detect shared memory having data\n";
                std::cout << shared_mem << std::endl;
                emailclass.process_email_archive(*shared_data);
                strcpy(shared_mem, "");
                if (*shared_data >= 5)
                    break;
            }
        }
        close(client_fd);
    }
    return pid_child;
}

void
setupShareMemory()
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
    *shared_data = 1;
}
void
setupParentSocket()
{
    char buffer[BUFFER_SIZE] = { 0 };
    const char* message = "Hello from server";
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket failed\n";
        exit(EXIT_FAILURE);
    }
    // Set socket option
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        std::cerr << "setsockopt\n";
        exit(EXIT_FAILURE);
    }
    // Bind socket to address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        std::cerr << "bind failure\n";
        exit(EXIT_FAILURE);
    }
    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        std::cerr << "listen\n";
        exit(EXIT_FAILURE);
    }
    // Accept incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address,
                             (socklen_t*)&addrlen))
        < 0)
    {
        std::cerr << "accept\n";
        exit(EXIT_FAILURE);
    }
    // Receive message from client
    if ((valread = read(new_socket, buffer, BUFFER_SIZE)) == -1)
    {
        perror("Read failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Parent process: received message: " << buffer << std::endl;
    // Send message to client
    // if (send(new_socket, message, strlen(message), 0) == -1)
    // {
    //     perror("Send failed");
    //     exit(EXIT_FAILURE);
    // }
    // std::cout << "Sent message: " << message << std::endl;
}
void
parent_process()
{
    setupShareMemory();
    EmailClass emailclass;

    // Email email =
    emailclass.readContent(1);  // 1.read 1 file
    pid_t pid_child = child_process_fork();
    if (pid_child > 0)
    {
        std::cout << "Parent process fork\n";

        setupParentSocket();
        // 2.check alive of process 2

        if (check_child_alive(pid_child, new_socket))
        {
            while (*shared_data <= 5)
            {
                // 3. save data to shared memory

                // std::cout << "Parent process " << *shared_data << std::endl;
                emailclass.readContent(*shared_data);
                strcpy(shared_mem, message.c_str());
                sleep(1);
                // std::cout << "Parent process: got repsonse\n";
                (*shared_data)++;
            }
        }
        waitpid(pid_child, NULL, 0);
        // munmap(shared_data, SHM_SIZE);
        // munmap(email_ptr, SHM_SIZE);
        shm_unlink("/my_shm");
        close(new_socket);
        close(server_fd);
    }
}

int
main(int argc, char const* argv[])
{
    parent_process();
    return 0;
}
