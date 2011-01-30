#include <cstring>

#include "Type.h"

Type::Type(std::string const & name) :
_name(name), _haveParent(false), _parent(0), _firstIndex(0), _secondIndex(0)
{
}

Type::Type(std::string const & name, std::list<Type>::iterator parent) :
_name(name), _haveParent(true), _parent(parent), _firstIndex(0), _secondIndex(0)
{
}

Type::Type(Type const & rhs) :
_name(rhs._name), _haveParent(rhs._haveParent), _parent(rhs._parent), _firstIndex(rhs._firstIndex), _secondIndex(rhs._secondIndex)
{
}

bool Type::operator==(std::string const & name) const
{
    if (std::strcmp(this->_name.c_str(), name.c_str()) == 0)
	return (true);
    return (false);
}

bool Type::operator==(std::pair<int, int> const & indexPair) const
{
    return (indexPair.first == this->_firstIndex && indexPair.second == this->_secondIndex);
}

void Type::setParentType(std::list<Type>::iterator parentType)
{
    this->_haveParent = true;
    this->_parent = parentType;
}

void Type::setTypeIndex(int firstIndex, int secondIndex)
{
    this->_firstIndex = firstIndex;
    this->_secondIndex = secondIndex;
}

std::string const & Type::getName() const
{
    return (this->_name);
}

void Type::printIndex() const
{
    std::cout << this->_firstIndex << ", " << this->_secondIndex << ") parent: (";
    if (this->_haveParent)
	(*this->_parent).printIndex();
}

void Type::cleanType()
{
    this->_haveParent = false;
    this->_firstIndex = 0;
    this->_secondIndex = 0;
}
