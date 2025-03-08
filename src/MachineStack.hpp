#ifndef MACHINESTACK_HPP_INCLUDED
#define MACHINESTACK_HPP_INCLUDED

#include <variant>
#include <memory>
#include <stack>

// Define stack types
enum class TStackRecordType
{
    stNone,
    stInteger,
    stDouble,
    stBoolean,
    stString,
    stList
};

// Define the structure using std::variant for type safety
class TMachineStackRecord
{
public:
    using TStackRecordValue = std::variant<std::monostate, int, bool, double, std::string>;

    TMachineStackRecord() : stackType_(TStackRecordType::stNone), value_(std::monostate{}) {}
    TMachineStackRecord(int i) : stackType_(TStackRecordType::stInteger), value_(i) {}
    TMachineStackRecord(bool b) : stackType_(TStackRecordType::stBoolean), value_(b) {}
    TMachineStackRecord(double d) : stackType_(TStackRecordType::stDouble), value_(d) {}

    const TStackRecordValue &value() const { return value_; }
    int ivalue() const { return std::get<int>(value_); }
    bool bvalue() const { return std::get<bool>(value_); }
    double dvalue() const { return std::get<double>(value_); }
    std::string svalue() const { return std::get<std::string>(value_); }

    TStackRecordType type() const { return stackType_; }

private:
    TStackRecordType stackType_;
    TStackRecordValue value_;
};

class TMachineStack
{
public:
    explicit TMachineStack(int stackSize)
        : stackSize_(stackSize) {}
    void checkStackOverflow();

    TMachineStackRecord pop();
    template <typename T>
    void push(T &&value)
    {
        ++stackTop_;
        checkStackOverflow();
        stack_.emplace(std::forward<T>(value));
    }

private:
    // maybe struct ?
    int stackTop_ = -1; // is this needed?
    int stackSize_;
    std::stack<TMachineStackRecord> stack_;
};

std::string TStackRecordTypeToStr(TStackRecordType type);

#endif