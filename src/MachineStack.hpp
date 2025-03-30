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
        : type_(TStackRecordType::stNone), value_(std::monostate{}) {}
    explicit TMachineStackRecord(int i)
        : type_(TStackRecordType::stInteger), value_(i) {}
    explicit TMachineStackRecord(bool b)
        : type_(TStackRecordType::stBoolean), value_(b) {}
    explicit TMachineStackRecord(double d)
        : type_(TStackRecordType::stDouble), value_(d) {}
    explicit TMachineStackRecord(TListObject *lobj)
        : type_(TStackRecordType::stList), value_(lobj) {}
    explicit TMachineStackRecord(TStringObject *sobj)
        : type_(TStackRecordType::stString), value_(sobj) {}

    const TStackRecordValue &value() const { return value_; }
    int ivalue() const { return std::get<int>(value_); }
    bool bvalue() const { return std::get<bool>(value_); }
    double dvalue() const { return std::get<double>(value_); }
    TStringObject *svalue() const { return std::get<TStringObject *>(value_); }
    TListObject *lvalue() const { return std::get<TListObject *>(value_); }
    TStackRecordType type() const { return type_; }

    // TODO template
    void setValue(int val) {
        value_ = val;
        type_ = TStackRecordType::stInteger;
    }
    void setValue(bool val) {
        value_ = val;
        type_ = TStackRecordType::stBoolean;
    }
    void setValue(double val) {
        value_ = val;
        type_ = TStackRecordType::stDouble;
    }
    void setValue(TListObject *val) {
        value_ = val;
        type_ = TStackRecordType::stList;
    }
    void setValue(TStringObject *val) {
        value_ = val;
        type_ = TStackRecordType::stString;
    }
    void setType(TStackRecordType type) { type_ = type; }

private:
    TStackRecordType type_;
    TStackRecordValue value_;
};

class TMachineStack {
public:
    void checkStackOverflow();
    const TMachineStackRecord &ctop() const { return stack_[stackTop_]; }
    bool empty() const { return stack_.empty(); }
    TMachineStackRecord &top() { return stack_[stackTop_]; }
    int topIndex() const { return stackTop_; }
    TMachineStackRecord pop();
    int popInteger();
    TMachineStackRecord &operator[](int index) { return stack_[index]; }

    void increaseBy(int val);
    void decreaseBy(int val);

    void push();
    // TODO template ?
    void push(int value);
    void push(bool value);
    void push(double value);
    void push(TStringObject *value);
    void push(TMachineStackRecord value);
    // void push(const std::vector<TMachineStackRecord> &value);

private:
    // maybe struct ?
    int stackTop_ = -1;  // is this needed?
    std::array<TMachineStackRecord, 4000> stack_;
};

std::string TStackRecordTypeToStr(TStackRecordType type);

#endif