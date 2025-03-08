#ifndef MACHINESTACK_HPP_INCLUDED
#define MACHINESTACK_HPP_INCLUDED

#include <variant>
#include <memory>
#include <stack>

// Define stack types
enum class TStackType
{
    stNone,
    stInteger,
    stDouble,
    stBoolean,
    stString
};

// Define the structure using std::variant for type safety
class TMachineStackRecord
{
public:
    using TStackRecordValue = std::variant<std::monostate, int, bool, double>;

    TMachineStackRecord() : stackType_(TStackType::stNone), value_(std::monostate{}) {}
    TMachineStackRecord(int i) : stackType_(TStackType::stInteger), value_(i) {}
    TMachineStackRecord(bool b) : stackType_(TStackType::stBoolean), value_(b) {}
    TMachineStackRecord(double d) : stackType_(TStackType::stDouble), value_(d) {}

    const TStackRecordValue &value() const { return value_; }
    TStackType type() const { return stackType_; }

private:
    TStackType stackType_;
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

#endif