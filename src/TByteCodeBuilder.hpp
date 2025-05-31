#ifndef TBYTECODEBUILDER_HPP_INCLUDED
#define TBYTECODEBUILDER_HPP_INCLUDED

#include "TModule.hpp"
#include "TokenTable.hpp"
#include "lexer.hpp"

class Scanner;
class TProgram;
class TUserFunction;
class TSymbolTable;

/*
 * The TByteCodeBuilder assumes that the code is valid.
 * SyntaxAnalysis is responsible for this.
 */
class TByteCodeBuilder
{
public:
    explicit TByteCodeBuilder(TokensTable &sc) : sc_(sc)
    {
    }

    void build(TModule *module);

private:
    TokenCode code() const
    {
        return sc_.token().code();
    }
    void expect(TokenCode tcode);
    void nextToken()
    {
        sc_.nextToken();
    }
    const TokenRecord &token() const
    {
        return sc_.token();
    }

    void enterUserFunctionScope()
    {
        inUserFunctionParsing_ = true;
    }
    void enterVariableDefinition()
    {
        inVariableDefinition_ = true;
    }
    void exitUserFunctionScope()
    {
        inUserFunctionParsing_ = false;
    }
    void exitVariableDefinition()
    {
        inVariableDefinition_ = false;
    }
    bool inUserFunctionScope() const
    {
        return inUserFunctionParsing_;
    }
    bool inVariableDefinition() const
    {
        return inVariableDefinition_;
    }

    void statementList(TProgram &program);
    void statement(TProgram &program);
    void exprStatement(TProgram &program);
    void expression(TProgram &program);
    void simpleExpression(TProgram &program);
    void relationalOperators(TProgram &program);
    void term(TProgram &program);
    void power(TProgram &program);
    void factor(TProgram &program);
    void parseIdentifier(TProgram &program);
    void letStatement(TProgram &program);
    void ifStatement(TProgram &program);
    void functionDef(TProgram &program);
    int argumentList(TProgram &program);
    void argument(TProgram &program);
    void returnStmt(TProgram &program);
    void parseFunctionCall(TProgram &program, int expectedArguments);
    int expressionList(TProgram &program);

    TSymbolTable &symboltable()
    {
        return module_->symboltable();
    }

    TokensTable &sc_;
    TModule *module_ = nullptr;
    bool inUserFunctionParsing_ = false;
    bool inVariableDefinition_ = false;
    TUserFunction *currentUserFunction = nullptr;
};

#endif