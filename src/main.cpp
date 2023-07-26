#include <unistd.h>
#include <iostream>

#include "../include/session1/condition_var.h"
#include "../include/session1/fork.h"
#include "../include/session1/multithreading.h"
#include "../include/session1/mutex_multithreading.h"
#include "../include/session1/named-pipe.h"
#include "../include/session2/multiprocessing.h"

int
main()
{
    // implementFork1();
    // implementMultiFork();
    // executeProcess();
    // implementShareMemory();
    // NamedPipe* namedpipe = new NamedPipe();
    // namedpipe->implementPipe();
    // MultiThreading* multithreading = new MultiThreading();
    // multithreading->executeMultiThreading();
    // implementFork12();
    // MuTex* mutex = new MuTex();
    // mutex->executeMutex();
    // ConditionVar conditionvar;
    // conditionvar.executeConditionVar();
    MultiProcessing multiprocessing;
    multiprocessing.executeMultiProcessing();
    return 0;
}