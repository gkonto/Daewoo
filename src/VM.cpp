#include "VM.hpp"

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <memory>

#include "OpCodes.hpp"
#include "TModule.hpp"
#include "macros.hpp"
#include "ConstantTable.hpp"
#include "TListObject.hpp"

void VM::error(const std::string &arg,
               const TMachineStackRecord &st1,
               const TMachineStackRecord &st2) {
    throw std::runtime_error(TStackRecordTypeToStr(st1.type()) + " and " +
                             TStackRecordTypeToStr(st2.type()) + " cannot be used with the " + arg +
                             " operation");
}

void VM::runModule(std::shared_ptr<TModule> module) {
    module_ = module;
    run(module_->code());
}

void VM::run(const TProgram &code) {
    size_t ip = 0;  // instruction counter.

    while (true) {
        const auto &byteCode = code[ip];
        switch (byteCode.opCode) {
            case OpCode::Nop:
                break;
            case OpCode::Halt:
                return;
            case OpCode::Pushd:
                push(constantValueTable.get(byteCode.index).dvalue());
                break;
            case OpCode::Umi:
                unaryMinusOp();
                break;
            case OpCode::Pushi:
                push(byteCode.index);
                break;
            case OpCode::Add:
                addOp();
                break;
            case OpCode::Sub:
                subOp();
                break;
            case OpCode::Mult:
                multOp();
                break;
            case OpCode::Divide:
                divOp();
                break;
            case OpCode::Power:
                powerOp();
                break;
            case OpCode::Store:
                store(byteCode.index);
                // TODO garbage collection if size reached.
                break;
            case OpCode::Load:
                loadSymbol(byteCode.index);
                break;
            case OpCode::IsEq:
                isEq();
                break;
            case OpCode::Pushb:
                push(static_cast<bool>(byteCode.index));
                break;
            case OpCode::IsLt:
                isLt();
                break;
            case OpCode::IsNotEq:
                isNotEq();
                break;
            case OpCode::Not:
                notOp();
                break;
            case OpCode::And:
                andOp();
                break;
            case OpCode::Or:
                orOp();
                break;
            case OpCode::IsGt:
                isGt();
                break;
            case OpCode::IsGte:
                isGte();
                break;
            case OpCode::IsLte:
                isLte();
                break;
            case OpCode::JmpIfFalse:
                if (!stack_.pop().bvalue()) {
                    ip += byteCode.index - 1;
                }
                break;
            case OpCode::Jmp:
                ip += byteCode.index - 1;
                break;
            case OpCode::Inc:
            case OpCode::Dec:
            case OpCode::Xor:
            case OpCode::Mod:
            case OpCode::Pushs:
            case OpCode::PushNone:
            case OpCode::JmpIfTrue:
            case OpCode::LocalInc:
            case OpCode::LocalDec:
            case OpCode::LoadLocal:
            case OpCode::StoreLocal:
            case OpCode::Pop:
            case OpCode::Call:
            case OpCode::Return:
                throw std::runtime_error("VM::Unsupported opcode: " +
                                         OpCodeToString(byteCode.opCode));

                // case OpCode::Inc:
                //     incOp(byteCode.index);
                //     break;
                // case OpCode::Dec:
                //     decOp(byteCode.index);
                //     break;

                // case OpCode::Xor:
                //     xorOp();
                //     break;

                // case OpCode::Mod:
                //     modOp();
                //     break;

                // case OpCode::Pushs:
                //     push(constantValueTable[byteCode.index].svalue());
                //     break;
                // case OpCode::PushNone:
                //     push();
                //     break;
                // case OpCode::JmpIfTrue:
                //     if (stack_.pop().bvalue()) {
                //         ip += byteCode.index - 1;
                //     }
                //     break;

                // case OpCode::Load:
                //     loadSymbol(byteCode.index);
                //     break;
        }
        ++ip;
    }
}

