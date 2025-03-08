#ifndef OPCODES_HPP_INCLUDED
#define OPCODES_HPP_INCLUDED

#include <string>

enum class OpCodes
{
    Nop,
    Halt,
    // Arithmetic
    Add, // Pop two values off stack; add them and push reult on to stack
    Sub,
    Mult,
    // Divi, // Integer division
    Mod, // Modulus of two integer values
    Divide,
    Umi,
    Power,
    Inc, // Increment stack entry by arg
    LocalInc,
    Dec, // Decrement stack entry by arg
    LocalDec,

    // Load and store values to symbol table
    Load,       // Load symbol argument onto stack, operand contains index to the symbol table
    Store,      // Save item on stack to symbol table, operand contains index to the symbol table
    LoadLocal,  // Load local symbol argument onto stack, operand contains index to the local symbol table
    StoreLocal, // Save item on stack to local symbol table, operand contains index to the symbol table

    // Logical
    And,
    Or,
    Not,
    Xor,

    // Push and pop to stac
    Pushi,    // Push integer onto stack, operand contains the integer
    Pushd,    // Push double onto stack, operand contains index to constant table
    Pushb,    // Push boolean value onto stack, operand contains index to constant table
    Pushs,    // Push string value onto stack, operand contains index to constant table
    PushNone, // Push noneStackType onto the stack, used by retVal for returning nothing
    Pop,      // Only pop() stack

    // Boolean Tests
    IsEq,    // Push True if TOS1 = TOS
    IsGt,    // Push True if TOS1 > TOS
    IsGte,   // Push True if TOS1 >= TOS
    IsLt,    // Push True if TOS1 < TOS
    IsLte,   // Push True if TOS1 <= TOS
    IsNotEq, // Push True if TOS1 <> TOS

    // Jump Instructions
    Jmp,        // Relative jump to new pc location
    JmpIfTrue,  // Relative jump if stack entry is True
    JmpIfFalse, // Relative jump if stack entry is False

    CreateList,   //
    LvecIdx,      // Load element from vector
    SvecIdx,      // Save element to vector
    LocalLvecIdx, // Load element from local vector
    LocalSvecIdx, // Save element to local vector

    // Calling routines
    Call,    // Call a user defined function
    BuiltIn, // Call a builin function
    Ret,     // Return from a function

    Print,       // Pop the stack and write the item to stdout
    Println,     // Pop the stack and write a newline stdout
    AssertTrue,  // Pop the stack and check that the operand is a boolean True
    AssertFalse, // Pop the stack and check that the operand is a boolean False

    AppendList,

    PopAndSend // Debug opcode
};

std::string OpCodeToName(OpCodes code);

#endif
