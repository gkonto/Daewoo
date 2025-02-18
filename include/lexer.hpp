#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include <string>
#include <unordered_map>
#include "token.hpp"
#include "defs.hpp"

class Lexer
{
public:
    explicit Lexer(const std::string &input);

    void readChar();
    Length readIdentifier();
    Length readNumber();

    Token nextToken();
    Token::Type lookupIdent(const std::string &ident);
    void skipWhitespace();
    char peekChar();

    std::string getLiteral(const Token &token) const;

private:
    std::string input_;
    BufferSize position_ = 0;
    BufferSize read_position_ = 0;
    char ch_ = 0;

    static std::unordered_map<std::string, Token::Type> keywords_;
};

#endif