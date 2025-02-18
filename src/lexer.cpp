#include "lexer.hpp"
#include "token.hpp"

#include <stdexcept>
#include <limits>

std::unordered_map<std::string, Token::Type> Lexer::keywords_ = {
    {"fn", Token::Type::Function},
    {"let", Token::Type::Let},
    {"true", Token::Type::True},
    {"false", Token::Type::False},
    {"if", Token::Type::If},
    {"else", Token::Type::Else},
    {"return", Token::Type::Return}};

static bool isLetter(char ch)
{
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch == 'Z') || ch == '_';
}

static bool isDigit(char ch)
{
    return '0' <= ch && ch <= '9';
}

Lexer::Lexer(const std::string &input)
    : input_(input)
{
    if (input_.size() > std::numeric_limits<BufferSize>::max())
    {
        throw std::out_of_range("Input size exceeds maximum allowed BufferSize");
    }

    readChar();
}

void Lexer::readChar()
{
    if (read_position_ >= input_.size())
    {
        ch_ = 0;
    }
    else
    {
        ch_ = input_.at(read_position_);
    }

    if (input_.size() > std::numeric_limits<decltype(read_position_)>::max())
    {
        throw std::out_of_range("Input size exceeds maximum allowed BufferSize");
    }

    position_ = read_position_++;
}

Token Lexer::nextToken()
{
    Token token;

    skipWhitespace();

    switch (ch_)
    {
    case '=':
        if (peekChar() == '=')
        {
            token = Token(Token::Type::EQ, position_, 2);
            readChar();
        }
        else
        {
            token = Token(Token::Type::Assign, position_, 1);
        }
        break;
    case '-':
        token = Token(Token::Type::Minus, position_, 1);
        break;
    case '!':
        if (peekChar() == '=')
        {
            token = Token(Token::Type::NotEQ, position_, 2);
            readChar();
        }
        else
        {
            token = Token(Token::Type::Bang, position_, 1);
        }
        break;
    case '*':
        token = Token(Token::Type::Asterisk, position_, 1);
        break;
    case '/':
        token = Token(Token::Type::Slash, position_, 1);
        break;
    case '<':
        token = Token(Token::Type::LessThan, position_, 1);
        break;
    case '>':
        token = Token(Token::Type::GreaterThan, position_, 1);
        break;
    case ';':
        token = Token(Token::Type::Semicolon, position_, 1);
        break;
    case '(':
        token = Token(Token::Type::LParen, position_, 1);
        break;
    case ')':
        token = Token(Token::Type::RParen, position_, 1);
        break;
    case ',':
        token = Token(Token::Type::Comma, position_, 1);
        break;
    case '+':
        token = Token(Token::Type::Plus, position_, 1);
        break;
    case '{':
        token = Token(Token::Type::LBrace, position_, 1);
        break;
    case '}':
        token = Token(Token::Type::RBrace, position_, 1);
        break;
    case 0:
        token = Token(Token::Type::Eof, position_, 0);
        break;
    default:
        if (isLetter(ch_))
        {
            token.position_ = position_;
            token.length_ = readIdentifier();
            token.type_ = lookupIdent(getLiteral(token));
            return token;
        }
        else if (isDigit(ch_))
        {
            token.position_ = position_;
            token.type_ = Token::Type::Int;
            token.length_ = readNumber();
            return token;
        }
        else
        {
            token = Token(Token::Type::Illegal, position_, 1);
        }
    }

    readChar();
    return token;
}

std::string Lexer::getLiteral(const Token &token) const
{
    return input_.substr(token.position_, token.length_);
}

Length Lexer::readIdentifier()
{
    BufferSize position = position_;
    while (isLetter(ch_))
    {
        readChar();
    }

    BufferSize length = position_ - position;
    if (length > std::numeric_limits<Length>::max())
    {
        throw std::out_of_range("Input size exceeds maximum allowed BufferSize");
    }

    return static_cast<Length>(length);
}

Token::Type Lexer::lookupIdent(const std::string &ident)
{
    auto it = keywords_.find(ident);
    return it == keywords_.end() ? Token::Type::Ident : it->second;
}

void Lexer::skipWhitespace()
{
    while (ch_ == ' ' || ch_ == '\t' || ch_ == '\n' || ch_ == '\r')
    {
        readChar();
    }
}

Length Lexer::readNumber()
{
    BufferSize position = position_;
    while (isDigit(ch_))
    {
        readChar();
    }

    BufferSize length = position_ - position;
    if (length > std::numeric_limits<Length>::max())
    {
        throw std::out_of_range("Input size exceeds maximum allowed BufferSize");
    }

    return static_cast<Length>(length);
}

char Lexer::peekChar()
{
    if (read_position_ >= input_.size())
    {
        return 0;
    }
    else
    {
        return input_[read_position_];
    }
}
