#ifndef CONSTANT_TABLE_HPP_INCLUDED
#define CONSTANT_TABLE_HPP_INCLUDED
/* DONE */

#include <vector>
#include <string>
#include <memory>

#include "TStringObject.hpp"

enum class TConstantValueType {
    None,
    Double,
    String
};

class TConstantValueElement {
public:
    explicit TConstantValueElement(double v)
        : valueType_(TConstantValueType::Double), dvalue_(v) {}
    explicit TConstantValueElement(const std::string &v)
        : valueType_(TConstantValueType::String), svalue_(TStringObject::createConstantObject(v)) {}
    explicit TConstantValueElement()
        : valueType_(TConstantValueType::None) {}
    double dvalue() const { return dvalue_; }
    const std::string &svalue() const { return svalue_->value(); }

private:
    TConstantValueType valueType_;
    TStringObject *svalue_;  // FIXME mutually exclusive wth dvalue_;
    double dvalue_;
};

class TConstantValueTable {
public:
    TConstantValueElement &get(int index);
    void clear() { table_.clear(); }
    template <typename T>
    void emplace_back(T &&entry) {
        table_.emplace_back(std::forward<T>(entry));
    }

    size_t size() const { return table_.size(); }

private:
    std::vector<TConstantValueElement> table_;
};

extern TConstantValueTable constantValueTable;

#endif