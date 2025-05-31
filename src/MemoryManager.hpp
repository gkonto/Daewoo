#ifndef TRHODUS_OBJECT_HPP_INCLUDED
#define TRHODUS_OBJECT_HPP_INCLUDED
/* DONE */

#include <memory>
#include <string>

enum class TBlockType
{
    btGarbage,
    btBound,
    btOwned,
    btConstant
};

class TRhodusObject
{
public:
    TRhodusObject() : type_(TBlockType::btGarbage)
    {
    }
    virtual ~TRhodusObject() = default;
    void setType(TBlockType type)
    {
        type_ = type;
    }
    bool isGarbage() const
    {
        return type_ == TBlockType::btGarbage;
    } // garbage nodes should be deleted..
    bool isBound() const
    {
        return type_ == TBlockType::btBound;
    }
    bool isOwned() const
    {
        return type_ == TBlockType::btOwned;
    }
    bool isConstant() const
    {
        return type_ == TBlockType::btConstant;
    }
    int getRhodusObjectSize() const
    {
        return sizeof(TBlockType);
    }

protected:
    TBlockType type_ = TBlockType::btGarbage;
};

struct TMemoryNode
{
    TRhodusObject *robj = nullptr;
    TMemoryNode *next = nullptr;
};

struct TMemorySummary
{
    int numGarbage = 0;
    int numConstant = 0;
    int numOwned = 0;
    int numBound = 0;
    int totalAllocated = 0;
};

class TMemoryList
{
public:
    static TMemoryList &getInstance();

    TMemoryNode *newNode(TRhodusObject *obj) const;
    TMemoryNode *addNode(TRhodusObject *robj);
    std::string mapMemory();
    int getMemoryListSize() const
    {
        return sizeOfList;
    }
    TMemorySummary getNumberAllocatedNodes();
    void freeGarbage();
    void freeList();

private:
    void freeFirstNode(TMemoryNode *cursor);
    void freeLastNode(TMemoryNode *cursor, TMemoryNode *previous);
    void freeMiddleNode(TMemoryNode *cursor, TMemoryNode *previous);

    int sizeOfList = 0;
    TMemoryNode *head = nullptr;
    TMemoryNode *tail = nullptr;
};

#endif