#ifndef ASTCONSTRUCT_HPP_INCLUDED
#define ASTCONSTRUCT_HPP_INCLUDED

#include <memory>
#include "tokenTable.hpp"
#include "token.hpp"
#include "ASTNode.hpp"

class ASTBuilder
{
public:
    explicit ASTBuilder(TokensTable &sc);
    std::unique_ptr<ASTProgram> build();

private:
    std::unique_ptr<ASTNode> expect(TokenCode tc);

    std::unique_ptr<ASTNodeList> statementList();
    std::unique_ptr<ASTNodeList> functionArgumentList();
    std::unique_ptr<ASTNode> statement();
    std::unique_ptr<ASTNode> ifStatement();
    std::unique_ptr<ASTNode> returnStmt();
    std::unique_ptr<ASTNode> parseUserDefinedFunction();
    std::unique_ptr<ASTNode> exprStatement();
    std::unique_ptr<ASTNode> expression();
    std::unique_ptr<ASTNode> functionArgument();
    std::unique_ptr<ASTIdentifier> variable();
    std::unique_ptr<ASTNode> relationalOperators();
    std::unique_ptr<ASTNode> simpleExpression();
    std::unique_ptr<ASTNode> term();
    std::unique_ptr<ASTNode> power();
    std::unique_ptr<ASTNode> primary();
    std::unique_ptr<ASTNode> factor();
    std::unique_ptr<ASTNode> primaryPlus();
    std::unique_ptr<ASTNodeList> parseFunctionCall();
    std::unique_ptr<ASTNodeList> expressionList();
    std::unique_ptr<ASTNode> letStatement();

    void enterUserFunctionScope() { inUserFunctionParsing = true; }
    void exitUserFunctionScope() { inUserFunctionParsing = false; }
    TokenCode code() const { return sc_.token().code(); }
    void nextToken() { sc_.nextToken(); }
    int lineNumber() const { return sc_.token().lineNumber(); }
    int columnNumber() const { return sc_.token().columnNumber(); }
    const TokenRecord &token() const { return sc_.token(); }

    TokensTable &sc_;
    bool inUserFunctionParsing = false;
    std::vector<std::string> globalVariableList;
};

#endif