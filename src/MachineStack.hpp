#ifndef MACHINESTACK_HPP_INCLUDED
#define MACHINESTACK_HPP_INCLUDED

/* DONE */
#include <variant>
#include <memory>
#include <stack>
#include <vector>
#include <string>
#include <array>

class TStringObject;
class TListObject;

// Define stack types
enum class TStackRecordType {
    stNone,
    stInteger,
    stDouble,
    stBoolean,
    stString,
    stList,
};

// Define the structure using std::variant for type safety
class TMachineStackRecord {
public:
    TMachineStackRecord()
        : type_(TStackRecordType::stNone) {}

    int ivalue() const { return v.i; }
    bool bvalue() const { return v.b;}
    double dvalue() const { return v.d; }
    TStringObject *svalue() const { return v.s; }
    TListObject *lvalue() const { return v.l; }
    TStackRecordType type() const { return type_; }

    // TODO template
    void setValue(int val) {
        v.i = val;
        type_ = TStackRecordType::stInteger;
    }
    void setValue(bool val) {
        v.b = val;
        type_ = TStackRecordType::stBoolean;
    }
    void setValue(double val) {
        v.d = val;
        type_ = TStackRecordType::stDouble;
    }
    void setValue(TListObject *val) {
        v.l = val;
        type_ = TStackRecordType::stList;
    }
    void setValue(TStringObject *val) {
        v.s = val;
        type_ = TStackRecordType::stString;
    }
    void setType(TStackRecordType type) { type_ = type; }

private:
    TStackRecordType type_;
    union value {
        TListObject *l;
        TStringObject *s;
        double d;
        int i;
        bool b;
    }v;
};

class TMachineStack {
public:
    void checkStackOverflow();
    const TMachineStackRecord &ctop() const { return stack_[stackTop_]; }
    bool empty() const { return stack_.empty(); }
    TMachineStackRecord &top() { return stack_[stackTop_]; }
    int topIndex() const { return stackTop_; }
    const TMachineStackRecord &pop() { return stack_[stackTop_--]; }
    int popInteger() { return pop().ivalue(); }
    TMachineStackRecord &operator[](int index) { return stack_[index]; }

    void increaseBy(int val) { stackTop_ += val; }
    void decreaseBy(int val) { stackTop_ -= val; }

    void push() { ++stackTop_; }
    // TODO template ?
    void push(int value) { stack_[++stackTop_].setValue(value); }
    void push(bool value) { stack_[++stackTop_].setValue(value); }
    void push(double value) { stack_[++stackTop_].setValue(value); }
    void push(TStringObject *value) { stack_[++stackTop_].setValue(value); }
    void push(TMachineStackRecord value);

private:
    // maybe struct ?
    int stackTop_ = -1;  // is this needed?
    std::array<TMachineStackRecord, 4000> stack_;
};

std::string TStackRecordTypeToStr(TStackRecordType type);

#endif