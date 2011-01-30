#include <iostream>
#include <algorithm>
#include <utility>

#include <stab.h>

#include "portability.h"

/* Stabs entries use a 12 byte format:
 * 4 byte string table index
 * 1 byte stab type
 * 1 byte stab other field
 * 2 byte stab desc field
 * 4 byte stab value
 */

#if ARCHI == 64

struct nlist
{

    union
    {
	unsigned int n_strx;
    } n_un;
    unsigned char n_type;
    char n_other;
    short n_desc;
    unsigned int n_value;
};
#else
#    include <a.out.h>
#endif

#include "ParseStabs.h"
#include "StringToType.hpp"

ParseStabs::ParseStabs(std::set<Function>& functions, std::list<Type>& types, void* stabSection, char const* stringTable) :
_functions(functions), _types(types), _stabSection(stabSection), _stringTable(stringTable)
{
}

/**
 * Found the function in the setList then set his return type and
 * return the founded function
 */
std::set<Function>::const_iterator ParseStabs::parseFunction(const char * newLine)
{
    std::string tmpLine(newLine);
    std::set<Function>::const_iterator tmpIt = std::find(this->_functions.begin(), this->_functions.end(), tmpLine.substr(0, tmpLine.find_first_of(':', 0)));
    if (tmpIt != this->_functions.end())
    {
	Function & oneFunc = const_cast<Function&> (*tmpIt);

	std::string leftInt;
	leftInt = tmpLine.substr(tmpLine.find_first_of('(', 0) + 1, tmpLine.find_first_of(',', 0) - tmpLine.find_first_of('(', 0) - 1);
	std::string rightInt;
	rightInt = tmpLine.substr(tmpLine.find_first_of(',', 0) + 1, tmpLine.find_first_of(')', 0) - tmpLine.find_first_of(',', 0) - 1);
	int leftIndex = stringToType<int>(leftInt);
	int rightIndex = stringToType<int>(rightInt);
	std::pair<int, int> typeIndex(leftIndex, rightIndex);
	std::cout << tmpLine.substr(0, tmpLine.find_first_of(':', 0)) << " = bzzzzzz : " << leftIndex << " - " << rightIndex << std::endl << "ligne : " << newLine << std::endl;
	std::list<Type>::iterator returnType = std::find(this->_types.begin(), this->_types.end(), typeIndex);
	if (returnType != this->_types.end())
	{
	    oneFunc.setReturnType(returnType);

	    std::cout << "SETRETURN TYPE !!!!! : " << (*oneFunc.getReturnType()).getName() << std::endl;
	}
    }
    std::list<Type>::iterator returnType = (*tmpIt).getReturnType();
    std::cout << "SETRETURN TYPE !!!!! : " << (*returnType).getName() << std::endl;
    return (tmpIt);
}

/**
 * Set the function arguments for given function
 */
void ParseStabs::parseParam(const char * newLine, std::set<Function>::const_iterator funcIt)
{
    std::string tmpLine(newLine);
    Function & oneFunc = const_cast<Function&> (*funcIt);

    std::string name;
    name = tmpLine.substr(0, tmpLine.find_first_of(':', 0));

    std::string leftInt;
    leftInt = tmpLine.substr(tmpLine.find_last_of('(', tmpLine.size()) + 1, tmpLine.find_last_of(',', tmpLine.size()) - tmpLine.find_last_of('(', tmpLine.size()) - 1);
    std::string rightInt;
    rightInt = tmpLine.substr(tmpLine.find_last_of(',', tmpLine.size()) + 1, tmpLine.find_last_of(')', tmpLine.size()) - tmpLine.find_last_of(',', tmpLine.size()) - 1);
    int leftIndex = stringToType<int>(leftInt);
    int rightIndex = stringToType<int>(rightInt);
    std::cout << leftInt << " - " << rightInt << std::endl << "ligne : " << name << std::endl;
    std::pair<int, int> typeIndex(leftIndex, rightIndex);
    std::list<Type>::iterator ParamType = std::find(this->_types.begin(), this->_types.end(), typeIndex);
    if (ParamType != this->_types.end())
    {
	oneFunc.getArguments().push_back(Argument(name, ParamType));
    }
    else
    {
	std::cout << "J'enregistre le nom de larg uniquement" << std::endl;
	oneFunc.getArguments().push_back(Argument(name));
    }
}

/**
 * Set the type described within the line
 */
