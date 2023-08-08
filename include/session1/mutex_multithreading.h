#ifndef MUTEX_H
#define MUTEX_H
#include <iostream>
#include <mutex>
class MuTex
{
public:
    std::mutex mtx;
    void
    task(int id);
    void
    executeMutex();

private:
    float data = 0;
};

#endif