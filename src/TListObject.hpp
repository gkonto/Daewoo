#ifndef TLISTOBJECT_HPP_INCLUDED
#define TLISTOBJECT_HPP_INCLUDED
/* DONE */
#include <vector>
#include "MemoryManager.hpp"
#include "TListObject.hpp"

class TStringObject;
class TListObject;

enum class TListItemType {
    liInteger,
    liBoolean,
    liDouble,
    liString,
    liList
};

class TListItem {
public:
    explicit TListItem(int v)
        : type_(TListItemType::liInteger), ivalue_(v) {}
    explicit TListItem(bool v)
        : type_(TListItemType::liBoolean), bvalue_(v) {}
    explicit TListItem(double v)
        : type_(TListItemType::liDouble), dvalue_(v) {}
    explicit TListItem(TStringObject *v)
        : type_(TListItemType::liString), svalue_(v) {}
    explicit TListItem(TListObject *v)
        : type_(TListItemType::liList), lvalue_(v) {}

    TListItemType type() const { return type_; }
    const TStringObject *svalue() const { return svalue_; }
    const TListObject *lvalue() const { return lvalue_; }
    int ivalue() const { return ivalue_; }
    bool bvalue() const { return bvalue_; }
    double dvalue() const { return dvalue_; }

    static bool listEquals(const TListItem &item1, const TListItem &item2);

private:
    TListItemType type_;
    int ivalue_ = 0;
    bool bvalue_ = false;
    double dvalue_ = 0.;
    TStringObject *svalue_ = nullptr;
    TListObject *lvalue_ = nullptr;  // TODO is this used ?
};

class TListObject : public TRhodusObject {
public:
    using const_iterator = std::vector<TListItem>::const_iterator;
    void append(int value);
    void append(double value);
    void append(bool value);
    void append(TStringObject *value);
    void insert(const_iterator begin, const_iterator end) { list_.insert(list_.end(), begin, end); }
    TListObject *clone() const;
    const_iterator begin() const { return list_.begin(); }
    const_iterator end() const { return list_.end(); }

    static TListObject *createObject();
    static TListObject *addLists(TListObject *l1, TListObject *l2);
    static TListObject *multiply(int multiplier, const TListObject *list);
    static bool listEquals(const TListObject *l1, const TListObject *l2);

private:
    TListObject() = default;
    std::vector<TListItem> list_;
};

#endif