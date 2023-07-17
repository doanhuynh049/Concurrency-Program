#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

constexpr int MAX_CLIENTS = 10;

std::mutex mutex;
std::condition_variable cv;
bool ready{false};
int client_sockets[MAX_CLIENTS];
int client_count{0};

// function to handle a client connection
void handle_client(int client_socket) {
    char buffer[1024];
    int read_size;
    while ((read_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        std::cout << "received: " << buffer << std::endl;
        send(client_socket, buffer, read_size, 0);
        std::memset(buffer, 0, sizeof(buffer));
    }
    if (read_size == 0) {
        std::cout << "client disconnected" << std::endl;
    } else if (read_size == -1) {
        std::cerr << "recv failed" << std::endl;
    }
    close(client_socket);
}

int main(int argc, char *argv[]) {
    int server_socket, client_socket;
    sockaddr_in server_address, client_address;
    socklen_t address_size = sizeof(sockaddr_in);

    // create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "socket creation failed" << std::endl;
        return 1;
    }

    // set server address and port
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // bind the socket to the address and port
    if (bind(server_socket, (sockaddr *)&server_address, sizeof(server_address)) == -1) {
        std::cerr << "bind failed" << std::endl;
        return 1;
    }

    // listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        std::cerr << "listen failed" << std::endl;
        return 1;
    }

    // accept incoming connections in another thread
    std::thread accept_thread([&] {
        while (true) {
            client_socket = accept(server_socket, (sockaddr *)&client_address, &address_size);
            if (client_socket == -1) {
                std::cerr << "accept failed" << std::endl;
            } else {
                std::unique_lock<std::mutex> lock(mutex);
                if (client_count == MAX_CLIENTS) {
                    std::cerr << "max clients reached" << std::endl;
                    close(client_socket);
                } else {
                    client_sockets[client_count++] = client_socket;
                }
            }
        }
    });

    // wait for the accept thread to start accepting connections
    {
        std::unique_lock<std::mutex> lock(mutex);
        ready = true;
        cv.notify_all();
    }

    // handle incoming connections in separate threads
    std::thread threads[MAX_CLIENTS];
    {
        std::unique_lock<std::mutex> lock(mutex);
        while (client_count == 0) {
            cv.wait(lock);
        }
        for (int i = 0; i < client_count; ++i) {
            threads[i] = std::thread(handle_client, client_sockets[i]);
        }
    }

    // join all threads when the program exits
    accept_thread.join();
    for (int i = 0; i < client_count; ++i) {
        threads[i].join();
    }

    return 0;
}
