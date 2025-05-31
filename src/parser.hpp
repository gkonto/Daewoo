#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include "Token.hpp"
#include "lexer.hpp"
#include "macros.hpp"
#include <array>
#include <functional>
#include <memory>
#include <string>

class Program;
class Statement;
class Expression;
class LetStatement;
class IntegetLiteral;
class ReturnStatement;
class ExpressionStatement;
class IntegerLiteral;
class Boolean;
class BlockStatement;
class FunctionLiteral;
class Identifier;

enum Precedence
{
    Lowest,
    Equals,
    LessGreater,
    Sum,
    Product,
    Prefix,
    Call,
};

class Parser
{
public:
    using PrefixParseF = std::function<__Ptr<Expression>()>;
    using InfixParseF = std::function<__Ptr<Expression>(__Ptr<Expression>)>;
    using Errors = std::vector<std::string>;

    explicit Parser(Lexer &l);
    void nextToken();

    __Ptr<Program> parseProgram();
    __Ptr<Boolean> parseBoolean();
    __Ptr<Statement> parseStatement();
    __Ptr<Expression> parseIdentifier();
    __Ptr<IntegerLiteral> parseIntegerLiteral();
    __Ptr<Expression> parsePrefixExpression();
    __Ptr<Expression> parseGroupedExpression();
    __Ptr<Expression> parseIfExpression();
    __Ptr<BlockStatement> parseBlockStatement();
    __Ptr<FunctionLiteral> parseFunctionLiteral();
    __Ptr<Expression> parseInfixExpression(__Ptr<Expression> left);
    __Ptr<Expression> parseCallExpression(__Ptr<Expression> left);
    std::vector<__Ptr<Expression>> parseCallArguments();
    std::vector<__Ptr<Identifier>> parseFunctionParameters();

    __Ptr<Expression> parseExpression(Precedence p);
    __Ptr<LetStatement> parseLetStatement();
    __Ptr<ReturnStatement> parseReturnStatement();
    __Ptr<ExpressionStatement> parseExpressionStatement();

    bool curTokenIs(Token::Type t) const;
    bool peekTokenIs(Token::Type t) const;
    bool expectPeek(Token::Type t);
    void peekError(Token::Type t);

    const Errors &errors() const
    {
        return errors_;
    }
    void noPrefixParseFnError(Token::Type t);

    void registerPrefix(Token::Type t, PrefixParseF f);
    void registerInfix(Token::Type t, InfixParseF f);
    void registerPrecedence(Token::Type t, Precedence p);

    size_t precedenceAsValue(Precedence p)
    {
        return static_cast<size_t>(p);
    }
    Precedence peekPrecedence();
    Precedence curPrecedence();

private:
    Lexer &lexer_;
    Token cur_token_;
    Token peek_token_;

    Errors errors_;
    std::array<PrefixParseF, tokenTypesCount()> prefix_parse_fns_;
    std::array<InfixParseF, tokenTypesCount()> infix_parse_fns_;
    std::array<Precedence, tokenTypesCount()> precedences_ = {
        Precedence::Lowest};
};

#endif