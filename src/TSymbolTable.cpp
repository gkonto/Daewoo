#include <stdexcept>
#include "TSymbolTable.hpp"

void TSymbolTable::store(int index, int ivalue)
{
    checkForExistingData(index);
    symbols_[index].type_ = TSymbolElementType::Integer;
    symbols_[index].value_ = ivalue;
}

void TSymbolTable::store(int index, bool bvalue)
{
    checkForExistingData(index);
    symbols_[index].type_ = TSymbolElementType::Boolean;
    symbols_[index].value_ = bvalue;
}

void TSymbolTable::store(int index, double dvalue)
{
    checkForExistingData(index);
    symbols_[index].type_ = TSymbolElementType::Double;
    symbols_[index].value_ = dvalue;
}

void TSymbolTable::checkForExistingData(int index)
{
    throw std::runtime_error("IMPLEMENTATION MISSING");
}