void VM::store(int symTableIndex) {
    TMachineStackRecord record = stack_.pop();
    switch (record.type()) {
        case TStackRecordType::stInteger:
            module_->symboltable().storeSymbolToTable(symTableIndex, record.ivalue());
            break;
        case TStackRecordType::stBoolean:
            module_->symboltable().storeSymbolToTable(symTableIndex, record.bvalue());
            break;
        case TStackRecordType::stDouble:
            module_->symboltable().storeSymbolToTable(symTableIndex, record.dvalue());
            break;
        case TStackRecordType::stString:
        case TStackRecordType::stList:
            throw std::runtime_error("TODO VM::store not supported");  // FIXME
            break;
        case TStackRecordType::stNone:
            break;
    }
}

// void VM::load(int symTableIndex) {
//     // auto symbol = getFromSymbolTable(symTableIndex);
//     // switch (symbol.type())
//     // {
//     // case symInteger:
//     //     stack_.push(symbol.ivalue());
//     //     break;
//     // case symDouble:
//     //     stack_.push(symbol.dvalue());
//     //     break;
//     // case symBoolean:
//     //     stack_.push(symbol.bvalue());
//     //     break;
//     // case symString:
//     //     stack_.push(symbol.svalue());
//     //     break;
//     // }
// }

void VM::addOp() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();

    auto st1_typ = st1.type();
    auto st2_typ = st2.type();

    if (st1_typ == TStackRecordType::stNone || st2_typ == TStackRecordType::stNone) {
        throw std::runtime_error("RunTimeError: Variable undefined");
    }

    if (st2_typ == TStackRecordType::stInteger) {
        if (st1_typ == TStackRecordType::stInteger) {
            stack_.push(st1.ivalue() + st2.ivalue());
        } else if (st1_typ == TStackRecordType::stDouble) {
            stack_.push(st1.dvalue() + st2.ivalue());
        } else {
            error("adding", st2, st1);
        }
    } else if (st2_typ == TStackRecordType::stBoolean) {
        error("adding", st2, st1);  // Can't add booleans
    } else if (st2_typ == TStackRecordType::stDouble) {
        if (st1_typ == TStackRecordType::stInteger) {
            stack_.push(st1.ivalue() + st2.dvalue());
        } else if (st1_typ == TStackRecordType::stDouble) {
            stack_.push(st1.dvalue() + st2.dvalue());
        } else {
            error("adding", st2, st1);
        }
    } else if (st2_typ == TStackRecordType::stString) {
        if (st1_typ == TStackRecordType::stString) {
            stack_.push(TStringObject::add(*st2.svalue(), *st1.svalue()));
            throw std::runtime_error(IMPLEMENTATION_MISSING);
        } else {
            error("adding", st2, st1);
        }
    } else if (st2_typ == TStackRecordType::stList) {
        if (st1_typ == TStackRecordType::stInteger) {
            auto list = st2.lvalue();
            list->append(st1.ivalue());
            stack_.push(list);
        } else if (st1_typ == TStackRecordType::stBoolean) {
            auto list = st2.lvalue();
            list->append(st1.bvalue());
            stack_.push(list);
        } else if (st1_typ == TStackRecordType::stDouble) {
            auto list = st2.lvalue();
            list->append(st1.dvalue());
            stack_.push(list);
        } else if (st1_typ == TStackRecordType::stString) {
            auto list = st2.lvalue();
            list->append(st1.svalue());
            stack_.push(list);
        } else if (st1_typ == TStackRecordType::stList) {
            auto trg = st2.lvalue();
            auto src = st1.lvalue();
            trg->insert(src->begin(), src->end());
            stack_.push(src);
        } else {
            error("adding", st2, st1);
        }
    } else {
        throw std::runtime_error("Internal Error: Unsupported datatype in add");
    }
}

