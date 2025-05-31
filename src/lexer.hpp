#ifndef LEXER_HPP_INCLUDED
#define LEXER_HPP_INCLUDED

#include "Token.hpp"
#include "defs.hpp"
#include <deque>
#include <istream>
#include <string>
#include <unordered_map>
#include <utility>

class ScannerError
{
public:
    ScannerError() = default;
    ScannerError(const std::string &errorMsg, int lineNumber, int columnNumber)
        : errorMsg_(errorMsg), lineNumber_(lineNumber),
          columnNumber_(columnNumber)
    {
    }

    template <typename T>
    void setError(T &&msg, int lineNumber, int columnNumber)
    {
        errorMsg_ = std::forward<std::string>(msg);
        lineNumber_ = lineNumber;
        columnNumber_ = columnNumber;
        throw std::runtime_error(errorMsg_);
    }

    bool empty() const
    {
        return errorMsg_.empty();
    }

private:
    std::string errorMsg_;
    int lineNumber_ = 0;
    int columnNumber_ = 0;
};

class Scanner
{
public:
    Scanner(std::istream &input);
    void nextToken();
    const TokenRecord &token() const
    {
        return tokenRecord_;
    }

    std::string tokenToString(TokenCode code);

private:
    bool isEof() const
    {
        return reader_.eof();
    }
    void startScanner();
    void skipBlanksAndComments();
    void skipSingleLineComment();
    void skipMultiLineComment();

    char readRawChar();
    char getOS_IndependentChar();
    char nextChar();

    bool isLetter(char ch) const;
    bool isHexDigit(char ch);

    void getWord();
    void getString();
    void getNumber();
    void getHexNumber();
    void getSpecial();
    void addKeywords();
    TokenCode getKeywordCode(const std::string &value);

    std::istream &reader_;
    TokenRecord tokenRecord_;
    bool inMultiLineComment_ = false;
    int lineNumber_ = 0;
    int columnNumber_ = 0;
    char ch_ = 0;
    ScannerError error_;
    std::unordered_map<std::string, TokenCode> tKeywords_;

    static constexpr int MAX_DIGIT_COUNT = 3;
    static constexpr int MAX_EXPONENT = 308;
    static constexpr char TAB = '\t';
    static constexpr char LF = '\n';
    static constexpr char CR = '\r';
};

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