#ifndef ASTNODETYPE_HPP_INCLUDED
#define ASTNODETYPE_HPP_INCLUDED

enum class ASTNodeType
{
    ntError,
    ntFloat,
    ntInteger,
    ntString,
    ntBoolean,
    ntNodeList,
    ntAND, // 6
    ntOR,
    ntNOT,
    ntXOR,
    ntUnaryMinus, // 10
    ntAdd,
    ntSub,
    ntMult,
    ntDiv,
    ntDivI,
    ntMod,
    ntPower,
    ntDotProduct,
    ntLT,
    ntLE,
    ntGT, // 21
    ntGE,
    ntNE,
    ntEQ,
    ntLeftSide,
    ntExpression,
    ntSubscript,
    ntNode,
    ntImportStmt,
    ntGlobalStmt, // 30
    ntModule,
    ntCreateList,
    ntPrint,
    ntPrintln,
    ntSetColor,
    ntAssertTrue,
    ntAssertFalse,
    ntAssertFalseEx,
    ntHelp,
    ntSwitch, // 40
    ntListOfCaseStatements,
    ntCaseStatement,
    ntIf,
    ntWhile,
    ntRepeat,
    ntFor,
    ntIter,
    ntTo,
    ntDownTo,
    ntFunction, // 50
    ntFunctionArguments,
    ntReturn,
    ntAssignment,
    ntAssignmentModule,
    ntBreak,
    ntFunctionCall,
    ntStatementList,
    ntExpressionStatement,
    ntIdentifier,
    ntPrimary, // 60
    ntPrimaryPeriod,
    ntPrimaryIndex,
    ntPrimaryFunction,
    ntSlice,
    ntSliceAll,
    ntSliceEqual,
    ntCreateVector,
    ntAddVectorItem,
    ntCreateMatrix,
    ntNull,
    ntProgram
};

#endif