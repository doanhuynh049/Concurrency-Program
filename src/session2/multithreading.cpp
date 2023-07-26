#include "../../include/session2/multithreading.h"

void
MainThread::simulateCencor()
{
    int value;
    std::string name;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10);
    // Start a thread manage 10 status
    for (int i = 0; i < NUM_CENCORS; i++)
    {
        cencor_names[i] = "cencor" + std::to_string(i);
        // std::cout << cencor_names[i] << std::endl;
    }
    for (int i = 1; i <= 10; i++)
    {
        name = cencor_names[dis(gen) - 1];
        value = dis(gen);
        std::cout << i << ": Random 1 cencor: " << name
                  << "\t random 1 value: " << value << std::endl;
    }
}
void
MainThread::implementMultiThreading()
{
    std::thread t1(&MainThread::simulateCencor, this);
    t1.join();
}
int
main()
{
    MainThread mainthread;
    mainthread.implementMultiThreading();
}