#include "Argument.h"

Argument::Argument(std::string const & name) :
_name(name), _type(0)
{
}

Argument::Argument(std::string const & name, std::list<Type>::iterator type) :
_name(name), _type(type)
{
}

Argument::Argument(Argument const & rhs) :
_name(rhs._name), _type(rhs._type)
{
}

Argument& Argument::operator=(Argument const & rhs)
{
    if (this != &rhs)
    {
	this->_name = rhs._name;
	this->_type = rhs._type;
    }
    return (*this);
}

void Argument::setType(std::list<Type>::iterator type)
{
    this->_type = type;
}

std::list<Type>::iterator const & Argument::getType() const
{
    return (this->_type);
}

std::string const & Argument::getName() const
{
    return (this->_name);
}
