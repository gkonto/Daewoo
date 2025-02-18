#include "token.hpp"
#include <unordered_map>

Token::Token(Type type, BufferSize sp, Length l)
    : type_(type), position_(sp), length_(l)
{
}

std::string tokenType2String(Token::Type type)
{
    switch (type)
    {
    case Token::Type::Illegal:
        return "ILLEGAL";
    case Token::Type::Eof:
        return "EOF";
    case Token::Type::Ident:
        return "IDENT";
    case Token::Type::Int:
        return "INT";
    case Token::Type::Assign:
        return "=";
    case Token::Type::Plus:
        return "+";
    case Token::Type::Comma:
        return ",";
    case Token::Type::Semicolon:
        return ";";
    case Token::Type::LParen:
        return "(";
    case Token::Type::RParen:
        return ")";
    case Token::Type::LBrace:
        return "{";
    case Token::Type::RBrace:
        return "}";
    case Token::Type::Function:
        return "FUNCTION";
    case Token::Type::Let:
        return "LET";
    case Token::Type::Minus:
        return "-";
    case Token::Type::Bang:
        return "!";
    case Token::Type::Asterisk:
        return "*";
    case Token::Type::Slash:
        return "/";
    case Token::Type::LessThan:
        return "<";
    case Token::Type::GreaterThan:
        return ">";
    case Token::Type::True:
        return "TRUE";
    case Token::Type::False:
        return "FALSE";
    case Token::Type::If:
        return "IF";
    case Token::Type::Else:
        return "ELSE";
    case Token::Type::Return:
        return "RETURN";
    case Token::Type::EQ:
        return "==";
    case Token::Type::NotEQ:
        return "!=";
    case Token::Type::Max:
        break;
    }
    return "";
}
