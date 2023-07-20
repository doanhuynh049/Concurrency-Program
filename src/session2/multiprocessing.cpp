#include "../../include/session2/multiprocessing.h"
#include <sys/socket.h>
#include <fstream>
#include <sstream>

Email
read_email_file(std::string filename)
{
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

bool
is_child_alive(pid_t child_pid, int socket_fd)
{
    std::string message = "is_alive";
    send(socket_fd, message.c_str(), message.length(), 0);
    return true;
}

std::string
send_message_to_child(int socket_fd, std::string message)
{
}

void
process_email_data()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(5000);
    bind(socket_fd, (struc sockaddr*)&server_address, sizeof(server_address));
    listen(socket_fd, 1);

    // parent process
    struct sockaddr_in client_address;
}

pid_t
fork_child_process()
{
    pid_t child_pid = fork();
    if (child_pid == 0)
    {
        std::cout << "Child process start\n";
        process_email_data();
        std::cout << "Child process end\n";

        exit(EXIT_FAILURE);
    }

    return child_pid;
}

void
executeMultiprocessing()
{
    pid_t child_pid = fork_child_process();
    if (child_pid == -1)
    {
        std::cerr << "Fork error";
        exit(EXIT_FAILURE);
    }
    else
    {
        std::string filename = "../../email/email_file1.txt";
        Email email = read_email_file(filename);
    }
}