void VM::multOp() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();

    auto st1_typ = st1.type();
    auto st2_typ = st2.type();

    if (st1_typ == TStackRecordType::stNone || st2_typ == TStackRecordType::stNone) {
        throw std::runtime_error("RunTimeError: Variable undefined");
    }

    if (st2_typ == TStackRecordType::stInteger) {
        if (st1_typ == TStackRecordType::stInteger) {
            stack_.push(st1.ivalue() * st2.ivalue());
        } else if (st1_typ == TStackRecordType::stDouble) {
            stack_.push(st1.dvalue() * st2.ivalue());
        } else if (st1_typ == TStackRecordType::stString) {
            throw std::runtime_error("VM::multOp> Not implemented yet");
            // auto ans = st1.svalue()->value();
            // for (int i = 2; i < st2.ivalue(); ++i) {
            //     ans += st1.svalue()->value();
            // }
            // stack_.push(ans);
        } else if (st1_typ == TStackRecordType::stList) {
            throw std::runtime_error("VM::multOp> Not implemented yet");
            // auto buffer = st1.lvalue();
            // auto newList = buffer;
            // for (int i = 2; i < st2.ivalue(); ++i) {
            //     newList.insert(newList.end(), buffer.begin(), buffer.end());
            // }
            // stack_.push(newList);
        } else {
            error("multiplying", st2, st1);
        }
    } else if (st2_typ == TStackRecordType::stBoolean) {
        error("multiplying", st2, st1);  // Can't add booleans
    } else if (st2_typ == TStackRecordType::stDouble) {
        if (st1_typ == TStackRecordType::stInteger) {
            stack_.push(st1.ivalue() * st2.dvalue());
        } else if (st1_typ == TStackRecordType::stDouble) {
            stack_.push(st1.dvalue() * st2.dvalue());
        } else {
            error("multiplying", st2, st1);
        }
    } else if (st2_typ == TStackRecordType::stString) {
        throw std::runtime_error("VM::multOp> Not implemented yet");
        // if (st1_typ == TStackRecordType::stString) {
        //     auto ans = st2.svalue();
        //     for (int i = 2; i < st1.ivalue(); ++i) {
        //         ans += st2.svalue();
        //     }
        //     stack_.push(ans);
        // } else {
        //     error("multiplying", st2, st1);
        // }
    } else if (st2_typ == TStackRecordType::stList) {
        throw std::runtime_error("VM::multOp> Not implemented yet");

        // if (st1_typ == TStackRecordType::stInteger) {
        //     auto buffer = st2.lvalue();
        //     auto newList = buffer;
        //     for (int i = 2; i < st1.ivalue(); ++i) {
        //         newList.insert(newList.end(), buffer.begin(), buffer.end());
        //     }
        //     stack_.push(newList);
        // } else {
        //     throw std::runtime_error("Lists can only be multiplied by integers");
        // }
    } else {
        throw std::runtime_error("Internal Error: Unsupported datatype in multiply");
    }
}

void VM::divOp() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();

    auto st1_typ = st1.type();
    auto st2_typ = st2.type();

    if (st1_typ == TStackRecordType::stNone || st2_typ == TStackRecordType::stNone) {
        throw std::runtime_error("RunTimeError: Variable undefined");
    }

    if (st2_typ == TStackRecordType::stInteger) {
        if (st1_typ == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() / st1.ivalue());
        } else if (st1_typ == TStackRecordType::stDouble) {
            stack_.push(st2.ivalue() / st1.dvalue());
        } else {
            error("dividing", st2, st1);
        }
    } else if (st2_typ == TStackRecordType::stDouble) {
        if (st1_typ == TStackRecordType::stInteger) {
            stack_.push(st2.dvalue() / st1.ivalue());
        } else if (st1_typ == TStackRecordType::stDouble) {
            stack_.push(st2.dvalue() / st1.dvalue());
        } else {
            error("dividing", st2, st1);
        }
    } else {
        throw std::runtime_error("Internal Error: Unsupported datatype in dividing");
    }
}

