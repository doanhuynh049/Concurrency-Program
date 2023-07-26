#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include <chrono>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

const int NUM_CENCORS = 10;

class MainThread
{
private:
    std::string cencor_names[10];
    std::queue<std::pair<std::string, int>> messageQueue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    struct Message
    {
        std::string cencorName;
        int cencorValue;
    };
    void
    init();
    void
    simulateCencor();
    void
    implementMultiThreading();
    void
    printMessageQueue();
    void
    cleanLogfile();
    void
    handleCencor();
};
#endif