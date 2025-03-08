#ifndef VM_HPP_INCLUDED
#define VM_HPP_INCLUDED

#include <memory>
#include "MachineStack.hpp"
#include "ConstantTable.hpp"
#include "TSymbolTable.hpp"
#include "TModule.hpp"

class TModule;

class VM
{
public:
    explicit VM(int stackSize)
        : stack_(stackSize) {}
    void runModule(std::shared_ptr<TModule> module);
    void run(const TProgram &code);

    void store(int symTableIndex);
    void load(int symTableIndex);
    void addOp();
    void subOp();
    void multOp();
    void divOp();
    void unaryMinusOp();
    void powerOp();
    void incOp(int index);

private:
    static void error(const std::string &arg, const TMachineStackRecord &st1, const TMachineStackRecord &st2);
    TSymbolTable &symboltable() { return module_->symboltable(); }

    TMachineStack stack_;
    std::shared_ptr<TModule> module_;
};

#endif