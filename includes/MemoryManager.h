#ifndef MEMORYMANAGER_H_
#define MEMORYMANAGER_H_

class MemoryManager
{
public:
    MemoryManager(pid_t pid);
    ~MemoryManager();

    void * readMemory(void * addr, size_t sizeToRead);
    std::string readString(void *addr);

private:
    void * readSpiedMemory(void * addr, size_t sizeToRead);

private:
    pid_t _pid;
    void * _addr;
    size_t _size;
};

#endif // MEMORYMANAGER_H_
