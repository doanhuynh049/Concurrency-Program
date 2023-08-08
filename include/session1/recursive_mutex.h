#ifndef RECURSIVE_MUTEX_H
#define RECURSIVE_MUTEX_H
#include <iostream>
#include <mutex>
#include <thread>
std::recursive_mutex mtx;
void
function(int i);
void
implementRecursiveMutex();

#endif