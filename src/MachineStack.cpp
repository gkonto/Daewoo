#include <stdexcept>
#include "MachineStack.hpp"
/* DONE */
std::string TStackRecordTypeToStr(TStackRecordType type) {
    switch (type) {
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

void TMachineStack::checkStackOverflow() {
    if (stackTop_ == stackSize_) {
        throw std::runtime_error("TMachineStack: Stack overflow error");
    }
}

TMachineStackRecord TMachineStack::pop() {
    if (stackTop_ > -1) {
        --stackTop_;
        auto entry = stack_.top();
        stack_.pop();
        return entry;
    } else {
        throw std::runtime_error("TMachineStack: Stack underflow error");
    }
}

// int TMachineStack::popInteger() {
//     auto entry = pop();
//     if (entry.type() != TStackRecordType::stInteger) {
//         throw std::runtime_error("Expecting integer type");
//     }
//     return entry.ivalue();
// }

void TMachineStack::push() {
    ++stackTop_;
    checkStackOverflow();
    stack_.emplace();
}

void TMachineStack::push(int value) {
    ++stackTop_;
    checkStackOverflow();
    stack_.emplace(value);
}

void TMachineStack::push(double value) {
    ++stackTop_;
    checkStackOverflow();
    stack_.emplace(value);
}

void TMachineStack::push(bool value) {
    ++stackTop_;
    checkStackOverflow();
    stack_.emplace(value);
}

void TMachineStack::push(TStringObject *value) {
    ++stackTop_;
    checkStackOverflow();
    stack_.emplace(value);
}

// void TMachineStack::push(const std::vector<TMachineStackRecord> &value) {
//     ++stackTop_;
//     checkStackOverflow();
//     stack_.emplace(value);
// }