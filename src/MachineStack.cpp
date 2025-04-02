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

const TMachineStackRecord &TMachineStack::pop() {
    return stack_[stackTop_--];
}

int TMachineStack::popInteger() {
    return pop().ivalue();
}

void TMachineStack::decreaseBy(int value) {
    stackTop_ -= value;
}

void TMachineStack::increaseBy(int value) {
    stackTop_ += value;
}

void TMachineStack::push() {
    ++stackTop_;
}

void TMachineStack::push(int value) {
    stack_[++stackTop_].setValue(value);
}

void TMachineStack::push(double value) {
    stack_[++stackTop_].setValue(value);
}

void TMachineStack::push(bool value) {
    stack_[++stackTop_].setValue(value);
}

void TMachineStack::push(TStringObject *value) {
    stack_[++stackTop_].setValue(value);
}

void TMachineStack::push(TMachineStackRecord value) {
    ++stackTop_;
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
