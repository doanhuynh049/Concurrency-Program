#ifndef NAMED_PIPE_H
#define NAMED_PIPE_H
#include <cstring>
#include <iostream>
#include <string>

class NamedPipe
{
public:
    void
    child_process();
    void
    parent_process(int timeout_ms = -1);
    void
    implementPipe();

private:
    // use std::string instead of char * to make it easier to modify later
    const std::string PIPE_NAME = "my_pipe";
};
#endif