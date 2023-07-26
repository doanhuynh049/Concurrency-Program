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

public:
    struct Message
    {
        std::string cencorName;
        int cencorValue;
    };

    void
    simulateCencor();
    void
    implementMultiThreading();
};
#endif