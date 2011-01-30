#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <csignal>
#include <cstdio>

#include <sys/wait.h>
#include <sys/ptrace.h>

#include "Tracer.h"
#include "ElfParser.h"

Tracer::Tracer(Process const & process, unsigned char verboseLevel, size_t minPC) :
_process(process), _verboseLevel(verboseLevel), _minPC(minPC),
_functions(), _types(), _manager(process.getPid()),
_functionAddressToReach(0),
_level(0),
_toggle_call(false),
_beginning_reached(false)
{
    std::cout << "minPC: " << _minPC << std::endl;
}

void Tracer::init()
{
    std::cout << "Tracer::init" << std::endl;
    ElfParser parser(this->_functions, this->_types, this->_manager);
    /* ElfParser parser(this->_functions, this->_functionMap, this->_types, this->_manager);*/
    int status;
    if (-1 == waitpid(this->_process.getPid(), &status, WUNTRACED))
    {
	perror("waitpid : ");
    }

    parser.parse(this->_process.getPathToBinary(), true);

#warning "cette fonction n'est pas necessaire, faire un std::find ds le set des fonctions pour la trouver"
    //this->_functionAddressToReach = (_minPC == 0) ? parser.getMainFunctionAddress() : _minPC;
    // if (_minPC == 0 || this->_functionMap.find(_minPC) == this->_functionMap.end())
    //   this->_functionAddressToReach = this->_functionMap["main"]->getAddress();
    // else
    //   this->_functionAddressToReach = _minPC;

    /*std::cout << "WIFEXITED " << std::dec << WIFEXITED(status) << "-->" << std::boolalpha << WIFEXITED(status) << std::endl;
    std::cout << "WEXITSTATUS " << std::dec << WEXITSTATUS(status) << "-->" << std::boolalpha << WEXITSTATUS(status) << std::endl;
    std::cout << "WIFSIGNALED " << std::dec << WIFSIGNALED(status) << "-->" << std::boolalpha << WIFSIGNALED(status) << std::endl;
    std::cout << "WIFSTOPPED " << std::dec << WIFSTOPPED(status) << "-->" << std::boolalpha << WIFSTOPPED(status) << std::endl;
    std::cout << "WSTOPSIG " << std::dec << WSTOPSIG(status) << "-->" << std::boolalpha << WSTOPSIG(status) << std::endl;
    std::cout << "WIFCONTINUED " << std::dec << WIFCONTINUED(status) << "-->" << std::boolalpha << WIFCONTINUED(status) << std::endl;

    if (-1 == kill(this->_process.getPid(), SIGCONT))
    {
	perror("kill : ");
    }
    
      if (-1 == ptrace(PT_CONTINUE, this->_process.getPid(), 0, SIGCONT))
      {
      perror("ptrace(PT_CONTINUE) : ");
      }*/
}

void Tracer::printOutIP(long ip, u_regs& regs)
{
    if (_beginning_reached == false)
    {
	if (EIP(regs) != _functionAddressToReach)
	    return;
	std::cout << "BEGINNING REACHED" << std::endl;
	_beginning_reached = true;
	this->_calls_stack.push(Function(_functionAddressToReach, "main"));
    }


    // if ((ip & 0xFF) == 0xC3)
    //   {
    //     std::cout << std::hex;
    //     std::cout << "OTHERWISE -> EIP: " << EIP(regs) << " ";
    //     std::cout << "IP: " << ip << " ";
    //     std::cout << std::endl;
    //   }

    if ((ip & 0xFF) == 0xE8) // CALL
    {
	_toggle_call = true;
	_level++;
    }
    else if ((ip & 0xFF) == 0xC3) // RET
    {
	std::cout << this->_calls_stack.top().getName()
		<< "\t:0x" << std::hex
		<< this->_calls_stack.top().getAddress();
	this->printOutSeparator();
	if (!this->_calls_stack.empty())
	    this->_calls_stack.pop();
	std::cout << "\tRET";
	std::cout << std::endl;
	_level--;
    }
    else
    {
	if (_toggle_call == true)
	{
	    Function pattern((size_t) EIP(regs));
	    std::set<Function>::iterator it = _functions.find(pattern);
	    std::set<Function>::iterator end = _functions.end();

	    if (it != end)
	    {
		std::cout << this->_calls_stack.top().getName()
			<< "\t:0x" << std::hex
			<< this->_calls_stack.top().getAddress();
		this->printOutSeparator();
		this->_calls_stack.push(*it);
		std::cout << "\t"
			<< this->_calls_stack.top().getName()
			<< this->_calls_stack.top().getArgumentsAsString()
			<< std::endl;

	    }
	    _toggle_call = false;
	}
	else // OTHERWISE
	{
	}
    }
}

void Tracer::printOutSeparator()
{
    int i;

    for (i = 0; i < this->_level; i++)
    {
	std::cout << " | ";
    }
}

void Tracer::startTracing()
{
    std::cerr << "Tracer::startTracing" << std::endl;
    while (1)
    {
	int status;
	pid_t pid = wait4(this->_process.getPid(), &status, WUNTRACED, 0);
	if (pid == -1 && errno != EINTR)
	{
	    perror("wait4 : ");
	    break;
	}

	int signal = 0;

	if (WIFEXITED(status)) // vrai si proccess s'est termine normalement
	{
	    std::cout << "exit = " << WEXITSTATUS(status) << std::endl;
	    break;
	}

	if (WIFSIGNALED(status)) // vrai si proccess s'est termine a cause d'un signal
	{
	    std::cout << "\nWIFSIGNALED : Number of the signal : " << WTERMSIG(status) << std::endl;
	    break;
	}

	if (WIFSTOPPED(status))
	{
	    if (WSTOPSIG(status) == SIGTRAP) // SIGTRAP: instruction terminee
	    {
		u_regs regs;

		//! get registers
#if SYSTEM == 1 // BSD
		if (ptrace(PT_GETREGS, pid, (caddr_t) & regs, sizeof (regs)) == -1)
#else // OTHERS (LINUX, ...)
		if (ptrace(PT_GETREGS, pid, 0, &regs) == -1)
#endif // !SYSTEM
		    perror("ptrace(GETREGS) :");
		else
		{
#if SYSTEM == 1 // BSD
		    long ip = ptrace(PT_READ_I, pid, (caddr_t) EIP(regs), 0);
#else // OTHERS (LINUX, ...)
		    long ip = ptrace(PT_READ_I, pid, EIP(regs), 0);
#endif // !SYSTEM

		    if (ip == -1)
			perror("ptrace(READ_I) : ");
		    else
		    {
			printOutIP(ip, regs);

			if (_level < 0)
			    break;
		    }
		}
	    }
	}
	else
	{
	    signal = WSTOPSIG(status);
	}
	if (ptrace(PT_STEP, pid, (caddr_t) 1, signal) == -1)
	{
	    perror("ptrace(STEP) : ");
	    exit(EXIT_FAILURE);
	}
    }
}
