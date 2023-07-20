#include "../../include/session2/multiprocessing"
#include <iostream>
#include <string>
Email
read_email_file(string filename)
{
    ifstream file(filename);
    string line;
    Email email;
    if (file.is_open())
    {
        getline(file, line);
        email.sender = line.substr(line.find(":") + 2);
        file.close();
    }
    return email;
}
