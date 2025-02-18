#ifndef PARSER_HPP_INCLUDED
#define PARSER_HPP_INCLUDED

#include "lexer.hpp"
#include "token.hpp"
#include <memory>
#include <string>
#include <array>
#include <functional>

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
    using PrefixParseF = std::function<std::unique_ptr<Expression>()>;
    using InfixParseF = std::function<std::unique_ptr<Expression>(std::unique_ptr<Expression>)>;
    using Errors = std::vector<std::string>;

    explicit Parser(Lexer &l);
    void nextToken();

    std::unique_ptr<Program> parseProgram();
    std::unique_ptr<Boolean> parseBoolean();
    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<Expression> parseIdentifier();
    std::unique_ptr<IntegerLiteral> parseIntegerLiteral();
    std::unique_ptr<Expression> parsePrefixExpression();
    std::unique_ptr<Expression> parseGroupedExpression();
    std::unique_ptr<Expression> parseIfExpression();
    std::unique_ptr<BlockStatement> parseBlockStatement();
    std::unique_ptr<FunctionLiteral> parseFunctionLiteral();
    std::unique_ptr<Expression> parseInfixExpression(std::unique_ptr<Expression> left);
    std::unique_ptr<Expression> parseCallExpression(std::unique_ptr<Expression> left);
    std::vector<std::unique_ptr<Expression>> parseCallArguments();
    std::vector<std::unique_ptr<Identifier>> parseFunctionParameters();

    std::unique_ptr<Expression> parseExpression(Precedence p);
    std::unique_ptr<LetStatement> parseLetStatement();
    std::unique_ptr<ReturnStatement> parseReturnStatement();
    std::unique_ptr<ExpressionStatement> parseExpressionStatement();

    bool curTokenIs(Token::Type t) const;
    bool peekTokenIs(Token::Type t) const;
    bool expectPeek(Token::Type t);
    void peekError(Token::Type t);

    const Errors &errors() const { return errors_; }
    void noPrefixParseFnError(Token::Type t);

    void registerPrefix(Token::Type t, PrefixParseF f);
    void registerInfix(Token::Type t, InfixParseF f);
    void registerPrecedence(Token::Type t, Precedence p);

    size_t precedenceAsValue(Precedence p) { return static_cast<size_t>(p); }
    Precedence peekPrecedence();
    Precedence curPrecedence();

private:
    Lexer &lexer_;
    Token cur_token_;
    Token peek_token_;

    Errors errors_;
    std::array<PrefixParseF, tokenTypesCount()> prefix_parse_fns_;
    std::array<InfixParseF, tokenTypesCount()> infix_parse_fns_;
    std::array<Precedence, tokenTypesCount()> precedences_ = {Precedence::Lowest};
};

#endif