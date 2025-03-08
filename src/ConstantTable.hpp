#ifndef CONSTANT_TABLE_HPP_INCLUDED
#define CONSTANT_TABLE_HPP_INCLUDED

#include <vector>

class TConstantValueElement
{
public:
    // explicit TConstantValueElement(double value)
    //     : dValue_(value) {}

    // explicit TConstantValueElement(const std::string &value)
    //     : sValue_(value) {}

private:
    // TConstantValueType valueType_;
    // double dValue_;
    // TStringObject sValue_;
    // TListObject lList_;
};

using TConstantValueTable = std::vector<TConstantValueElement>;
extern TConstantValueTable constantValueTable;

#endif