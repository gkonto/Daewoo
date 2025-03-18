/* DONE */
#include "OpCodes.hpp"
#include <string>

std::string OpCodeToName(OpCodes code) {
    switch (code) {
        case OpCodes::Nop:
            return "nop";
        case OpCodes::Halt:
            return "halt";
        case OpCodes::Add:
            return "add";
        case OpCodes::Sub:
            return "sub";
        case OpCodes::Mult:
            return "mult";
        case OpCodes::Divide:
            return "divide";
        case OpCodes::Mod:
            return "mod";
        case OpCodes::Umi:
            return "umi";
        case OpCodes::Power:
            return "power";
        case OpCodes::Inc:
            return "inc";
        case OpCodes::LocalInc:
            return "localInc";
        case OpCodes::Dec:
            return "dec";
        case OpCodes::LocalDec:
            return "localDec";
        case OpCodes::Load:
            return "load";
        case OpCodes::Store:
            return "store";
        case OpCodes::LoadLocal:
            return "loadLocal";
        case OpCodes::StoreLocal:
            return "storeLocal";
        case OpCodes::And:
            return "and";
        case OpCodes::Or:
            return "or";
        case OpCodes::Not:
            return "not";
        case OpCodes::Xor:
            return "xor";
        case OpCodes::Pushi:
            return "pushi";
        case OpCodes::Pushb:
            return "pushb";
        case OpCodes::Pushd:
            return "pushd";
        case OpCodes::Pushs:
            return "pushs";
        case OpCodes::PushNone:
            return "pushNone";
        case OpCodes::Pop:
            return "pop";
        case OpCodes::IsEq:
            return "isEq";
        case OpCodes::IsGt:
            return "isGt";
        case OpCodes::IsGte:
            return "isGte";
        case OpCodes::IsLt:
            return "isLt";
        case OpCodes::IsLte:
            return "isLte";
        case OpCodes::IsNotEq:
            return "isNotEq";
        case OpCodes::Jmp:
            return "jmp";
        case OpCodes::JmpIfTrue:
            return "jmpIfTrue";
        case OpCodes::JmpIfFalse:
            return "jmpIfFalse";
            // case OpCodes::LvecIdx:
            //     return "lvecIdx";
            // case OpCodes::SvecIdx:
            //     return "svecIdx";
            // case OpCodes::LocalLvecIdx:
            //     return "locallvecIdx";
            // case OpCodes::LocalSvecIdx:
            return "localsvecIdx";
        case OpCodes::Call:
            return "call";
        // case OpCodes::BuiltIn:
        //     return "builtin";
        case OpCodes::Ret:
            return "ret";
            // case OpCodes::CreateList:
            //     return "createList";
            // case OpCodes::AppendList:
            //     return "appendList";
    }
    return "";
}