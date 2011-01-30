#ifndef TYPE_H_

#define TYPE_H_

#include <string>
#include <list>
#include <utility>
#include <iostream>

class Type
{
public:
    Type(std::string const & name);
    Type(std::string const & name, std::list<Type>::iterator parent);
    Type(Type const & rhs);

    bool operator==(std::string const & name) const;
    bool operator==(std::pair<int, int> const & index) const;

    void setParentType(std::list<Type>::iterator);
    void setTypeIndex(int firstIndex, int secondIndex);

    std::string const & getName() const;
    void printIndex() const;

    void cleanType();

private:
    std::string _name;
    bool _haveParent;
    std::list<Type>::iterator _parent;
    int _firstIndex;
    int _secondIndex;
};

#endif //! TYPE_H_
