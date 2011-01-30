#ifndef ARGUMENT_H_
#define ARGUMENT_H_

#include <string>
#include <list>

#include "Type.h"

class Argument
{
public:
    Argument(std::string const & name);
    Argument(std::string const & name, std::list<Type>::iterator type);
    Argument(Argument const & rhs);
    Argument & operator=(Argument const & rhs);

    void setType(std::list<Type>::iterator);
    std::list<Type>::iterator const & getType() const;
    std::string const & getName() const;

private:
    std::string _name;
    std::list<Type>::iterator _type;
};

#endif // ARGUMENT_H_
