#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Define struct to hold email data
struct Email
{
    string sender;
    string receiver;
    string content;
};

// Define function to read email data from file
Email
read_email_file(string filename)
{
    ifstream file(filename);
    string line;
    Email email;
    if (file.is_open())
    {
        getline(file, line);
        email.sender = line.substr(line.find(":") + 2);
        getline(file, line);
        email.receiver = line.substr(line.find(":") + 2);
        stringstream buffer;
        buffer << file.rdbuf();
        email.content = buffer.str();
        file.close();
    }
    return email;
}

// Define function to check if child process is alive
bool
is_child_alive(pid_t child_pid, int socket_fd)
{
    // Send "is_alive" message to child process
    string message = "is_alive";
    send(socket_fd, message.c_str(), message.length(), 0);

    // Wait for response or timeout after 1 second
    char response[1024];
    int bytes_received =
        recv(socket_fd, response, sizeof(response), MSG_DONTWAIT);
    if (bytes_received > 0)
    {
        // Check if response is "alive"
        response[bytes_received] = '\0';
        return strcmp(response, "alive") == 0;
    }
    else
    {
        // If no response, assume child process is dead
        return false;
    }
}

// Define function to fork a new child process
pid_t
fork_child_process()
{
    // Fork new process
    pid_t child_pid = fork();

    // Child process
    if (child_pid == 0)
    {
        // Call process_email_data function
        process_email_data();

        // Exit child process
        exit(EXIT_SUCCESS);
    }

    // Return child process ID
    return child_pid;
}

// Define function to send message to child process
string
send_message_to_child(int socket_fd, string message)
{
    // Send message to child process
    send(socket_fd, message.c_str(), message.length(), 0);

    // Wait for response or timeout after 1 second
    char response[1024];
    int bytes_received =
        recv(socket_fd, response, sizeof(response), MSG_DONTWAIT);
    if (bytes_received > 0)
    {
        response[bytes_received] = '\0';
        return string(response);
    }
    else
    {
        // If no response, assume child process is dead
        return "";
    }
}

// Define function to process email data
void
process_email_data()
{
    // Create socket connection
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(5000);
    bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(socket_fd, 1);

    // Wait for connection from parent process
    struct sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    int client_fd = accept(socket_fd, (struct sockaddr*)&client_address,
                           &client_address_size);

    // Continuously read and process email data
    while (true)
    {
        // Check if shared memory has data
        if (shared_memory->size() > 0)
        {
            // Get email data from shared memory
            Email email = shared_memory->at(0);

            // Log email data to CSV file
            ofstream file("email_archive.csv", ofstream::app);
            if (file.is_open())
            {
                file << email.sender << ";" << email.receiver << ";"
                     << email.content << endl;
                file.close();
            }

            // Remove email data from shared memory
            shared_memory->erase(shared_memory->begin());
        }

        // Check for incoming messages
        char buffer[1024];
        int bytes_received =
            recv(client_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
        if (bytes_received > 0)
        {
            buffer[bytes_received] = '\0';
            string message(buffer);
            if (message == "shutdown")
            {
                // If "shutdown" message received, terminate child process
                break;
            }
            else if (message == "is_alive")
            {
                // If "is_alive" message received, send "alive" message back
                // immediately
                string response = "alive";
                send(client_fd, response.c_str(), response.length(), 0);
            }
        }

        // Close socket connection
        close(client_fd);
        close(socket_fd);
    }

    int main()
    {
        // Create shared memory object for email data
        vector<Email>* shared_memory = (vector<Email>*)mmap(
            NULL, sizeof(vector<Email>), PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (shared_memory == MAP_FAILED)
        {
            cerr << "Error: Failed to create shared memory" << endl;
            exit(EXIT_FAILURE);
        }

        // Fork child process to handle email data
        pid_t child_pid = fork_child_process();

        // Read email data from files and send to child process
        for (int i = 1; i <= 5; i++)
        {
            // Read email data from file
            string filename = "email" + to_string(i) + ".txt";
            Email email = read_email_file(filename);

            // Check if child process is alive
            if (is_child_alive(child_pid, socket_fd))
            {
                // Store email data in shared memory
                shared_memory->push_back(email);
            }
            else
            {
                // Fork new child process and store email data in shared memory
                child_pid = fork_child_process();
                shared_memory->push_back(email);
            }

            // Wait for child process to log email data to CSV file
            usleep(100000);
        }

        // Send "shutdown" message to child process
        send_message_to_child(socket_fd, "shutdown");

        // Wait for child process to terminate
        waitpid(child_pid, NULL, 0);

        // Unmap shared memory
        munmap(shared_memory, sizeof(vector<Email>));
    }
    // int main()
    // {
    //     // Create shared memory object for email data
    //     vector<Email>* shared_memory = (vector<Email>*)mmap(
    //         NULL, sizeof(vector<Email>), PROT_READ | PROT_WRITE,
    //         MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    //     if (shared_memory == MAP_FAILED)
    //     {
    //         cerr << "Error: Failed to create shared memory" << endl;
    //         exit(EXIT_FAILURE);
    //     }

    //     // Fork child process to handle email data
    //     pid_t child_pid = fork_child_process();

    //     // Read email data from files and send to child process
    //     for (int i = 1; i <= 5; i++)
    //     {
    //         // Read email data from file
    //         string filename = "email" + to_string(i) + ".txt";
    //         Email email = read_email_file(filename);

    //         // Check if child process is alive
    //         if (is_child_alive(child_pid, socket_fd))
    //         {
    //             // Store email data in shared memory
    //             shared_memory->push_back(email);
    //         }
    //         else
    //         {
    //             // Fork new child process and store email data in shared
    //             memory child_pid = fork_child_process();
    //             shared_memory->push_back(email);
    //         }

    //         // Wait for child process to log email data to CSV file
    //         usleep(100000);
    //     }

    //     // Send "shutdown" message to child process
    //     send_message_to_child(socket_fd, "shutdown");

    //     // Wait for child process to terminate
    //     waitpid(child_pid, NULL, 0);

    //     // Unmap shared memory
    //     munmap(shared_memory, sizeof(vector<Email>));
    // }