void VM::subOp() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();

    auto st1_typ = st1.type();
    auto st2_typ = st2.type();

    if (st1_typ == TStackRecordType::stNone || st2_typ == TStackRecordType::stNone) {
        throw std::runtime_error("RunTimeError: Variable undefined");
    }

    if (st2_typ == TStackRecordType::stInteger) {
        if (st1_typ == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() - st1.ivalue());
        } else if (st1_typ == TStackRecordType::stDouble) {
            stack_.push(st2.ivalue() - st1.dvalue());
        } else {
            error("subtracting", st2, st1);
        }
    } else if (st2_typ == TStackRecordType::stDouble) {
        if (st1_typ == TStackRecordType::stInteger) {
            stack_.push(st2.dvalue() - st1.ivalue());
        } else if (st1_typ == TStackRecordType::stDouble) {
            stack_.push(st2.dvalue() - st1.dvalue());
        } else {
            error("subtracting", st2, st1);
        }
    } else {
        throw std::runtime_error(
            "Internal Error: Only integers and floats can be "
            "subtracted from each other");
    }
}

void VM::powerOp() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();

    auto st1_typ = st1.type();
    auto st2_typ = st2.type();

    if (st1_typ == TStackRecordType::stNone || st2_typ == TStackRecordType::stNone) {
        throw std::runtime_error("RunTimeError: Variable undefined");
    }

    if (st2_typ == TStackRecordType::stInteger) {
        if (st1_typ == TStackRecordType::stInteger) {
            stack_.push(std::pow(st2.ivalue(), st1.ivalue()));
        } else if (st1_typ == TStackRecordType::stDouble) {
            stack_.push(std::pow(st2.ivalue(), st1.dvalue()));
        } else {
            error("exponential", st2, st1);
        }
    } else if (st2_typ == TStackRecordType::stDouble) {
        if (st1_typ == TStackRecordType::stInteger) {
            stack_.push(std::pow(st2.dvalue(), st1.ivalue()));
        } else if (st1_typ == TStackRecordType::stDouble) {
            stack_.push(std::pow(st2.dvalue(), st1.dvalue()));
        } else {
            error("exponential", st2, st1);
        }
    } else {
        throw std::runtime_error("Internal Error: Data type not supported by power operator");
    }
}

void VM::unaryMinusOp() {
    auto st = stack_.pop();
    switch (st.type()) {
        case TStackRecordType::stInteger:
            stack_.push(-st.ivalue());
            break;
        case TStackRecordType::stDouble:
            stack_.push(-st.dvalue());
            break;
        default:
            throw std::runtime_error("Data type not supported by unary operator");
    }
}

// void VM::incOp(int index) {
//     auto &st = symboltable();
//     auto symbol = st.get(index);
//     switch (symbol.type()) {
//         case TSymbolElementType::Integer:
//             st.store(index, symbol.ivalue() + 1);
//             break;
//         case TSymbolElementType::Double:
//             st.store(index, symbol.dvalue() + 1);
//             break;
//         default:
//             throw std::runtime_error(
//                 "Internal error: Illegal use of incBy on a non-integer/double type");
//     }
// }

// void VM::decOp(int index) {
//     auto &st = symboltable();
//     auto symbol = st.get(index);
//     switch (symbol.type()) {
//         case TSymbolElementType::Integer:
//             st.store(index, symbol.ivalue() - 1);
//             break;
//         case TSymbolElementType::Double:
//             st.store(index, symbol.dvalue() - 1);
//             break;
//         default:
//             throw std::runtime_error(
//                 "Internal error: Illegal use of deccBy on a "
//                 "non-integer/double type");
//     }
// }

void VM::andOp() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();
    if (st1.type() == TStackRecordType::stBoolean && st2.type() == TStackRecordType::stBoolean) {
        stack_.push(st1.bvalue() && st2.bvalue());
        return;
    }
    throw std::runtime_error("Incompatible types in AND operation");
}

void VM::orOp() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();
    if (st1.type() == TStackRecordType::stBoolean && st2.type() == TStackRecordType::stBoolean) {
        stack_.push(st1.bvalue() || st2.bvalue());
        return;
    }
    throw std::runtime_error("Incompatible types in OR operation");
}

void VM::notOp() {
    auto st = stack_.pop();
    if (st.type() == TStackRecordType::stBoolean) {
        stack_.push(!st.bvalue());
        return;
    }
    throw std::runtime_error("Incompatible type in NOT operation");
}

