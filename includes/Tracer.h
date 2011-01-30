#ifndef TRACER_H_
#define TRACER_H_

#include <map>
#include <set>
#include <stack>
#include <string>

#include <boost/variant.hpp>

#include "Process.h"
#include "Function.h"
#include "MemoryManager.h"

#include "portability.h"

#undef EIP

#if SYSTEM == 1 // BSD

#include <machine/reg.h>

typedef struct reg u_regs;

#define EIP(regs)      ((regs).r_eip)

#else // OTHERS (LINUX, ...)

#include <sys/user.h>

typedef struct user_regs_struct u_regs;

#if ARCHI == 32
#define EIP(regs)     ((regs).eip)
#else // 64
#define EIP(regs)     ((regs).rip)
#endif // !ARCHI

#endif // !SYSTEM

class Tracer
{
public:
    Tracer(Process const & process, unsigned char verboseLevel, size_t minPC);

    void init(); // Parsing Binary
    void startTracing(); // start to trace the process

private:
    void printOutIP(long ip, u_regs& regs);
    void printOutSeparator();

private:
    Process const & _process;
    unsigned char _verboseLevel;
    size_t _minPC;
    std::set<Function> _functions;
    std::map< boost::variant< size_t, std::string >, const Function* > _functionMap;
    std::list<Type> _types;
    MemoryManager _manager;

    size_t _functionAddressToReach;
    int _level;
    bool _toggle_call;
    bool _beginning_reached;
    std::stack<Function> _calls_stack;
};

#endif // TRACER_H_
