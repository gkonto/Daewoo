#ifndef TOKEN_HPP_INCLUDED
#define TOKEN_HPP_INCLUDED

#include <string>
#include "defs.hpp"

enum class TokenCode
{
    /*0*/ tIdentifier,
    tFloat,
    tInteger,
    tString,
    tPlus,
    tMinus,
    tMult,
    tDotproduct,
    tDivide,
    tPower,
    /*10*/ tLessThan,
    tLessThanOrEqual,
    tMoreThan,
    tMoreThanOrEqual,
    tNotEqual,
    tRightParenthesis,
    tLeftParenthesis,
    tLeftBracket,
    tRightBracket,
    tLeftCurleyBracket,
    /*20*/ tRightCurleyBracket,
    tAssign,
    tEquivalence,
    tApostrophy,
    tDollar,
    tSemicolon,
    tColon,
    tComma,
    tPeriod,
    tArrow,
    /*30*/ tAnd,
    tOr,
    tNot,
    tError, // used when an error is detected
    tEnd,
    tEndofStream,
    tPrint,
    tPrintln,
    tIf,
    tThen,
    /*40*/ tElse,
    tFalse,
    tTrue,
    tFor,
    tDo,
    tTo,
    tDownTo,
    tStep,
    tWhile,
    tRepeat,
    /*50*/ tUntil,
    tBreak,
    tFunction,
    tGlobal,
    tSwitch,
    tCase,
    tImport,
    tReturn,
    tLet
};

class TokenRecord
{
public:
    TokenCode code() const { return code_; }
    void setCode(TokenCode c) { code_ = c; }

    std::string literal() const;

    template <typename T>
    void setString(T &&v) { tString_ = std::forward<std::string>(v); }
    const std::string &tString() const { return tString_; }

    void setFloat(double v) { tFloat_ = v; }
    double tFloat() const { return tFloat_; }

    void setInt(int v) { tInt_ = v; }
    int tInt() const { return tInt_; }
    double tIntAsDouble() const { return tInt_; }
    void setChar(char v) { tChar_ = v; }
    char tChar() const { return tChar_; }

    void setColumnNumber(int v) { columnNumber_ = v; }
    int columnNumber() const { return columnNumber_; }

    void setLineNumber(int v) { lineNumber_ = v; }
    int lineNumber() const { return lineNumber_; }

private:
    TokenCode code_;
    int lineNumber_;
    int columnNumber_;

    double tFloat_;
    int tInt_;
    char tChar_;
    std::string tString_;
};

std::string tokenToString(TokenCode code);

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