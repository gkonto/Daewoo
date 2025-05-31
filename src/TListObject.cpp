#include "TListObject.hpp"
#include "MemoryManager.hpp"
#include "TStringObject.hpp"
/* DONE */
TListObject *TListObject::clone() const
{
    auto *ret = new TListObject;
    for (const auto &entry : *this)
    {
        switch (entry.type())
        {
        case TListItemType::liBoolean:
        case TListItemType::liInteger:
        case TListItemType::liDouble:
            ret->list_.emplace_back(entry);
            break;
        case TListItemType::liString:
        {
            auto *obj = entry.svalue()->clone();
            obj->setType(TBlockType::btOwned);
            ret->list_.emplace_back(obj);
            break;
        }
        case TListItemType::liList:
        {
            auto *obj = entry.lvalue()->clone();
            obj->setType(TBlockType::btOwned);
            ret->list_.emplace_back(obj);
            break;
        }
        }
    }
    return ret;
}

TListObject *TListObject::addLists(TListObject *l1, TListObject *l2)
{
    TListObject *ret = nullptr;
    if (l1->isGarbage())
    {
        ret = l1;
    }
    else
    {
        ret = l1->clone();
    }

    ret->list_.insert(ret->list_.end(), l2->list_.begin(), l2->list_.end());
    return ret;
}

TListObject *TListObject::multiply(int multiplier, const TListObject *aList)
{
    const auto *workingCopy = aList->isBound() ? aList->clone() : aList;

    size_t nContents = aList->list_.size();
    TListObject *result = new TListObject();

    for (int i = 0; i < multiplier; ++i)
    {
        if (nContents == 0)
        {
            result->list_.push_back(TListItem(0));
        }
        else
        {
            for (size_t j = 0; j < nContents; ++j)
            {
                result->list_.push_back(TListItem(workingCopy->list_[j]));
            }
        }
    }

    if (aList->isBound())
        delete workingCopy; // Clean up cloned copy if needed

    return result;
}

bool TListObject::listEquals(const TListObject *l1, const TListObject *l2)
{
    if (l1->list_.size() != l2->list_.size())
        return false;

    for (size_t i = 0; i < l1->list_.size(); ++i)
    {
        if (!TListItem::listEquals(l1->list_[i], l2->list_[i]))
            return false;
    }
    return true;
}

void TListObject::append(int value)
{
    list_.emplace_back(value);
}

void TListObject::append(bool value)
{
    list_.emplace_back(value);
}

void TListObject::append(double value)
{
    list_.emplace_back(value);
}

void TListObject::append(TStringObject *value)
{
    list_.emplace_back(value);
}

TListObject *TListObject::createObject()
{
    auto *obj = new TListObject();
    TMemoryList::getInstance().addNode(obj);
    return obj;
}

bool TListItem::listEquals(const TListItem &item1, const TListItem &item2)
{
    if (item1.type() == TListItemType::liInteger &&
        item2.type() == TListItemType::liInteger)
    {
        return item1.ivalue() == item2.ivalue();
    }

    if (item1.type() == TListItemType::liBoolean &&
        item2.type() == TListItemType::liBoolean)
    {
        return item1.bvalue() == item2.bvalue();
    }

    if (item1.type() == TListItemType::liDouble &&
        item2.type() == TListItemType::liDouble)
    {
        return item1.dvalue() == item2.dvalue();
    }

    if (item1.type() == TListItemType::liString &&
        item2.type() == TListItemType::liString)
    {
        return item1.svalue() == item2.svalue();
    }

    if (item1.type() == TListItemType::liList &&
        item2.type() == TListItemType::liList)
    {
        return item1.lvalue() == item2.lvalue();
    }

    return false;
}
