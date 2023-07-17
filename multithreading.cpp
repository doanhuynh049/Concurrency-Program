#include <iostream>
#include <thread>

void task(int id) {
    std::cout << "Task " << id << " started" << std::endl;

    // Simulate some work
    for (int i = 0; i < 1000000000; i++) {
        // Do nothing
    }

    std::cout << "Task " << id << " completed" << std::endl;
}

int main() {
    std::cout << "Main thread started" << std::endl;

    // Create some tasks
    std::thread t1(task, 1);
    std::thread t2(task, 2);

    // Wait for the tasks to complete
    t1.join();
    t2.join();

    std::cout << "Main thread completed" << std::endl;

    return 0;
}

