#ifndef TOKEN_HPP_INCLUDED
#define TOKEN_HPP_INCLUDED

#include <string>
#include "defs.hpp"

struct Token
{
    enum class Type
    {
        Illegal,
        Eof,
        Ident,
        Int,
        Assign,
        Plus,
        Comma,
        Semicolon,
        LParen,
        RParen,
        LBrace,
        RBrace,
        Function,
        Let,
        Minus,
        Bang,
        Asterisk,
        Slash,
        LessThan,
        GreaterThan,
        True,
        False,
        If,
        Else,
        Return,
        EQ,
        NotEQ,
        Max,
    };

    Token() = default;
    Token(Type type, BufferSize sp, Length l);

    Type type_ = Type::Illegal;
    BufferSize position_ = 0;
    Length length_ = 0;
};

std::string tokenType2String(Token::Type type);
std::ostream &operator<<(std::ostream &os, const Token &token);

constexpr size_t tokenTypesCount()
{
    return static_cast<size_t>(Token::Type::Max);
}

#endif