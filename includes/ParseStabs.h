#ifndef PARSESTABS_H_

#define PARSESTABS_H_

#include <string>
#include <list>
#include <set>

#include "Type.h"
#include "Function.h"

class ParseStabs
{
public:
    ParseStabs(std::set<Function>&, std::list<Type>&, void*, char const*);

    void readStabs();

private:
    std::set<Function>::const_iterator parseFunction(const char *);
    void parseParam(const char *, std::set<Function>::const_iterator);
    void parseType(const char *);

    // A EFFACER (AFFICHAGE)
    void printFunctions();

private:
    std::set<Function>& _functions;
    std::list<Type>& _types;
    void * _stabSection;
    char const * _stringTable;
};

#endif //! PARSESTABS_H_
