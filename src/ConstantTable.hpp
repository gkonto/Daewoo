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

using TConstantValueTable = std::vector<TConstantValueElement>;
extern TConstantValueTable constantValueTable;

#endif