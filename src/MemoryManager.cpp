#include "MemoryManager.hpp"
/* DONE */

TMemoryNode *TMemoryList::addNode(TRhodusObject *node)
{
    ++sizeOfList;
    auto *cursor = newNode(node);
    if (head == nullptr)
    {
        head =
            cursor; // FIXME what happens if i delete a single node TMemoryList ? possibly crash...
        tail = cursor;
    }
    else
    {
        tail->next = cursor;
        tail = cursor;
    }
    return tail;
}

TMemoryList &TMemoryList::getInstance()
{
    static TMemoryList instance;
    return instance;
}

TMemoryNode *TMemoryList::newNode(TRhodusObject *obj) const
{
    auto node = new TMemoryNode();
    node->next = nullptr;
    node->robj = obj;
    return node;
}

void TMemoryList::freeFirstNode(TMemoryNode *cursor)
{
    if (!head)
        return; // Ensure head is not null before proceeding

    cursor = head->next; // Move cursor to the next node

    delete head->robj; // Free the object stored in the head node
    delete head;       // Free the memory allocated for the head node

    head = cursor; // Update head to the new cursor position
}

void TMemoryList::freeLastNode(TMemoryNode *cursor, TMemoryNode *previous)
{
    if (!cursor || !previous)
        return; // Ensure cursor and previous are not null

    tail = previous; // Set tail to previous node
    previous->next =
        nullptr; // Ensure previous node does not point to deleted memory

    delete cursor->robj; // Free the object stored in cursor node
    delete cursor;       // Free the memory allocated for cursor node

    cursor = nullptr; // Set cursor to null
}

void TMemoryList::freeMiddleNode(TMemoryNode *cursor, TMemoryNode *previous)
{
    if (!cursor || !previous)
        return; // Ensure valid pointers

    // Link the previous node to the next node, bypassing the cursor node
    previous->next = cursor->next;

    // Free the object stored in the cursor node
    delete cursor->robj;

    // Delete the cursor node itself
    delete cursor;

    // Move cursor to the next node
    cursor = previous->next;
}

void TMemoryList::freeGarbage()
{
    if (!head)
        return; // If the list is empty, exit

    TMemoryNode *cursor = head;
    TMemoryNode *previous = nullptr;

    while (cursor)
    {
        if (cursor->robj->isGarbage())
        { // Check if the node should be deleted
            --sizeOfList;

            if (cursor == head)
            { // If it's the first node
                freeFirstNode(cursor);
            }
            else
            {
                if (!cursor->next)
                { // If it's the last node
                    freeLastNode(cursor, previous);
                }
                else
                { // If it's a middle node
                    freeMiddleNode(cursor, previous);
                }
            }
        }
        else
        { // Move to the next node
            previous = cursor;
            cursor = cursor->next;
        }
    }
}

void TMemoryList::freeList()
{
    while (head)
    {
        TMemoryNode *cursor = head;
        head = head->next;

        delete cursor->robj; // Free the stored object
        cursor->robj = nullptr;

        delete cursor; // Free the node itself
    }

    head = nullptr;
    tail = nullptr;
    sizeOfList = 0;
}

TMemorySummary TMemoryList::getNumberAllocatedNodes()
{
    TMemorySummary result = {0, 0, 0, 0, 0}; // Initialize all fields to 0

    TMemoryNode *temp = head;
    while (temp)
    {
        ++result.totalAllocated;

        if (temp->robj->isConstant())
            ++result.numConstant;
        if (temp->robj->isGarbage())
            ++result.numGarbage;
        if (temp->robj->isBound())
            ++result.numBound;
        if (temp->robj->isOwned())
            ++result.numOwned;

        temp = temp->next;
    }

    return result;
}

std::string TMemoryList::mapMemory()
{
    std::string result;
    TMemoryNode *cursor = head;
    int count = 0;

    while (cursor)
    {
        if (cursor->robj->isBound())
            result += 'B';
        if (cursor->robj->isOwned())
            result += 'O';
        if (cursor->robj->isGarbage())
            result += 'G';

        cursor = cursor->next;
        ++count;

        if (count % 70 == 0)
        {
            result += '\n'; // Insert a newline after every 70 characters
        }
    }

    return result;
}
