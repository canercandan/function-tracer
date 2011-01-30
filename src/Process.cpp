#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <climits>
#include <fstream>

#include <sys/ptrace.h>

#include "Process.h"
#include "portability.h"
#include "TypeToString.hpp"

#if SYSTEM == 0
const std::string defaultSymLink = "/exe";
#else
const std::string defaultSymLink = "/file";
#endif

Process::Process(pid_t pid) :
_pid(pid), _pathToBinary("")
{
    std::cerr << "Attach" << std::endl;
    if (ptrace(PT_ATTACH, pid, 0, 0) == -1)
    {
	perror("ptrace(ATTACH) : ");
	exit(EXIT_FAILURE);
    }
    this->setPathToBinary();
}

Process::Process(char *cmd[]) :
_pid(fork()), _pathToBinary("")
{
    if (this->_pid == -1)
    {
	perror("fork : ");
	exit(EXIT_FAILURE);
    }
    if (this->_pid == 0)
    {
	if (ptrace(PT_TRACE_ME, 0, 0, 0) == -1)
	{
	    perror("ptrace(TRACE_ME) : ");
	    exit(EXIT_FAILURE);
	}

	if (execvp(cmd[0], cmd) == -1)
	{
	    perror("execvp : ");
	    exit(EXIT_FAILURE); // utile?
	}
    }
    else
    {
	sleep(2);
	std::cerr << "process create " << std::endl;
	this->setPathToBinary();
    }
}

void Process::setPathToBinary()
{
    std::string symbolicLink("/proc/" + typeToString(this->_pid) + defaultSymLink);
    char buffer[PATH_MAX];
    ssize_t size = readlink(symbolicLink.c_str(), buffer, PATH_MAX);
    if (size == -1)
    {
	perror("readlink : ");
    }
    else
    {
	buffer[size] = '\0';
	this->_pathToBinary = buffer;
    }
}

std::string const & Process::getPathToBinary() const
{
    return (this->_pathToBinary);
}

pid_t Process::getPid() const
{
    return (this->_pid);
}

std::ostream & operator<<(std::ostream & os, Process const & process)
{
    return (os << "Traced Process  is :\nPid : " << process.getPid() << std::endl
	    << "Binary use : " << process.getPathToBinary() << std::endl);
}
