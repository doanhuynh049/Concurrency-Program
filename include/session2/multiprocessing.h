#ifndef MULTIPROCESSING
#define MULTIPROCESSING
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
struct Email
{
    std::string sender;
    std::string receiver;
    std::string content;
};
Email
read_email_file(std::string filename);

bool
is_child_alive(pid_t child_pid, int socket_fd);

std::string
send_message_to_child(int socket_fd, std::string message);

void
process_email_data();

pid_t
fork_child_process();

void
executeMultiprocessing();

#endif