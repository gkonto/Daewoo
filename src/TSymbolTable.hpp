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

// using TSymbolValue = std::variant<int, double, bool, //TStringObject *, TListObject *, TUserFunction *>;

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
    int index = 0;
    OpCodes opCode = OpCodes::Nop;  // TODO what is the size of this ?
};

using TCode = std::vector<TByteCode>;

/* A program is a collection of bytecodes */
class TProgram {
public:
    TProgram() = default;
    TByteCode &operator[](size_t index) { return code_[index]; }
    const TByteCode &operator[](size_t index) const { return code_[index]; }

    int count() const { return actualLength_; }
    void clear();
    void append(TByteCode bytecode);
    void compactCode() { code_.resize(actualLength_); }
    int addByteCode(OpCodes opCode);
    void addByteCode(OpCodes opCode, int ivalue);
    void addByteCode(OpCodes opCode, double dvalue);
    void addByteCode(OpCodes opCode, bool bvalue);
    void addByteCode(OpCodes opCode, std::string svalue);  // FIXME passed by value

    void appendProgram(TProgram program);  // FIXME passed by value
    int getCurrentInstructionPointer() const { return actualLength_; }
    int createInstructionSpace();
    void setGotoLabel(int location, int value) { code_[location].index = value; }

private:
    void checkSpace();
    TCode code_;
    int actualLength_ = 0;

    static constexpr int ALLOC_BY = 512;
};

class TSymbol {
public:
    friend class TSymbolTable;

    explicit TSymbol(const std::string &name);
    explicit TSymbol(TUserFunction *fvalue);

    TSymbolElementType type() const { return type_; }
    void setType(TSymbolElementType type) { type_ = type; }

    const std::string &name() const { return name_; }

    int ivalue() const { return iValue; }
    void setValue(int val) { iValue = val; }

    double dvalue() const { return dValue; }
    void setValue(double val) { dValue = val; }

    bool bvalue() const { return bValue; }
    void setValue(bool val) { bValue = val; }

    TStringObject *svalue() const { return sValue; }
    void setValue(TStringObject *val) { sValue = val; }

    TListObject *lvalue() const { return lValue; }
    void setValue(TListObject *val) { lValue = val; }

private:
    TSymbolElementType type_ = TSymbolElementType::symUndefined;
    std::string name_ = "";

    int iValue = 0;
    double dValue = 0.;
    bool bValue = false;
    TStringObject *sValue = nullptr;
    TListObject *lValue = nullptr;
    TUserFunction *fValue = nullptr;
};

class TSymbolTable {
public:
    int addSymbol(const std::string &name) { symbols_.emplace_back(name); }
    int addSymbol(TUserFunction *fvalue) { symbols_.emplace_back(fvalue); }
    bool find(const std::string &name, int &index);  // TODO refactor
    bool reverseFind(const std::string &name, int &index);

    void storeSymbolToTable(int index, int ivalue);
    void storeSymbolToTable(int index, bool bvalue);
    void storeSymbolToTable(int index, double dvalue);
    void storeSymbolToTable(int index, TListObject *lvalue);
    void storeSymbolToTable(int index, TStringObject *svalue);  // FIXME possible mem leak...

    const TSymbol &getFromSymbolTable(size_t index) const { return symbols_[index]; }

private:
    void checkForExistingData(int index);
    std::vector<TSymbol> symbols_;
};

class TUserFunction {
public:
    explicit TUserFunction(const std::string &name)
        : name_(name) {}
    const std::string &name() const { return name_; }

private:
    std::string name_;
    int nArgs_;
    TSymbolTable symboltable_;
    TConstantValueTable constanttable_;
    std::vector<std::string> globalVariableList_;
    TProgram funcCode;
};

#endif