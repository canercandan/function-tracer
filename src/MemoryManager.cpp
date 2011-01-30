#include <ctime>
#include <cstdlib>
#include <cstdio>

#include <iostream>

#include <sys/types.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <errno.h>

#include "MemoryManager.h"
#include "portability.h"

MemoryManager::MemoryManager(pid_t pid) :
_pid(pid), _addr(0), _size(0)
{
}

MemoryManager::~MemoryManager()
{
    free(this->_addr);
}

#if SYSTEM == 1

void * MemoryManager::readSpiedMemory(void* addr, size_t sizeToRead)
{
    if (0 != this->_addr)
    {
	struct ptrace_io_desc iodesc;
	iodesc.piod_op = PIOD_READ_D;
	iodesc.piod_offs = addr;
	iodesc.piod_addr = this->_addr;
	iodesc.piod_len = sizeToRead;
	/*      std::cerr << "trying to read at " << std::hex << addr
		<< " nb octet : " << std::dec << sizeToRead << std::endl;*/
	if (-1 == ptrace(PT_IO, this->_pid, (caddr_t) & iodesc, 0))
	{
	    perror("ptrace(PT_IO) : ");
	    return (0);
	}
	if (sizeToRead == iodesc.piod_len)
	{
	    //std::cerr << "REad success" << std::endl;
	    return (this->_addr);
	}
	std::cerr << "ptrace(PT_IO) : Size wanted : " << sizeToRead << " -  real read : " << iodesc.piod_len << std::endl;
    }
    return (0);
}

#else

void * MemoryManager::readSpiedMemory(void* addr, size_t sizeToRead)
{
    if (0 != this->_addr)
    {
	//std::cerr << "readSpiedMemory" << std::endl;
	size_t bytesRead = 0, addrToRead = (size_t) addr, index = 0;
	//std::cout << "adresse a ecrire : " << this->_addr << std::endl;
	size_t * writeZone = (size_t *)this->_addr;
	//std::cout << "adresse a ecrire writezone : " << writeZone << std::endl;
	//std::cerr << "va t-il y avoir des restes ? " << sizeToRead << " % " << sizeof(long) << " = " << sizeToRead % sizeof(long) << std::endl;
	while (bytesRead < sizeToRead)
	{
	    errno = 0;
	    long valueRead = ptrace(PTRACE_PEEKTEXT, this->_pid, addrToRead + bytesRead, 0);
	    if (-1 == valueRead && 0 != errno)
	    {
		perror("ptrace(PTRACE_PEEKTEXT) : ");
		return (0);
	    }
	    bytesRead += sizeof (bytesRead);
	    writeZone[index++] = valueRead;
	}
	/*std::cout << "adresse a ecrire writezone : " << writeZone + index << std::endl;
	std::cout << "Fin normale : " << std::hex << (size_t)this->_addr + sizeToRead << std::endl;
	std::cerr << "fin readSpiedMemory" << std::endl;*/
    }
    return (this->_addr);
}
#endif

void * MemoryManager::readMemory(void* addr, size_t sizeToRead)
{
    if (sizeToRead > this->_size)
    {
	free(this->_addr);
	this->_addr = malloc(sizeToRead);
	if (0 == this->_addr)
	{
	    perror("malloc : ");
	}
	else
	{
	    this->_size = sizeToRead;
	}
    }
    return (this->readSpiedMemory(addr, sizeToRead));
}

std::string MemoryManager::readString(void *addr)
{

    union un
    {
	long read;
	char charRead[sizeof (long) ];
    };

    un valueRead;
    std::string stringRead;
    size_t i = 0, addrToRead = (size_t) addr;
    do
    {
	errno = 0;
	valueRead.read = ptrace(PTRACE_PEEKTEXT, this->_pid, addrToRead, 0);
	if (-1 == valueRead.read && 0 != errno)
	{
	    perror("ptrace(PTRACE_PEEKTEXT) : ");
	    return (0);
	}
	i = 0;
	while (i < sizeof (long) && valueRead.charRead[i])
	{
	    stringRead.push_back(valueRead.charRead[i++]);
	}
	addrToRead += sizeof (addrToRead);
    }
    while (i == sizeof (long));
    return (stringRead);
}
