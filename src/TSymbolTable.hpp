#ifndef TSYMBOLTABLE_HPP_INCLUDED
#define TSYMBOLTABLE_HPP_INCLUDED

#include <vector>
#include <cstddef>

#include "OpCodes.hpp"

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

class TSymbolTable
{
public:
private:
};

#endif