#ifndef TMODULE_HPP_INCLUDED
#define TMODULE_HPP_INCLUDED
#include <string>

#include "TSymbolTable.hpp"

class TModule
{
public:
    explicit TModule(const std::string &name)
        : name_(name) {}

    const TProgram &code() const { return code_; }
    TSymbolTable &symboltable() { return symboltable_; }

private:
    std::string name_;
    TProgram code_;
    TSymbolTable symboltable_;
};
#endif