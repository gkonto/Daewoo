#ifndef TMODULE_HPP_INCLUDED
#define TMODULE_HPP_INCLUDED
#include <string>

#include "TSymbolTable.hpp"

/* A module is a pair of bytecode associated with a symboltable */
class TModule {
public:
    TModule() = default;
    explicit TModule(const std::string &name)
        : name_(name) {}

    const TProgram &code() const { return code_; }
    TProgram &code() { return code_; }
    TSymbolTable &symboltable() { return symboltable_; }

private:
    std::string name_ = "";
    TProgram code_;
    TSymbolTable symboltable_;
};
#endif