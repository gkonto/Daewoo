#include <stdexcept>
#include <assert.h>
#include "TSymbolTable.hpp"
#include "TListObject.hpp"
#include "ConstantTable.hpp"
/* DONE */
static TByteCode createByteCode(OpCode opcode, int ivalue) {
    TByteCode bcode;
    bcode.opCode = opcode;
    bcode.index = static_cast<int>(ivalue);
    return bcode;
}

static TByteCode createByteCode(OpCode opcode, double dvalue) {
    TByteCode bcode;
    bcode.opCode = opcode;
    TConstantValueElement elem(dvalue);
    constantValueTable.emplace_back(std::move(elem));
    bcode.index = static_cast<int>(constantValueTable.size());
    return bcode;
}

static TByteCode createByteCode(OpCode opcode, bool bvalue) {
    TByteCode bcode;
    bcode.opCode = opcode;
    bcode.index = bvalue;
    return bcode;
}

static TByteCode createByteCode(OpCode opcode) {
    TByteCode bcode;
    bcode.opCode = opcode;
    return bcode;
}

static TByteCode createByteCode(OpCode opcode, const std::string &svalue) {
    TByteCode bcode;
    bcode.opCode = opcode;
    constantValueTable.emplace_back(svalue);
    bcode.index = static_cast<int>(constantValueTable.size());
    return bcode;
}

bool TSymbolTable::find(const std::string &name, int &index) {
    for (size_t i = 0; i < symbols_.size(); ++i) {
        if (symbols_[i].name() == name) {
            index = static_cast<int>(i);
            return true;
        }
    }
    return false;
}

int TSymbolTable::addSymbol(const std::string &name) {
    symbols_.emplace_back(name);
    return static_cast<int>(symbols_.size() - 1);
}

int TSymbolTable::addSymbol(TUserFunction *fvalue) {
    symbols_.emplace_back(fvalue);
    symbols_.back().setName(fvalue->name());
    symbols_.back().setType(TSymbolElementType::symUserFunc);
    return static_cast<int>(symbols_.size() - 1);
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

void TSymbolTable::checkForExistingData(int index) {
    if (index >= symbols_.size()) {
        throw std::runtime_error("TSymbolTable> Symbols buffer overflow");
    }
}

void TProgram::clear() {
    actualLength_ = 0;
    code_.clear();
}

void TProgram::append(TByteCode bytecode) {
    checkSpace();
    code_[actualLength_++] = bytecode;
}

size_t TProgram::addByteCode(OpCode opCode) {
    checkSpace();
    size_t ret = actualLength_;
    code_[actualLength_++].opCode = opCode;
    return ret;
}

void TProgram::addByteCode(OpCode opcode, int val) {
    checkSpace();
    code_[actualLength_++] = createByteCode(opcode, val);
}

void TProgram::addByteCode(OpCode opcode, double val) {
    checkSpace();
    code_[actualLength_++] = createByteCode(opcode, val);
}

void TProgram::addByteCode(OpCode opcode, bool val) {
    checkSpace();
    code_[actualLength_++] = createByteCode(opcode, val);
}

void TProgram::addByteCode(OpCode opcode, std::string val) {
    checkSpace();
    code_[actualLength_++] = createByteCode(opcode, val);
}

void TProgram::appendProgram(TProgram program) {
    for (size_t i = 0; i < program.size(); ++i) {
        append(program[i]);
    }
}

size_t TProgram::createInstructionSpace() {
    checkSpace();
    return ++actualLength_;
}

void TProgram::checkSpace() {
    if (actualLength_ == code_.size()) {
        code_.resize(code_.size() + ALLOC_BY);
    }
}

bool TProgram::operator==(const TProgram &other) const {
    if (actualLength_ != other.actualLength_) return false;
    if (code_ != other.code_) return false;
    return true;
}

std::string TProgram::string() const {
    std::string msg;
    int i = static_cast<int>(actualLength_);
    while (--i >= 0) {
        msg += OpCodeToString(code_[i].opCode);
        if (code_[i].index != -1) {
            msg += " " + std::to_string(code_[i].index);
        }
        msg += "\n";
    }
    return msg;
}

TSymbol::TSymbol(const std::string &name)
    : type_(TSymbolElementType::symUndefined), name_(name) {}

TSymbol::TSymbol(TUserFunction *fvalue)
    : type_(TSymbolElementType::symUserFunc), name_(fvalue->name()), fValue_(fvalue) {}

bool operator==(const TByteCode &lhs, const TByteCode &rhs) {
    return lhs.index == rhs.index && lhs.opCode == rhs.opCode;
}

bool operator!=(const TByteCode &lhs, const TByteCode &rhs) {
    return !(lhs == rhs);
}

bool TGlobalVariableList::find(const std::string &identifier, int &index) const {
    auto it = table_.find(identifier);
    if (it == table_.end()) {
        return false;
    } else {
        index = it->second;
        return true;
    }
}
