#include "../../include/session2/multithreading.h"
void
MainThread::init()
{
    for (int i = 0; i < NUM_CENCORS; i++)
    {
        cencor_names[i] = "cencor" + std::to_string(i);
        // std::cout << cencor_names[i] << std::endl;
    }
}
void
MainThread::simulateCencor()
{
    int value;
    std::string name;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);
    // Start a thread manage 10 status

    for (int j = 0; j < 3; j++)
    {
        for (int i = 1; i <= 10; i++)
        {
            name = cencor_names[dis(gen) - 1];
            value = dis(gen);
            messageQueue.push({ name, value });
            // std::cout << i << ": Random 1 cencor: " << name
            //           << "\t random 1 value: " << value << std::endl;
            // std::cout << messageQueue;
        }
        // std::cout << j;
        printMessageQueue();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
void
MainThread::printMessageQueue()
{
    std::queue<std::pair<std::string, int>> message_queue = messageQueue;

    std::cout << "Print Message Queue\n";
    while (!message_queue.empty())
    {
        auto message = message_queue.front();
        std::cout << "Message: " << message.first
                  << ", Value: " << message.second << std::endl;
        message_queue.pop();
    }
    std::cout << "Done print\n";
}
void
MainThread::cleanLogfile()
{
    std::string logfile_pre = "../../email/logfile/";
    for (int i = 0; i < NUM_CENCORS; i++)
    {
        std::string logfile = logfile_pre + cencor_names[i] + ".log";
        // std::cout << logfile << cencor_names[i];
        std::ofstream file(logfile, std::ios_base::trunc);
        if (!file.is_open())
        {
            std::cerr << "Failed to clean log file" << std::endl;
            continue;
        }
        file.close();
    }
}
void
MainThread::handleCencor()
{
    // std::cout << "multithreading";
    while (true)
    {
        std::unique_lock<std ::mutex> lock(mtx);
        cv.wait(lock, [this] { return !messageQueue.empty(); });
        if (messageQueue.empty())
        {
            continue;
        }
        auto message = messageQueue.front();
        messageQueue.pop();
        std::cout << "handleCencor: ";
        std::cout << "Message: " << message.first
                  << ", Value: " << message.second << std::endl;
        lock.unlock();
        cv.notify_one();
        std::string log_file = "../../email/logfile/" + message.first + ".log";
        // std::cout << log_file;
        std::ofstream file(log_file, std::ios_base::app);
        if (!file.is_open())
        {
            std::cerr << "Failed to open log file " << message.first << ".log "
                      << log_file << std::endl;
            continue;
        }
        file << message.second << ", ";
        file.close();
    }
}
void
MainThread::implementMultiThreading()
{
    std::thread t1(&MainThread::simulateCencor, this);
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; i++)
    {
        threads.emplace_back(&MainThread::handleCencor, this);
    }
    t1.join();

    for (auto& thread : threads)
    {
        thread.join();
    }
}
int
main()
{
    MainThread mainthread;
    mainthread.init();
    mainthread.implementMultiThreading();
    // mainthread.cleanLogfile();
}