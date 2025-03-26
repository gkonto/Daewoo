/* DONE */

#include "ConstantTable.hpp"

TConstantValueTable constantValueTable;

TConstantValueElement &TConstantValueTable::get(int index) {
    return table_[index - 1];
}
