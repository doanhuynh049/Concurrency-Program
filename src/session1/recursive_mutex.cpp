#include "../../include/session1/recursive_mutex.h"
void
function(int i)
{
    mtx.lock();
    std::cout << "Thread " << std::this_thread::get_id() << "acquired lock"
              << std::endl;
    if (i > 1)
    {
        function(i - 1);
    }
    std::cout << "Thread " << std::this_thread::get_id() << "releasing lock"
              << std::endl;
    mtx.unlock();
}
void
implementRecursiveMutex()
{
    std::thread t1(function, 3);
    t1.join();
}