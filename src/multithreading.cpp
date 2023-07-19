#include "../include/multithreading.h"
#include <iostream>
#include <thread>
#include <vector>

void
MultiThreading::task(int id)
{
    std::cout << "Task " << id << " started" << std::endl;
    data += 10;
    std::cout << "Data: " << data << std::endl;
    std::cout << "Task" << id << " completed" << std::endl;
}
void
MultiThreading::executeMultiThreading()
{
    std::cout << "Main thread started" << std::endl;
    int num_threads = std::thread::hardware_concurrency();
    std::cout << " Number of hardware threads available: " << num_threads
              << std::endl;
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; i++)
    {
        threads.emplace_back(&MultiThreading::task, this, i + 1);
    }
    for (auto& thread : threads)
    {
        thread.join();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    std::cout << "All tasks completed in " << duration.count() << "ms"
              << std::endl;
    std::cout << "Main thread completed" << std::endl;
}