void ParseStabs::parseType(const char * newLine)
{
    std::string tmpLine(newLine);

    std::string name;
    name = tmpLine.substr(0, tmpLine.find_first_of(':', 0));

    if (tmpLine.substr(tmpLine.find_first_of(':', 0) + 1, 1) == "t")
    {

	std::string leftInt;
	leftInt = tmpLine.substr(tmpLine.find_first_of('(', 0) + 1, tmpLine.find_first_of(',', 0) - tmpLine.find_first_of('(', 0) - 1);
	std::string rightInt;
	rightInt = tmpLine.substr(tmpLine.find_first_of(',', 0) + 1, tmpLine.find_first_of(')', 0) - tmpLine.find_first_of(',', 0) - 1);
	int leftIndex = stringToType<int>(leftInt);
	int rightIndex = stringToType<int>(rightInt);

	leftInt = tmpLine.substr(tmpLine.find_last_of('(', tmpLine.size()) + 1, tmpLine.find_last_of(',', tmpLine.size()) - tmpLine.find_last_of('(', tmpLine.size()) - 1);
	rightInt = tmpLine.substr(tmpLine.find_last_of(',', tmpLine.size()) + 1, tmpLine.find_last_of(')', tmpLine.size()) - tmpLine.find_last_of(',', tmpLine.size()) - 1);
	int leftParentIndex = stringToType<int>(leftInt);
	int rightParentIndex = stringToType<int>(rightInt);
	std::list<Type>::iterator ParentTypeIt = this->_types.end();
	if (leftIndex != leftParentIndex || rightIndex != rightParentIndex)
	{
	    std::pair<int, int> typeIndex(leftParentIndex, rightParentIndex);
	    ParentTypeIt = std::find(this->_types.begin(), this->_types.end(), typeIndex);
	}
	else std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1" << std::endl;
	std::list<Type>::iterator returnTypeIt = std::find(this->_types.begin(), this->_types.end(), name);
	if (returnTypeIt != this->_types.end())
	{
	    (*returnTypeIt).setTypeIndex(leftIndex, rightIndex);
	    if (ParentTypeIt != this->_types.end())
		(*returnTypeIt).setParentType(ParentTypeIt);
	}
	else
	{
	    Type oneType(name);
	    oneType.setTypeIndex(leftIndex, rightIndex);
	    if (ParentTypeIt != this->_types.end())
	    {
		oneType.setParentType(ParentTypeIt);
	    }
	    this->_types.push_back(oneType);
	}
    }
}
// http://sources.redhat.com/gdb/onlinedocs/stabs.html#index-N_005fFUN_002c-for-functions-13
// http://tldp.org/LDP/LG/issue85/sandeep.html
// http://www.google.com/codesearch/p?hl=en#4FSOSMZ6Pxc/distfiles/binutils-2.14.tar.bz2|kJcUciuu3bI/binutils-2.14/binutils/objdump.c&q=objdump

void ParseStabs::printFunctions()
{
    std::cout << "----------------TYPES-------------------" << std::endl;
    std::list<Type>::iterator itb = this->_types.begin();
    std::list<Type>::iterator ite = this->_types.end();
    for (; itb != ite; ++itb)
    {
	std::cout << (*itb).getName() << "(";
	(*itb).printIndex();
	std::cout << ")" << std::endl;
    }

    std::cout << "-----------------FUNC--------------------" << std::endl;
    std::set<Function>::iterator itfb = this->_functions.begin();
    std::set<Function>::iterator itfe = this->_functions.end();
    for (; itfb != itfe; ++itfb)
    {
	std::cout << (*itfb);
    }
    std::cout << "----------------------END---------------" << std::endl;
}

void ParseStabs::readStabs()
{
    struct nlist const * stabLine = static_cast<struct nlist const *> (this->_stabSection);
    std::cout << "Addr Section Stab:" << this->_stabSection << "\tnb de symbole a lire : " << std::dec << stabLine->n_desc << std::endl;
    unsigned int stayingSymbol = stabLine->n_desc;
    stabLine += 1;
    std::set<Function>::const_iterator lastFunction = this->_functions.end();
    while (stayingSymbol--)
    {
	std::cout << "A parser " << (int) stabLine->n_type << " --> " << this->_stringTable + stabLine->n_un.n_strx << std::endl;
	switch (stabLine->n_type)
	{
	    case N_FUN: //function
		lastFunction = this->parseFunction(this->_stringTable + stabLine->n_un.n_strx);
		break;
	    case N_PSYM: // Parameter
		this->parseParam(this->_stringTable + stabLine->n_un.n_strx, lastFunction);
		break;
	    case N_LSYM: // Type
		this->parseType(this->_stringTable + stabLine->n_un.n_strx);
		break;
	}
	stabLine += 1;
    }
    this->printFunctions();
}
