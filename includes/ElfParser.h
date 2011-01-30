#ifndef ELFPARSER_H_
#define ELFPARSER_H_

#include <set>
#include <map>
#include <string>

#include <boost/variant.hpp>

#include <stdint.h>

#include "Function.h"
#include "MemoryManager.h"
#include "ElfPortability.h"

class ElfParser
{
    typedef std::set<Function> FunctionSet;
    typedef std::map< boost::variant< size_t, std::string >, const Function* > FunctionMap;

public:
    ElfParser(std::set<Function> & functions, std::list<Type>& types, MemoryManager & manager);
    //ElfParser(FunctionSet& functions, FunctionMap& functionMap, std::list<Type>& types, MemoryManager & manager);
    ~ElfParser();

    void parse(std::string const & binary, bool isMainBinary);

private:
    void parseElf();
    void reset();
    void readElfInMemory();

    int checkFile(std::string const & path, struct stat* fileStatus) const;
    bool isElfBinary(unsigned char const * magicCode) const;

    int getIndexInHeaderSection(Elf_Word typeSearch) const;
    int getIndexInHeaderSection(const char * typeSearch) const;
    int getIndexInHeaderProgram(Elf_Word typeSearch) const;
    int getIndexInDynamicSection(Elf_Sxword searchType) const;
    size_t getBaseAddress();

    void getSharedLibraryDependencies(std::list<std::string> & dependencies);

    void readHeaderSection();
    void readDebug();
    void readSymbolTable(Elf_Xword sizeSection);
    void readDynamicSection(std::list<std::string>& dependencies);

    std::string getFullPath(std::string const & dependence);

private:
    void* _headerElf;
    void* _headerProgram;
    void* _headerSection;
    void* _symbolTable;
    char const * _stringTableSymTab;
    void* _dynamicSection;
    char const * _stringTableDynamicSection;
    std::set<Function>& _functions;
    std::list<Type>& _types;
    std::list<std::string> _libs;
    MemoryManager& _manager;
};

#endif // ELFPARSER_H_
