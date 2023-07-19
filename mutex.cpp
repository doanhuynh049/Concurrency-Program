#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;
int shared_var = 0;

void
task(int id)
{
    std::cout << "Task " << id << " started" << std::endl;

    // Acquire the lock
    mtx.lock();

    // Increment the shared variable
    shared_var++;

    // Release the lock
    mtx.unlock();

    std::cout << "Task " << id << " completed" << std::endl;
}

int
main()
{
    std::cout << "Main thread started" << std::endl;

    // Create some tasks
    std::thread t1(task, 1);
    std::thread t2(task, 2);

    // Wait for the tasks to complete
    t1.join();
    t2.join();

    std::cout << "Shared variable value: " << shared_var << std::endl;
    std::cout << "Main thread completed" << std::endl;

    return 0;
}
