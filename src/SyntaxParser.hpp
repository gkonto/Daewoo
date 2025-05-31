#ifndef SYNTAX_PARSER_HPP_INCLUDED
#define SYNTAX_PARSER_HPP_INCLUDED

#include "Token.hpp"
#include "TokenTable.hpp"
#include <optional>
#include <vector>

class Scanner;

class SyntaxError
{
public:
    template <typename T>
    SyntaxError(T &&msg) : msg_(std::forward<std::string>(msg))
    {
    }

    const std::string &msg() const
    {
        return msg_;
    }

private:
    std::string msg_;
};

class SyntaxParser
{
public:
    explicit SyntaxParser(Scanner &sc);
    std::optional<SyntaxError> syntaxCheck();
    TokensTable &tokens()
    {
        return tokenVector_;
    }

private:
    TokensTable tokenVector_;

    void nextToken();

    std::optional<SyntaxError> expect(TokenCode code);
    std::optional<SyntaxError> statementList();
    std::optional<SyntaxError> statement();
    std::optional<SyntaxError> ifStatement();
    std::optional<SyntaxError> returnStatement();
    std::optional<SyntaxError> letStatement();
    std::optional<SyntaxError> parseUserDefinedFunction();
    std::optional<SyntaxError> expressionStatement();
    std::optional<SyntaxError> expression();
    std::optional<SyntaxError> relationalOperators();
    std::optional<SyntaxError> simpleExpression();
    std::optional<SyntaxError> term();
    std::optional<SyntaxError> power();
    std::optional<SyntaxError> primary();
    std::optional<SyntaxError> factor();
    std::optional<SyntaxError> primaryPlus();
    std::optional<SyntaxError> parseFunctionCall();
    std::optional<SyntaxError> expressionList();
    std::optional<SyntaxError> functionArgumentList();
    std::optional<SyntaxError> variable();
    std::optional<SyntaxError> functionArgument();

    void enterUserFunctionScope()
    {
        inUserFunctionParsing_ = true;
    }
    void exitUserFunctionScope()
    {
        inUserFunctionParsing_ = false;
    }

    bool inUserFunctionParsing_ = false;
    std::vector<std::string> globalVariableList;
    Scanner &sc_;

    std::optional<SyntaxError> error_;
};

#endif