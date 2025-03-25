#ifndef TBYTECODEBUILDER_HPP_INCLUDED
#define TBYTECODEBUILDER_HPP_INCLUDED

#include "lexer.hpp"
#include "TokenTable.hpp"

class Scanner;
class TModule;
class TProgram;

/*
 * The TByteCodeBuilder assumes that the code is valid.
 * SyntaxAnalysis is responsible for this.
 */
class TByteCodeBuilder {
public:
    explicit TByteCodeBuilder(TokensTable &sc)
        : sc_(sc) {}

    void build(TModule *module);

private:
    TokenCode code() const { return sc_.token().code(); }
    void expect(TokenCode tcode);
    void nextToken() { sc_.nextToken(); }
    const TokenRecord &token() const { return sc_.token(); }

    void enterUserFunctionScope() { inUserFunctionParsing_ = true; }
    void exitUserFunctionScope() { inUserFunctionParsing_ = false; }
    bool inUserFunctionScope() const { return inUserFunctionParsing_; }

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

    TokensTable &sc_;
    TModule *module_ = nullptr;
    bool inUserFunctionParsing_ = false;
};

#endif