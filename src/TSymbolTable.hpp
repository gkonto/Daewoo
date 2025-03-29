#ifndef TSYMBOLTABLE_HPP_INCLUDED
#define TSYMBOLTABLE_HPP_INCLUDED

#include <vector>
#include <cstddef>
#include <variant>

#include "OpCodes.hpp"
#include "ConstantTable.hpp"
#include "TSymbolTable.hpp"
#include "TStringObject.hpp"

/* DONE */
class TListObject;
class TStringObject;
class TSymbol;
class TUserFunction;

// using TSymbolValue = std::variant<int, double, bool, //TStringObject *,
// TListObject *, TUserFunction *>;

enum class TSymbolElementType : std::uint8_t {
    symNonExistant,
    symUndefined,
    symInteger,
    symDouble,
    symBoolean,
    symString,
    symList,
    symUserFunc
};

struct TByteCode {
    int index = -1;
    OpCode opCode = OpCode::Nop;  // TODO what is the size of this ?
};

bool operator==(const TByteCode &lhs, const TByteCode &rhs);
bool operator!=(const TByteCode &lhs, const TByteCode &rhs);

using TCode = std::vector<TByteCode>;

/* A program is a collection of bytecodes */
class TProgram {
public:
    TProgram() = default;
    TByteCode &operator[](size_t index) { return code_[index]; }
    const TByteCode &operator[](size_t index) const { return code_[index]; }
    TByteCode &last() { return code_[actualLength_ - 1]; }

    size_t size() const { return actualLength_; }
    void clear();
    void append(TByteCode bytecode);
    void compactCode() { code_.resize(actualLength_); }
    size_t addByteCode(OpCode opCode);
    void addByteCode(OpCode opCode, int ivalue);
    void addByteCode(OpCode opCode, double dvalue);
    void addByteCode(OpCode opCode, bool bvalue);
    void addByteCode(OpCode opCode, std::string svalue);  // FIXME passed by value

    void appendProgram(TProgram program);  // FIXME passed by value
    size_t getCurrentInstructionPointer() const { return actualLength_; }
    size_t createInstructionSpace();
    void setGotoLabel(int location, int value) { code_[location].index = value; }
    std::string string() const;
    bool operator==(const TProgram &other) const;

private:
    void checkSpace();
    TCode code_;
    size_t actualLength_ = 0;

    static constexpr int ALLOC_BY = 512;
};

class TSymbol {
public:
    friend class TSymbolTable;

    explicit TSymbol(const std::string &name);
    explicit TSymbol(TUserFunction *fvalue);

    TSymbolElementType type() const { return type_; }
    const std::string &name() const { return name_; }
    TStringObject *svalue() const { return sValue_; }
    TListObject *lvalue() const { return lValue_; }
    TUserFunction *fvalue() const { return fValue_; }
    double dvalue() const { return dValue_; }
    bool bvalue() const { return bValue_; }
    int ivalue() const { return iValue_; }

    void setType(TSymbolElementType type) { type_ = type; }
    void setValue(int val) { iValue_ = val; }
    void setValue(double val) { dValue_ = val; }
    void setValue(bool val) { bValue_ = val; }
    void setValue(TStringObject *val) { sValue_ = val; }
    void setValue(TListObject *val) { lValue_ = val; }
    void setName(const std::string &name) { name_ = name; }
    void setValue(TUserFunction *val) { fValue_ = val; }

private:
    TSymbolElementType type_ = TSymbolElementType::symUndefined;
    std::string name_ = "";  // FIXME Do i need this ?

    int iValue_ = 0;
    double dValue_ = 0.;
    bool bValue_ = false;
    TStringObject *sValue_ = nullptr;
    TListObject *lValue_ = nullptr;
    TUserFunction *fValue_ = nullptr;
};

class TSymbolTable {
public:
    int addSymbol(const std::string &name);
    int addSymbol(TUserFunction *fvalue);
    bool find(const std::string &name, int &index);  // TODO refactor
    size_t size() const { return symbols_.size(); }

    void storeSymbolToTable(int index, int ivalue);
    void storeSymbolToTable(int index, bool bvalue);
    void storeSymbolToTable(int index, double dvalue);
    void storeSymbolToTable(int index, TListObject *lvalue);
    void storeSymbolToTable(int index, TStringObject *svalue);  // FIXME possible mem leak...

    const TSymbol &get(size_t index) const { return symbols_[index]; }

private:
    void checkForExistingData(int index);
    std::vector<TSymbol> symbols_;
};

class TUserFunction {
public:
    explicit TUserFunction(const std::string &name)
        : name_(name) {}
    const std::string &name() const { return name_; }
    TProgram &funcCode() { return funcCode_; }
    void setNumberOfArguments(int n) { nArgs_ = n; }
    int numberOfArguments() const { return nArgs_; }  // TODO rename
    TSymbolTable &symboltable() { return symboltable_; }
    TConstantValueTable &constantTable() { return constantTable_; }

private:
    std::string name_;
    int nArgs_;
    TSymbolTable symboltable_;
    TConstantValueTable constantTable_;  // FIXME is this a global reference ?
    std::vector<std::string> globalVariableList_;
    TProgram funcCode_;
};

#endif