// void VM::xorOp() {
//     auto st1 = stack_.pop();
//     auto st2 = stack_.pop();
//     if (st1.type() == TStackRecordType::stBoolean && st2.type() == TStackRecordType::stBoolean) {
//         stack_.push(st1.bvalue() ^ st2.bvalue());
//         return;
//     }
//     throw std::runtime_error("Incompatible types in XOR operation");
// }

void VM::isLt() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();
    auto st1_type = st1.type();
    auto st2_type = st2.type();
    if (st1_type == TStackRecordType::stInteger) {
        if (st2.type() == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() < st1.ivalue());
        } else if (st2_type == TStackRecordType::stDouble) {
            stack_.push(st2.dvalue() < st1.ivalue());
        } else {
            throw std::runtime_error("Incompatible types in ''Is Less Than'' Operation");
        }
    } else if (st1_type == TStackRecordType::stDouble) {
        if (st2_type == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() < st1.dvalue());
        } else if (st2_type == TStackRecordType::stDouble) {
            stack_.push(st2.dvalue() < st1.dvalue());
        } else {
            throw std::runtime_error("Incompatible types in ''Is Less Than'' Operation");
        }
    } else {
        throw std::runtime_error("Incompatible types in ''Is Less Than'' Operation");
    }
}

void VM::isGt() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();
    auto st1_type = st1.type();
    auto st2_type = st2.type();
    if (st1_type == TStackRecordType::stInteger) {
        if (st2.type() == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() > st1.ivalue());
        } else if (st2_type == TStackRecordType::stDouble) {
            stack_.push(st2.dvalue() > st1.ivalue());
        } else {
            throw std::runtime_error("Incompatible types in ''Is Greater Than'' Operation");
        }
    } else if (st1_type == TStackRecordType::stDouble) {
        if (st2_type == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() > st1.dvalue());
        } else if (st2_type == TStackRecordType::stDouble) {
            stack_.push(st2.dvalue() > st1.dvalue());
        } else {
            throw std::runtime_error("Incompatible types in ''Is Greater Than'' Operation");
        }
    } else {
        throw std::runtime_error("Incompatible types in ''Is Greater Than'' Operation");
    }
}

void VM::isGte() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();
    auto st1_type = st1.type();
    auto st2_type = st2.type();
    if (st1_type == TStackRecordType::stInteger) {
        if (st2.type() == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() >= st1.ivalue());
        } else if (st2_type == TStackRecordType::stDouble) {
            stack_.push(st2.dvalue() >= st1.ivalue());
        } else {
            throw std::runtime_error("Incompatible types in ''Is Greater-Equal Than'' Operation");
        }
    } else if (st1_type == TStackRecordType::stDouble) {
        if (st2_type == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() >= st1.dvalue());
        } else if (st2_type == TStackRecordType::stDouble) {
            stack_.push(st2.dvalue() >= st1.dvalue());
        } else {
            throw std::runtime_error("Incompatible types in 'Is Greater-Equal Than' Operation");
        }
    } else {
        throw std::runtime_error("Incompatible types in ''Is Greater-Equal Than'' Operation");
    }
}

void VM::isLte() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();
    auto st1_type = st1.type();
    auto st2_type = st2.type();
    if (st1_type == TStackRecordType::stInteger) {
        if (st2.type() == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() <= st1.ivalue());
        } else if (st2_type == TStackRecordType::stDouble) {
            stack_.push(st2.dvalue() <= st1.ivalue());
        } else {
            throw std::runtime_error("Incompatible types in ''Is Greater-Equal Than'' Operation");
        }
    } else if (st1_type == TStackRecordType::stDouble) {
        if (st2_type == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() <= st1.dvalue());
        } else if (st2_type == TStackRecordType::stDouble) {
            stack_.push(st2.dvalue() <= st1.dvalue());
        } else {
            throw std::runtime_error("Incompatible types in ''Is Greater-Equal Than'' Operation");
        }
    } else {
        throw std::runtime_error("Incompatible types in ''Is Greater-Equal Than'' Operation");
    }
}

// void VM::modOp() {
//     auto st1 = stack_.pop();
//     auto st2 = stack_.pop();
//     auto st1_type = st1.type();
//     auto st2_type = st2.type();

