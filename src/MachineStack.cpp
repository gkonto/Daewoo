#include <stdexcept>
#include "MachineStack.hpp"

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
