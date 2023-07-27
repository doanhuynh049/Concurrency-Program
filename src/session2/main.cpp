#include <iostream>
#include "../../include/session2/multiprocessing.h"
#include "../../include/session2/multithreading.h"

// void
// readEmail()
// {
//     std::string filename = "../../email/email_file1.txt";
//     Email email = read_email_file(filename);
//     std::cout << "Sender: " << email.sender << std::endl;
//     std::cout << "Receiver: " << email.receiver << std::endl;
//     std::cout << "Content: " << email.content << std::endl;
// }
int
main()
{
    MainThread mainthread;
    mainthread.init();
    mainthread.implementMultiThreading();
    // mainthread.cleanLogfile();
    // sleep(10);
    // mainthread.cleanLogfile();
    // MultiProcessing multiprocessing;
    // multiprocessing.executeMultiProcessing();
    // return 0;
}
