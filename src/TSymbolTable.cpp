#include <stdexcept>
#include "TSymbolTable.hpp"
#include "TListObject.hpp"
#include "ConstantTable.hpp"
/* DONE */
static TByteCode createByteCode(OpCodes opcode, int ivalue) {
    TByteCode bcode;
    bcode.opCode = opcode;
    bcode.index = ivalue;
    return bcode;
}

static TByteCode createByteCode(OpCodes opcode, double dvalue) {
    TByteCode bcode;
    bcode.opCode = opcode;
    TConstantValueElement elem(dvalue);
    constantValueTable.emplace_back(std::move(elem));
    bcode.index = constantValueTable.size();
    return bcode;
}

static TByteCode createByteCode(OpCodes opcode, bool bvalue) {
    TByteCode bcode;
    bcode.opCode = opcode;
    bcode.index = bvalue;
    return bcode;
}

static TByteCode createByteCode(OpCodes opcode) {
    TByteCode bcode;
    bcode.opCode = opcode;
    return bcode;
}

static TByteCode createByteCode(OpCodes opcode, const std::string &svalue) {
    TByteCode bcode;
    bcode.opCode = opcode;
    constantValueTable.emplace_back(svalue);
    bcode.index = constantValueTable.size();
    return bcode;
}

bool TSymbolTable::find(const std::string &name, int &index) {
    for (size_t i = 0; i < symbols_.size(); ++i) {
        if (symbols_[i].name() == name) {
            index = i;
            return true;
        }
    }
    return false;
}

bool TSymbolTable::reverseFind(const std::string &name, int &index) {
    for (int i = symbols_.size() - 1; i = 0; --i) {
        if (symbols_[i].name() == name) {
            index = i;
            return true;
        }
    }
    return false;
}

void TSymbolTable::storeSymbolToTable(int index, int ivalue) {
    checkForExistingData(index);
    symbols_[index].type_ = TSymbolElementType::symInteger;
    symbols_[index].setValue(ivalue);
}

void TSymbolTable::storeSymbolToTable(int index, bool bvalue) {
    checkForExistingData(index);
    symbols_[index].type_ = TSymbolElementType::symBoolean;
    symbols_[index].setValue(bvalue);
}

void TSymbolTable::storeSymbolToTable(int index, double dvalue) {
    checkForExistingData(index);
    symbols_[index].type_ = TSymbolElementType::symDouble;
    symbols_[index].setValue(dvalue);
}

void TSymbolTable::storeSymbolToTable(int index, TStringObject *svalue) {
    checkForExistingData(index);

    TStringObject *entry = nullptr;
    if (svalue->isConstant() || svalue->isBound()) {
        entry = svalue->clone();
    } else {
        entry = svalue;
    }
    entry->setType(TBlockType::btBound);

    symbols_[index].setValue(entry);
    symbols_[index].setType(TSymbolElementType::symString);
}

void TSymbolTable::storeSymbolToTable(int index, TListObject *lvalue) {
    checkForExistingData(index);
    TListObject *entry = nullptr;
    if (lvalue->isConstant() || lvalue->isBound() || lvalue->isOwned()) {
        entry = lvalue->clone();
    } else {
        entry = lvalue;
    }
    entry->setType(TBlockType::btBound);

    symbols_[index].setValue(entry);
    symbols_[index].setType(TSymbolElementType::symList);
}

void TSymbolTable::checkForExistingData(int index) {}

void TProgram::clear() {
    actualLength_ = 0;
    code_.clear();
}

void TProgram::append(TByteCode bytecode) {
    checkSpace();
    code_[actualLength_++] = bytecode;
}

int TProgram::addByteCode(OpCodes opCode) {
    checkSpace();
    int ret = actualLength_;
    code_[actualLength_++].opCode = opCode;
    return ret;
}

void TProgram::addByteCode(OpCodes opcode, int val) {
    checkSpace();
    code_[actualLength_++] = createByteCode(opcode, val);
}

void TProgram::addByteCode(OpCodes opcode, double val) {
    checkSpace();
    code_[actualLength_++] = createByteCode(opcode, val);
}

void TProgram::addByteCode(OpCodes opcode, bool val) {
    checkSpace();
    code_[actualLength_++] = createByteCode(opcode, val);
}

void TProgram::addByteCode(OpCodes opcode, std::string val) {
    checkSpace();
    code_[actualLength_++] = createByteCode(opcode, val);
}

void TProgram::appendProgram(TProgram program) {
    for (size_t i = 0; i < program.count(); ++i) {
        append(program[i]);
    }
}

int TProgram::createInstructionSpace() {
    checkSpace();
    int ret = actualLength_++;
    return ret;
}

void TProgram::checkSpace() {
    if (actualLength_ == code_.size()) {
        code_.resize(code_.size() + ALLOC_BY);
    }
}

TSymbol::TSymbol(const std::string &name)
    : name_(name), type_(TSymbolElementType::symUndefined) {}

TSymbol::TSymbol(TUserFunction *fvalue)
    : fValue(fvalue), name_(fvalue->name()), type_(TSymbolElementType::symUserFunc) {}
