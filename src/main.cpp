#include <unistd.h>
#include <iostream>

#include "../include/fork.h"
#include "../include/multithreading.h"
#include "../include/mutex_multithreading.h"
#include "../include/named-pipe.h"

int
main()
{
    // implementFork1();
    // implementMultiFork();
    // executeProcess();
    // implementShareMemory();
    // NamedPipe* namedpipe = new NamedPipe();
    // namedpipe->implementPipe();
    MultiThreading* multithreading = new MultiThreading();
    multithreading->executeMultiThreading();
    // implementFork12();
    // MuTex* mutex = new MuTex();
    // mutex->executeMutex();
    return 0;
}