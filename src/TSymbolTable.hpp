#ifndef TSYMBOLTABLE_HPP_INCLUDED
#define TSYMBOLTABLE_HPP_INCLUDED

#include <vector>
#include <cstddef>
#include <variant>

#include "OpCodes.hpp"

using TSymbolValue = std::variant<int, double, bool, std::string>;

enum class TSymbolElementType
{
    NonExistant,
    Undefined,
    Integer,
    Double,
    Boolean,
    String,
    List,
    UserFunc
};

struct TByteCode
{
    int index;
    OpCodes opCode; // TODO what is the size of this ?
};

class TProgram
{
public:
    TByteCode &operator[](size_t index) { return bcode_[index]; }
    const TByteCode &operator[](size_t index) const { return bcode_[index]; }

private:
    std::vector<TByteCode> bcode_;
};

class TSymbol
{
public:
    friend class TSymbolTable;

    TSymbolElementType type() const { return type_; }
    void setType(TSymbolElementType type) { type_ = type; }

    const std::string &symbolName() const { return name_; }
    int ivalue() const { return std::get<int>(value_); }
    double dvalue() const { return std::get<double>(value_); }

private:
    TSymbolElementType type_;
    std::string name_;
    TSymbolValue value_;
};

class TSymbolTable
{
public:
    const TSymbol &get(int index) const { return symbols_[index]; };
    void store(int index, int ivalue);
    void store(int index, bool bvalue);
    void store(int index, double dvalue);

private:
    void checkForExistingData(int index);
    std::vector<TSymbol> symbols_;
};

#endif