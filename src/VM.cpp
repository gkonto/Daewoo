#include "VM.hpp"
#include "TModule.hpp"
#include "OpCodes.hpp"

void VM::runModule(std::shared_ptr<TModule> module)
{
    module_ = module;
    run(module_->code());
}

void VM::run(const TProgram &code)
{
    int ip = 0; // instruction counter.

    while (true)
    {
        const auto &byteCode = code[ip];
        switch (byteCode.opCode)
        {
        case OpCodes::Pushi:
            // push(byteCode.index);
            break;
            // case OpCodes::Add:
            //     addOp();
            //     break;
            // case OpCodes::Sub:
            //     subOp();
            //     break;
            // case OpCodes::Mult:
            //     multOp();
            //     break;
            // case OpCodes::Divide:
            //     divOp();
            //     break;
            // case OpCodes::Umi:
            //     unaryMinusOp();
            //     break;
            // case OpCodes::Power:
            //     powerOp();
            //     break;
        }
    }
}

void VM::store(int symTableIndex)
{
    TMachineStackRecord record = stack_.pop();
    switch (record.type())
    {
    case TStackType::stInteger:
        // storeToSymbolTable(symTableIndex, std::get<int>(record.value()));
        break;
    case TStackType::stBoolean:
        // storeToSymbolTable(symTableIndex, std::get<bool>(record.value()));
        break;
    case TStackType::stDouble:
        // storeToSymbolTable(symTableIndex, std::get<double>(record.value()));
        break;
    case TStackType::stString:
        // storeToSymbolTable(symTableIndex, std::get<std::string>(record.value()));
        break;
    case TStackType::stNone:
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