#include <stdexcept>

#include "VM.hpp"
#include "TModule.hpp"
#include "OpCodes.hpp"
#include "macros.hpp"

void VM::error(const std::string &arg, const TMachineStackRecord &st1, const TMachineStackRecord &st2)
{
    throw std::runtime_error(TStackRecordTypeToStr(st1.type()) +
                             " and " +
                             TStackRecordTypeToStr(st2.type()) +
                             " cannot be used with the " +
                             arg +
                             " operation");
}

void VM::runModule(std::shared_ptr<TModule> module)
{
    module_ = module;
    run(module_->code());
}

void VM::run(const TProgram &code)
{
    size_t ip = 0; // instruction counter.

    while (true)
    {
        const auto &byteCode = code[ip];
        switch (byteCode.opCode)
        {
        case OpCodes::Nop:
            break;
        case OpCodes::Halt:
            break;
        case OpCodes::Add:
            addOp();
            break;
        case OpCodes::Mult:
            multOp();
            break;
            // case OpCodes::Pushi:
            //  push(byteCode.index);
            // break;
            //  case OpCodes::Add:
            //      addOp();
            //      break;
            //  case OpCodes::Sub:
            //      subOp();
            //      break;
            //  case OpCodes::Mult:
            //      multOp();
            //      break;
            //  case OpCodes::Divide:
            //      divOp();
            //      break;
            //  case OpCodes::Umi:
            //      unaryMinusOp();
            //      break;
            //  case OpCodes::Power:
            //      powerOp();
            //      break;
        }
    }
}

void VM::store(int symTableIndex)
{
    TMachineStackRecord record = stack_.pop();
    switch (record.type())
    {
    case TStackRecordType::stInteger:
        // storeToSymbolTable(symTableIndex, std::get<int>(record.value()));
        break;
    case TStackRecordType::stBoolean:
        // storeToSymbolTable(symTableIndex, std::get<bool>(record.value()));
        break;
    case TStackRecordType::stDouble:
        // storeToSymbolTable(symTableIndex, std::get<double>(record.value()));
        break;
    case TStackRecordType::stString:
        // storeToSymbolTable(symTableIndex, std::get<std::string>(record.value()));
        break;
    case TStackRecordType::stNone:
        break;
    }
}

void VM::load(int symTableIndex)
{
    // auto symbol = getFromSymbolTable(symTableIndex);
    // switch (symbol.type())
    // {
    // case symInteger:
    //     stack_.push(symbol.ivalue());
    //     break;
    // case symDouble:
    //     stack_.push(symbol.dvalue());
    //     break;
    // case symBoolean:
    //     stack_.push(symbol.bvalue());
    //     break;
    // case symString:
    //     stack_.push(symbol.svalue());
    //     break;
    // }
}

void VM::addOp()
{
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();

    auto st1_typ = st1.type();
    auto st2_typ = st2.type();

    if (st1_typ == TStackRecordType::stNone ||
        st2_typ == TStackRecordType::stNone)
    {
        throw std::runtime_error("RunTimeError: Variable undefined");
    }

    if (st2_typ == TStackRecordType::stInteger)
    {
        if (st1_typ == TStackRecordType::stInteger)
        {
            stack_.push(st1.ivalue() + st2.ivalue());
        }
        else if (st1_typ == TStackRecordType::stDouble)
        {
            stack_.push(st1.dvalue() + st2.ivalue());
        }
        else
        {
            error("adding", st2, st1);
        }
    }
    else if (st2_typ == TStackRecordType::stBoolean)
    {
        error("adding", st2, st1); // Can't add booleans
    }
    else if (st2_typ == TStackRecordType::stDouble)
    {
        if (st1_typ == TStackRecordType::stInteger)
        {
            stack_.push(st1.ivalue() + st2.dvalue());
        }
        else if (st1_typ == TStackRecordType::stDouble)
        {
            stack_.push(st1.dvalue() + st2.dvalue());
        }
        else
        {
            error("adding", st2, st1);
        }
    }
    else if (st2_typ == TStackRecordType::stString)
    {
        if (st1_typ == TStackRecordType::stString)
        {
            throw std::runtime_error(IMPLEMENTATION_MISSING);
        }
        else
        {
            error("adding", st2, st1);
        }
    }
    else if (st2_typ == TStackRecordType::stList)
    {
        throw std::runtime_error(IMPLEMENTATION_MISSING);
        if (st1_typ == TStackRecordType::stInteger)
        {
        }
        else if (st1_typ == TStackRecordType::stBoolean)
        {
        }
        else if (st1_typ == TStackRecordType::stDouble)
        {
        }
        else if (st1_typ == TStackRecordType::stString)
        {
        }
        else if (st1_typ == TStackRecordType::stList)
        {
        }
        else
        {
            error("adding", st2, st1);
        }
    }
    else
    {
        throw std::runtime_error("Internal Error: Unsupported datatype in add");
    }
}

void VM::multOp()
{
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();

    auto st1_typ = st1.type();
    auto st2_typ = st2.type();

    if (st1_typ == TStackRecordType::stNone ||
        st2_typ == TStackRecordType::stNone)
    {
        throw std::runtime_error("RunTimeError: Variable undefined");
    }

    if (st2_typ == TStackRecordType::stInteger)
    {
        if (st1_typ == TStackRecordType::stInteger)
        {
            stack_.push(st1.ivalue() * st2.ivalue());
        }
        else if (st1_typ == TStackRecordType::stDouble)
        {
            stack_.push(st1.dvalue() * st2.ivalue());
        }
        else if (st1_typ == TStackRecordType::stString)
        {
            throw std::runtime_error(IMPLEMENTATION_MISSING);
        }
        else
        {
            error("multiplying", st2, st1);
        }
    }
    else if (st2_typ == TStackRecordType::stBoolean)
    {
        error("multiplying", st2, st1); // Can't add booleans
    }
    else if (st2_typ == TStackRecordType::stDouble)
    {
        if (st1_typ == TStackRecordType::stInteger)
        {
            stack_.push(st1.ivalue() + st2.dvalue());
        }
        else if (st1_typ == TStackRecordType::stDouble)
        {
            stack_.push(st1.dvalue() + st2.dvalue());
        }
        else
        {
            error("multiplying", st2, st1);
        }
    }
    else if (st2_typ == TStackRecordType::stString)
    {
        if (st1_typ == TStackRecordType::stString)
        {
        }
        else
        {
            error("multiplying", st2, st1);
        }
    }
    else if (st2_typ == TStackRecordType::stList)
    {
        throw std::runtime_error(IMPLEMENTATION_MISSING);
        if (st1_typ == TStackRecordType::stInteger)
        {
        }
        else
        {
            throw std::runtime_error("Lists can only be multiplied by integers");
        }
    }
    else
    {
        throw std::runtime_error("Internal Error: Unsupported datatype in add");
    }
}