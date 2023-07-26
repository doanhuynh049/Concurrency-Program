#include "../../include/session2/multiprocessing.h"

const char* SHM_NAME = "/my_shm";

const int SHM_SIZE = sizeof(int);

char* shared_mem;
int* shared_data;
// std::string message;
// void
// EmailClass::display_emails(std::vector<Email> emails)
// {
//     for (auto email : emails)
//     {
//         std::cout << "From: " << email.sender << std::endl;
//         std::cout << "To: " << email.receiver << std::endl;
//         std::cout << "Content: " << std::endl;
//         std::cout << email.content << std::endl;
//         std::cout << std::endl;
//     }
// }

Email
EmailClass::exstractMessage()
{
    std::string filename =
        "../../email/email_file" + std::to_string(*shared_data) + ".txt";

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

// void
// EmailClass::read_email_file(std::vector<Email>& emails)
// {
//     for (int i = 1; i <= 5; i++)
//     {
//         std::string filename =
//             "../../email/email_file" + std::to_string(i) + ".txt";

//         std::ifstream file(filename);
//         std::string line;
//         Email email;
//         if (file.is_open())
//         {
//             getline(file, line);
//             email.sender = line.substr(line.find(":") + 2);
//             getline(file, line);
//             email.receiver = line.substr(line.find(":") + 2);
//             std::stringstream buffer;
//             buffer << file.rdbuf();
//             email.content = buffer.str();
//             file.close();
//         }
//         emails.push_back(email);
//         file.close();
//     }
//     // display_emails(emails);
// }
void
EmailClass::readContent(int i)
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
EmailClass::process_email_archive()
{
    // std::cout << message;
    Email email = exstractMessage();
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
// Email
// EmailClass::read_email_csv(std::string filename)
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
MultiProcessing::check_child_alive(int socket_fd)
{
    std::string message = "is_alive";
    send(socket_fd, message.c_str(), message.length(), 0);
    char response[BUFFER_SIZE];
    int bytes_received = read(socket_fd, response, BUFFER_SIZE);
    std::cout << "Parrent process received response: " << response << std::endl;
    if (bytes_received > 0)
    {
        response[bytes_received] = '\0';
        return std::string(response) == "alive";
    }
    else
    {
        return false;
    }
}

void
MultiProcessing::setupChildSocket()
{
    int status;

    const char* message = "Hello from client";

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
}
void
MultiProcessing::setupParentSocket()
{
    // const char* message = "Hello from server";
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
}

void
MultiProcessing::setupShareMemory()
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

    const size_t shm_size = 500;
    // Create shared memory
    int shm_id = shmget(IPC_PRIVATE, shm_size, 0666);

    // Attach to shared memory
    shared_mem = (char*)shmat(shm_id, NULL, 0);
}

void
MultiProcessing::child_process()
{
    std::cout << "Child process fork\n";
    // int valread;
    // char buffer[BUFFER_SIZE] = { 0 };
    setupChildSocket();
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
    // EmailClass emailclass;
    while (1)
    {
        // 2. detect shared memory is having data and read it
        if (strlen(shared_mem) > 0)
        {
            std::cout << "Child process: detect shared memory having data\n";
            std::cout << shared_mem << std::endl;
            emailclass.process_email_archive();
            strcpy(shared_mem, "");
            if (*shared_data >= 5)
                break;
        }
    }
    valread = read(client_fd, buffer, BUFFER_SIZE);
    if (valread > 0)
    {
        buffer[valread] = '\0';
        std::string message(buffer);
        if (message == "shutdown")
        {
            std::cout << "Child process: received message: " << buffer
                      << std::endl;
            close(client_fd);
        }
    }
}
int
MultiProcessing::child_process_fork()
{
    pid_t pid_child = fork();
    if (pid_child == 0)
    {
        child_process();
    }
    return pid_child;
}
void
MultiProcessing::parent_process()
{
    std::cout << "Parent process fork\n";
    setupParentSocket();
    emailclass.readContent(1);  // 1.read 1 file
    std::cout << "READ ONE FILE:\n" << emailclass.message << std::endl;
    // 2.check alive of process 2

    if (check_child_alive(new_socket))
    {
        while (*shared_data <= 5)
        {
            // 3. save data to shared memory
            // std::cout << "Parent process " << *shared_data << std::endl;
            emailclass.readContent(*shared_data);
            strcpy(shared_mem, emailclass.message.c_str());
            sleep(1);
            // std::cout << "Parent process: got repsonse\n";
            (*shared_data)++;
        }
    }
    else
    {
        executeMultiProcessing();
    }
    //  4. send shutdown message to client.
    // const char* message = "Hello from server";
    const char* message = "shutdown";
    if (send(new_socket, message, strlen(message), 0) == -1)
    {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "Sent message: " << message << std::endl;

    // waitpid(pid_child, NULL, 0);
    wait(NULL);
    munmap(shared_data, SHM_SIZE);
    // munmap(email_ptr, SHM_SIZE);
    shm_unlink("/my_shm");
    close(new_socket);
    close(server_fd);
}

void
MultiProcessing::executeMultiProcessing()
{
    setupShareMemory();
    // Email email =
    pid_t pid_child = child_process_fork();
    if (pid_child > 0)
    {
        parent_process();
    }
}
// int
// main(int argc, char const* argv[])
// {
//     return 0;
// }
