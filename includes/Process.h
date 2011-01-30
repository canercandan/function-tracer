#ifndef PROCESS_H_
#define PROCESS_H_

#include <string>
#include <ostream>

class Process
{
public:
    Process(pid_t pid); // existing process
    Process(char *cmd[]); // to create

    std::string const & getPathToBinary() const;
    pid_t getPid() const;

private:
    void setPathToBinary();

private:
    pid_t _pid;
    std::string _pathToBinary;
};

std::ostream & operator<<(std::ostream & os, Process const & process);

#endif // PROCESS_H_
