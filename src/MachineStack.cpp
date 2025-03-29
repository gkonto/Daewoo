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
    if (stackTop_ == static_cast<int>(stack_.size())) {
        throw std::runtime_error("TMachineStack: Stack overflow error");
    }
}

TMachineStackRecord TMachineStack::pop() {
    if (stackTop_ > -1) {
        auto entry = stack_[stackTop_];
        --stackTop_;
        return entry;
    } else {
        throw std::runtime_error("TMachineStack: Stack underflow error");
    }
}

int TMachineStack::popInteger() {
    auto entry = pop();
    if (entry.type() != TStackRecordType::stInteger) {
        throw std::runtime_error("Expecting integer type");
    }
    return entry.ivalue();
}

void TMachineStack::decreaseBy(int value) {
    stackTop_ -= value;
}

void TMachineStack::increaseBy(int value) {
    stackTop_ += value;
    checkStackOverflow();
    for (int i = 0; i < value; ++i) {
        stack_[stackTop_ - i].setType(TStackRecordType::stNone);
    }
}

void TMachineStack::push() {
    ++stackTop_;
    checkStackOverflow();
}

void TMachineStack::push(int value) {
    ++stackTop_;
    checkStackOverflow();
    stack_[stackTop_].setValue(value);
}

void TMachineStack::push(double value) {
    ++stackTop_;
    checkStackOverflow();
    stack_[stackTop_].setValue(value);
}

void TMachineStack::push(bool value) {
    ++stackTop_;
    checkStackOverflow();
    stack_[stackTop_].setValue(value);
}

void TMachineStack::push(TStringObject *value) {
    ++stackTop_;
    checkStackOverflow();
    stack_[stackTop_].setValue(value);
}

void TMachineStack::push(TMachineStackRecord value) {
    ++stackTop_;
    checkStackOverflow();
    stack_[stackTop_].setType(value.type());
    if (value.type() == TStackRecordType::stInteger) {
        stack_[stackTop_].setValue(value.ivalue());
    } else if (value.type() == TStackRecordType::stBoolean) {
        stack_[stackTop_].setValue(value.bvalue());
    } else if (value.type() == TStackRecordType::stDouble) {
        stack_[stackTop_].setValue(value.dvalue());
    } else if (value.type() == TStackRecordType::stNone) {
        ;
    } else if (value.type() == TStackRecordType::stString) {
        throw std::runtime_error("TMachineStack::push> Unknown type in push method");
    } else if (value.type() == TStackRecordType::stList) {
        throw std::runtime_error("TMachineStack::push> Unknown type in push method");
    }
}

// void TMachineStack::push(const std::vector<TMachineStackRecord> &value) {
//     ++stackTop_;
//     checkStackOverflow();
//     stack_.emplace(value);
// }