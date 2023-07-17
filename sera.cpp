#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <semaphore.h>

sem_t sem;

void task(int id)
{
    std::cout << "Task " << id << " started" << std::endl;

    sem_wait(&sem);

    std::cout << "Task " << id << " completed" << std::endl;

    sem_post(&sem);
}

int main()
{
    std::cout << "Main thread started" << std::endl;

    sem_init(&sem, 0, 2);

    std::thread t1(task, 1);
    std::thread t2(task, 2);
    std::thread t3(task, 3);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "Main thread completed" << std::endl;

    sem_destroy(&sem);

    return 0;
}
