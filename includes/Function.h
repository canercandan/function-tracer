#ifndef FUNCTION_H_
#define FUNCTION_H_

#include <list>
#include <ostream>

#include "Argument.h"

class Function
{
public:
    Function(size_t address, std::string const & functionName = std::string("None"));

    bool operator==(Function const & rhs) const;
    bool operator==(std::string const & name) const;
    bool operator<(Function const & rhs) const;

    std::string const & getName() const;
    size_t getAddress() const;
    void setReturnType(std::list<Type>::iterator returnType);
    std::list<Type>::iterator const & getReturnType() const;
    std::string getReturnTypeAsString() const;
    std::list<Argument>& getArguments();
    std::string getArgumentsAsString() const;

private:
    size_t _address;
    std::list<Type>::iterator _returnType;
    std::string _name;
    std::list<Argument> _arguments;
    bool _hasReturnType;
};

std::ostream & operator<<(std::ostream & os, Function const & function);

#endif // FUNCTION_H_
