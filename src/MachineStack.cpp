#include <stdexcept>
#include "MachineStack.hpp"

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
    if (stackTop_ == stackSize_)
    {
        throw std::runtime_error("TMachineStack: Stack overflow error");
    }
}

TMachineStackRecord TMachineStack::pop()
{
    if (stackTop_ > -1)
    {
        --stackTop_;
        auto entry = stack_.top();
        stack_.pop();
        return entry;
    }
    else
    {
        throw std::runtime_error("TMachineStack: Stack underflow error");
    }
}
