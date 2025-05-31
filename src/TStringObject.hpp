#ifndef TSTRINGOBJECT_HPP_INCLUDED
#define TSTRINGOBJECT_HPP_INCLUDED
/* DONE */

#include "MemoryManager.hpp"
#include <string>

class TStringObject : public TRhodusObject
{
public:
    template <typename T>
    explicit TStringObject(T &&value) : value_(std::forward<T>(value))
    {
    }

    template <typename T>
    static TStringObject *createConstantObject(T &&value)
    {
        auto *ret = new TStringObject(std::forward<T>(value));
        ret->setType(TBlockType::btConstant);
        return ret;
    }

    template <typename T>
    static TStringObject *createStringObject(T &&value)
    {
        auto *ret = new TStringObject(std::forward<T>(value));
        TMemoryList::getInstance().addNode(ret);
        return ret;
    }

    bool isEqualTo(const TStringObject &other) const
    {
        return !value_.compare(other.value_);
    }

    // FIXME whoever calls this function, must free the returned object.
    static TStringObject *add(const TStringObject &first,
                              const TStringObject &second)
    {
        return createStringObject(first.value_ + second.value_);
    }

    TStringObject *clone() const
    {
        return createStringObject(value_);
    }

    const std::string &value() const
    {
        return value_;
    }

private:
    std::string value_;
};

#endif