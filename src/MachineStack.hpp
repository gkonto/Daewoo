#ifndef MACHINESTACK_HPP_INCLUDED
#define MACHINESTACK_HPP_INCLUDED

/* DONE */
#include <variant>
#include <memory>
#include <stack>
#include <vector>
#include <string>

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
    using TStackRecordValue =
        std::variant<std::monostate, int, bool, double, TStringObject *, TListObject *>;

    explicit TMachineStackRecord()
        : stackType_(TStackRecordType::stNone), value_(std::monostate{}) {}
    explicit TMachineStackRecord(int i)
        : stackType_(TStackRecordType::stInteger), value_(i) {}
    explicit TMachineStackRecord(bool b)
        : stackType_(TStackRecordType::stBoolean), value_(b) {}
    explicit TMachineStackRecord(double d)
        : stackType_(TStackRecordType::stDouble), value_(d) {}
    explicit TMachineStackRecord(TListObject *lobj)
        : stackType_(TStackRecordType::stList), value_(lobj) {}
    explicit TMachineStackRecord(TStringObject *sobj)
        : stackType_(TStackRecordType::stString), value_(sobj) {}

    const TStackRecordValue &value() const { return value_; }
    int ivalue() const { return std::get<int>(value_); }
    bool bvalue() const { return std::get<bool>(value_); }
    void setValue(bool val) { value_ = val; }
    double dvalue() const { return std::get<double>(value_); }
    TStringObject *svalue() const { return std::get<TStringObject *>(value_); }
    TListObject *lvalue() const { return std::get<TListObject *>(value_); }

    TStackRecordType type() const { return stackType_; }

private:
    TStackRecordType stackType_;
    TStackRecordValue value_;
};

class TMachineStack {
public:
    explicit TMachineStack(int stackSize)
        : stackSize_(stackSize) {}
    void checkStackOverflow();
    const TMachineStackRecord &top() const { return stack_.top(); }

    TMachineStackRecord pop();
    // int popInteger();

    // void push(TMachineStackRecord value);
    void push();
    void push(int value);
    void push(bool value);
    void push(double value);
    void push(TStringObject *value);
    // void push(const std::vector<TMachineStackRecord> &value);

private:
    // maybe struct ?
    int stackTop_ = -1;  // is this needed?
    int stackSize_;
    std::stack<TMachineStackRecord> stack_;
};

std::string TStackRecordTypeToStr(TStackRecordType type);

#endif