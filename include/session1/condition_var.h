#ifndef CONDITION_VAR_H
#define CONDITION_VAR_H
#include <condition_variable>
#include <mutex>
class ConditionVar
{
public:
    void
    task(int id);
    void
    executeConditionVar();

private:
    int data = 0;
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;
};
#endif