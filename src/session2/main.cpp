#include <iostream>
#include "../../include/session2/multiprocessing.h"
void
readEmail()
{
    std::string filename = "../../email/email_file1.txt";
    Email email = read_email_file(filename);
    std::cout << "Sender: " << email.sender << std::endl;
    std::cout << "Receiver: " << email.receiver << std::endl;
    std::cout << "Content: " << email.content << std::endl;
}
int
main()
{
    executeMultiprocessing();
    return 0;
}