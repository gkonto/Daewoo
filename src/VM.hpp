#ifndef VM_HPP_INCLUDED
#define VM_HPP_INCLUDED

#include <memory>
#include "MachineStack.hpp"
#include "ConstantTable.hpp"
#include "TSymbolTable.hpp"
#include "TModule.hpp"

class TModule;

struct TFrame {
    int funcIndex = -1;
    int bsp = -1;     // stack base of function arguments
    int nArgs = 0;    // number of arguments
    int nlocals = 0;  // number of local variables
    // This is a reference to the global constant table
    TConstantValueTable constantTable;
    // This is a reference to the local symbol table
    TSymbolTable symbolTable;
};

class TFrameStack {
public:
    int topIndex() const { return topIndex_; }
    TFrame &top();
    void increase() { ++topIndex_; }
    void decrease() { --topIndex_; }

private:
    std::array<TFrame, 2048> frameStack_;
    int topIndex_ = -1;
};

class VM {
public:
    void runModule(std::shared_ptr<TModule> module);
    void run(const TProgram &code);
    const TMachineStackRecord &top() const { return stack_.ctop(); }
    bool empty() const { return stack_.empty(); }

private:
    void store(int symTableIndex);
    // void load(int symTableIndex);
    void addOp();
    void subOp();
    void multOp();
    void divOp();
    void unaryMinusOp();
    void powerOp();
    // void incOp(int index);
    // void decOp(int index);
    void andOp();
    void orOp();
    void notOp();
    // void xorOp();
    void isLt();
    void isLte();
    void isGt();
    void isGte();
    // void modOp();
    void isEq();
    void isNotEq();
    void loadSymbol(int index);

    void push() { stack_.push(); };
    void push(int value) { stack_.push(value); }
    void push(bool value) { stack_.push(value); }
    void push(double value) { stack_.push(value); }
    void push(TMachineStackRecord value) { stack_.push(value); }
    TMachineStackRecord pop() { return stack_.pop(); }
    void callUserFunction();
    void returnOp();
    void storeLocalSymbol(int index);
    void loadLocalSymbol(int index);
    void copyToStack(TMachineStackRecord &stackelem, TFrame &frame);

    //  void push(const std::string &value) { stack_.push(value); }

    static void
    error(const std::string &arg, const TMachineStackRecord &st1, const TMachineStackRecord &st2);
    TSymbolTable &symboltable() { return module_->symboltable(); }

    TMachineStack stack_;
    TFrameStack frameStack_;
    std::shared_ptr<TModule> module_;
};

#endif