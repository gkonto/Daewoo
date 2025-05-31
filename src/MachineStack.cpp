#include "MachineStack.hpp"
#include <stdexcept>
/* DONE */
std::string TStackRecordTypeToStr(TStackRecordType type)
{
    switch (type)
    {
    case TStackRecordType::stNone:
        return "Nil";
    case TStackRecordType::stInteger:
        return "INT";
    case TStackRecordType::stDouble:
        return "DOUBLE";
    case TStackRecordType::stBoolean:
        return "BOOLEAN";
    case TStackRecordType::stString:
        return "STRING";
    case TStackRecordType::stList:
        return "LIST";
    }
    return "";
}

void TMachineStack::checkStackOverflow()
{
    if (stackTop_ == static_cast<int>(stack_.size()))
    {
        throw std::runtime_error("TMachineStack: Stack overflow error");
    }
}

void TMachineStack::push(TMachineStackRecord value)
{
    ++stackTop_;
    stack_[stackTop_].setType(value.type());
    if (value.type() == TStackRecordType::stInteger)
    {
        stack_[stackTop_].setValue(value.ivalue());
    }
    else if (value.type() == TStackRecordType::stBoolean)
    {
        stack_[stackTop_].setValue(value.bvalue());
    }
    else if (value.type() == TStackRecordType::stDouble)
    {
        stack_[stackTop_].setValue(value.dvalue());
    }
    else if (value.type() == TStackRecordType::stNone)
    {
        ;
    }
    else if (value.type() == TStackRecordType::stString)
    {
        throw std::runtime_error(
            "TMachineStack::push> Unknown type in push method");
    }
    else if (value.type() == TStackRecordType::stList)
    {
        throw std::runtime_error(
            "TMachineStack::push> Unknown type in push method");
    }
}
