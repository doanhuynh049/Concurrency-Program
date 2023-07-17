#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void task(int id) {
    std::cout << "Task " << id << " started" << std::endl;

    // Acquire the lock
    std::unique_lock<std::mutex> lock(mtx);

    // Wait for the condition to be true
    while (!ready) {
        cv.wait(lock);
    }

    // Do some work
    std::cout << "Task " << id << " completed" << std::endl;
}

int main() {
    std::cout << "Main thread started" << std::endl;

    // Create some tasks
    std::thread t1(task, 1);
    std::thread t2(task, 2);

    // Simulate some work
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Set the condition to true
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_all();

    // Wait for the tasks to complete
    t1.join();
    t2.join();

    std::cout << "Main thread completed" << std::endl;

    return 0;
}