//     if (st1_type == TStackRecordType::stNone || st2_type == TStackRecordType::stNone) {
//         throw std::runtime_error("Variable undefined");
//     }

//     if (st2_type == TStackRecordType::stInteger) {
//         if (st1_type == TStackRecordType::stInteger) {
//             stack_.push(st2.ivalue() % st1.ivalue());
//         } else if (st1_type == TStackRecordType::stDouble) {
//             stack_.push(std::fmod(st2.ivalue(), st1.dvalue()));
//         } else {
//             throw std::runtime_error("Incompatible types in mod operation");
//         }
//     } else if (st2_type == TStackRecordType::stDouble) {
//         if (st1_type == TStackRecordType::stInteger) {
//             stack_.push(std::fmod(st2.dvalue(), st1.ivalue()));
//         } else if (st1_type == TStackRecordType::stDouble) {
//             stack_.push(std::fmod(st2.dvalue(), st1.dvalue()));
//         } else {
//             throw std::runtime_error("Incompatible types in mod operation");
//         }
//     } else {
//         throw std::runtime_error("Incompatible types in mod operation");
//     }
// }

void VM::isEq() {
    auto st1 = stack_.pop();
    auto st2 = stack_.pop();
    auto st1_type = st1.type();
    auto st2_type = st2.type();

    if (st1_type == TStackRecordType::stBoolean && st2_type == TStackRecordType::stBoolean) {
        stack_.push(st1.bvalue() == st2.bvalue());
        return;
    }

    if (st1_type == TStackRecordType::stInteger) {
        if (st2_type == TStackRecordType::stInteger) {
            stack_.push(st2.ivalue() == st1.ivalue());
        } else if (st2_type == TStackRecordType::stDouble) {
            stack_.push(std::fabs(st1.ivalue() - st2.dvalue()) < 1e-9);
        } else {
            throw std::runtime_error("Incompatible types in equality test");
        }
    } else if (st1_type == TStackRecordType::stDouble) {
        if (st2_type == TStackRecordType::stInteger) {
            stack_.push(std::fabs(st1.ivalue() - st2.dvalue()) < 1e-9);
        } else if (st2_type == TStackRecordType::stDouble) {
            stack_.push(std::fabs(st1.dvalue() - st2.dvalue()) < 1e-9);
        } else {
            throw std::runtime_error("Incompatible types in equality test");
        }
    } else if (st1_type == TStackRecordType::stString) {
        // TODO
        throw std::runtime_error("VM::isEq> TODO");
        // if (st2_type == TStackRecordType::stString) {
        //     stack_.push(!st1.svalue().compare(st2.svalue()));
        // } else {
        //     throw std::runtime_error("Incompatible types in equality test");
        // }
    } else if (st1_type == TStackRecordType::stList) {
        // TODO
        throw std::runtime_error("VM::isEq> Incompatible types in equality test");
    } else {
        throw std::runtime_error("VM::isEq> Incompatible types in equality test");
    }
}

void VM::isNotEq() {
    isEq();
    auto value = stack_.ctop().bvalue();
    stack_.top().setValue(!value);
}

void VM::loadSymbol(int index) {
    auto symbol = symboltable().get(index);
    switch (symbol.type()) {
        case TSymbolElementType::symUndefined:
            throw std::runtime_error("Undefined variable" + symbol.name());
        case TSymbolElementType::symInteger:
            stack_.push(symbol.ivalue());
            break;
        case TSymbolElementType::symBoolean:
            stack_.push(symbol.bvalue());
            break;
        case TSymbolElementType::symDouble:
            stack_.push(symbol.dvalue());
            break;
        case TSymbolElementType::symString:
            stack_.push(symbol.svalue());
            break;
        case TSymbolElementType::symList:
            stack_.push(symbol.lvalue());
            break;
        case TSymbolElementType::symUserFunc:
            // TODO
            throw std::runtime_error("VM::loadSymbol:: User function");
        case TSymbolElementType::symNonExistant:
            throw std::runtime_error("VM::nonExistant");
            break;
    }
}