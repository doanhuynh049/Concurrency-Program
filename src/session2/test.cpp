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

#define PORT 8080
#define BUFFER_SIZE 4096

struct Email
{
    std::vector<std::string> sender;
    std::vector<std::string> receiver;
    std::vector<std::string> content;
};
Email
read_email_file(std::string filename)
{
    std::ifstream file(filename);
    std::string line;
    Email email;
    if (file.is_open())
    {
        getline(file, line);
        email.sender.push_back(line.substr(line.find(":") + 2));
        // std::cout << email.sender[0] << std::endl;
        getline(file, line);
        email.receiver.push_back(line.substr(line.find(":") + 2));
        // // std::cout << email.receiver[0] << std::endl;
        std::stringstream buffer;
        buffer << file.rdbuf();
        email.content.push_back(buffer.str());
        file.close();
    }
    return email;
}
Email
read_email_csv(std::string filename)
{
    Email emails;
    std::ifstream file(filename);
    std::string line;
    // int i = std::cout << std::getline(file, line) << std::endl;
    getline(file, line);
    while (getline(file, line))
    {
        std::stringstream ss(line);
        std::string sender, receiver, content;
        getline(ss, sender, ';');
        getline(ss, receiver, ';');
        getline(ss, content, ';');
        emails.sender.push_back(sender);
        emails.receiver.push_back(receiver);
        emails.content.push_back(content);
    }

    return emails;
}
Email
print_email_csv(Email emails)
{
    // Print the emails
    for (int i = 0; i < emails.sender.size() - 1; i++)
    {
        std::cout << "From: " << emails.sender[i] << std::endl;
        std::cout << "To: " << emails.receiver[i] << std::endl;
        std::cout << "Content: " << emails.content[i] << std::endl;
        std::cout << std::endl;
    }
    return emails;
}
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

void
process_email_archive(Email* shared_data)
{
    std::cout << "\nRead shared memory: \n";
    std::cout << "From: " << shared_data->sender[0] << std::endl;
    std::cout << "To: " << shared_data->receiver[0] << std::endl;
    std::cout << "Content: " << shared_data->content[0] << std::endl;
    std::cout << std::endl;

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
        file_open << shared_data->sender[0] << ";" << shared_data->receiver[0]
                  << ";" << shared_data->content[0] << std::endl;
        file_open.close();
    }
}
int
child_process_fork(Email* shared_data)
{
    pid_t pid_child = fork();
    if (pid_child == 0)
    {
        std::cout << "Child process fork\n";
        int status, valread, client_fd;
        struct sockaddr_in server_addr;
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
        std::cout << "Hello message sent\n";
        // Receive message from server
        valread = read(client_fd, buffer, BUFFER_SIZE);
        // std::endl;
        if (valread > 0)
        {
            buffer[valread] = '\0';
            std::string message(buffer);
            if (message == "is_alive")
            {
                std::cout << "Child process received message: " << buffer
                          << std::endl;

                std::string response = "alive";
                send(client_fd, response.c_str(), response.length(), 0);
            }
        }
        sleep(1);
        if (shared_data->sender.size() > 0)
        {
            process_email_archive(shared_data);
            // shared_data->erase(shared_data->begin());
        }

        close(client_fd);
    }
    return pid_child;
}

void
parent_process()
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = { 0 };
    const char* message = "Hello from server";

    // initialize shared memory
    // int* shared_data;  // pointer to shared memory
    const char* SHM_NAME = "/my_shm";
    const int SHM_SIZE = sizeof(int);
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm open");
        exit(EXIT_FAILURE);
    }

    ftruncate(shm_fd, SHM_SIZE);

    Email* shared_data;

    // std::vector<Email>* shared_data = (std::vector<Email>*)(mmap(
    //     NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
    void* shared_mem =
        mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    shared_data = static_cast<Email*>(shared_mem);

    // for (int i = 1; i <= 5; i++)
    // {
    std::string filename = "../../email/email_file1.txt";
    Email email = read_email_file(filename);
    // }

    pid_t pid_child = child_process_fork(shared_data);
    if (pid_child > 0)
    {
        std::cout << "Parent process fork\n";

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
        std::cout << "Parent process received message: " << buffer << std::endl;
        // Send message to client
        // if (send(new_socket, message, strlen(message), 0) == -1)
        // {
        //     perror("Send failed");
        //     exit(EXIT_FAILURE);
        // }
        // std::cout << "Sent message: " << message << std::endl;

        // std::string email_archive = "../../email/email.csv";
        // Email email = read_email_csv(email_archive);
        // print_email_csv(email);
        if (check_child_alive(pid_child, new_socket))
        {
            std::cout << "Parent process got repsonse\n";
            // shared_data->push_back(email);
            std::memcpy(shared_mem, &email, sizeof(Email));
        }
        // std::cout << "From: " << shared_data->sender.size() << std::endl;
        // std::cout << "Size of shared memory: " << shared_data->size()
        //           << std::endl;
        // Close sockets
        waitpid(pid_child, NULL, 0);

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