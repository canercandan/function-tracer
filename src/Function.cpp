#include "Function.h"

Function::Function(size_t address, std::string const & functionName /*= std::string("None")*/) :
_address(address), _returnType(), _name(functionName), _arguments(), _hasReturnType(false)
{
}

bool Function::operator ==(Function const & rhs) const
{
    return (this->_address == rhs._address);
}

bool Function::operator ==(std::string const & name) const
{
    return (this->_name == name);
}

bool Function::operator<(Function const & rhs) const
{
    return (this->_address < rhs._address);
}

std::string const & Function::getName() const
{
    return (this->_name);
}

size_t Function::getAddress() const
{
    return (this->_address);
}

void Function::setReturnType(std::list<Type>::iterator returnType)
{
    this->_hasReturnType = true;
    this->_returnType = returnType;
}

std::list<Type>::iterator const & Function::getReturnType() const
{
    return (this->_returnType);
}

std::string Function::getReturnTypeAsString() const
{
    return (true == this->_hasReturnType ? (*this->_returnType).getName() : "");
}

std::list<Argument>& Function::getArguments()
{
    return (this->_arguments);
}

std::string Function::getArgumentsAsString() const
{
    std::string tmp("(");
    std::list<Argument>::const_iterator itab = this->_arguments.begin();
    std::list<Argument>::const_iterator itae = this->_arguments.end();
    for (; itab != itae; ++itab)
    {
	if (itab != this->_arguments.begin())
	    tmp += ", ";
	tmp += (*(*itab).getType()).getName() + " " + (*itab).getName();
    }
    tmp += ")";
    return (tmp);
}

std::ostream & operator<<(std::ostream & os, Function const & function)
{
    return (os << function.getReturnTypeAsString()
	    << function.getName()
	    << ":0x"
	    << std::hex << function.getAddress() << '\t'
	    << " " << function.getArgumentsAsString() << std::endl);
}
