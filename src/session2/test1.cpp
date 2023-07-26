#include <chrono>
#include <queue>
#include <random>
#include <string>
#include <thread>

class MainThread
{
public:
    void
    simulateCencor()
    {
        for (int i = 0; i < 100; i++)
        {
            // Simulate 10 censor values
            for (int j = 0; j < 10; j++)
            {
                // Generate random name and value
                string name = censorNames[rand() % 10];
                int value = rand() % 10 + 1;
                // Put message in queue
                messageQueue.push({ name, value });
            }
            // Sleep for 1 second
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

private:
    string censorNames[10] = { "censor1", ... };
    std::queue<std::pair<std::string, int>> messageQueue;
};

int
main()
{
    // Create main thread object
    MainThread mainThread;

    // Start simulator thread
    std::thread t1(&MainThread::simulateCencor, &mainThread);

    // Wait for thread to finish
    t1.join();

    // Process messages from queue
    while (!mainThread.messageQueue.empty())
    {
        auto msg = mainThread.messageQueue.front();
        std::cout << msg.first << ": " << msg.second << "\n";
        mainThread.messageQueue.pop();
    }
}