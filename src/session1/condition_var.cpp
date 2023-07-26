#include "../../include/session1/condition_var.h"
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

void
ConditionVar::task(int id)
{
    std::unique_lock<std::mutex> lock(mtx);
    mtx.lock();
    while (!ready)
    {
        cv.wait(lock);
    }
    mtx.unlock();
    std::cout << "Thread " << id << " got the signal!" << std::endl;
}
void
ConditionVar::executeConditionVar()
{
    // std::cout << "Main thread of Condition Variable started " << std::endl;
    // int number_thread = std::thread::hardware_concurrency();
    // std::cout << "Number of thread: " << number_thread << std::endl;
    // std::vector<std::thread> threads;
    // threads.reserve(number_thread);

    // auto start_time = std::chrono::high_resolution_clock::now();
    // for (int i = 0; i < number_thread; i++)
    // {
    //     threads.emplace_back(&ConditionVar::task, this, i + 1);
    // }

    // std::this_thread::sleep_for(std::chrono::seconds(1));

    // {
    //     std::lock_guard<std::mutex> lock(mtx);
    //     ready = true;
    // }
    // cv.notify_all();
    // std::cout << "Main thread: Sending signal..." << std::endl;
    // for (auto& thread : threads)
    // {
    //     thread.join();
    // }

    // auto end_time = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
    //     end_time - start_time);
    // std::cout << "All tasks completed in " << duration.count() << " ms"
    //           << std::endl;
    // std::cout << "Main thread completed" << std::endl;

    std::vector<std::thread> threads;
    int num_threads = 12;

    for (int i = 0; i < num_threads; i++)
    {
        threads.emplace_back(&ConditionVar::task, this, i + 1);
    }

    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_all();

    std::cout << "Main thread: Sending signal..." << std::endl;

    for (auto& thread : threads)
    {
        thread.join();
    }
    std::cout << "Main thread: All worker threads joined." << std::endl;
}