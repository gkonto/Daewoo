#include "lexer.hpp"
#include "token.hpp"
#include "macros.hpp"

#include <stdexcept>
#include <limits>
#include <cmath>

Scanner::Scanner(std::istream &in)
    : reader_(in)
{
    startScanner();
    addKeywords();
}

void Scanner::startScanner()
{
    lineNumber_ = 1;
    columnNumber_ = 0;
    ch_ = nextChar();
}

char Scanner::readRawChar()
{
    if (reader_.eof())
    {
        return EOF;
    }
    else
    {
        ++columnNumber_;
        return reader_.get();
    }
}

char Scanner::getOS_IndependentChar()
{
    ch_ = readRawChar();
    if (ch_ == CR || ch_ == LF)
    {
        if (ch_ == CR)
        {
            ch_ = readRawChar();
            if (ch_ == LF)
            {
                return ch_;
            }
            else
            {
                error_.setError("expecting line feed character", lineNumber_, columnNumber_);
                return ch_;
            }
        }
        else
        {
            return ch_;
        }
    }
    else
    {
        return ch_;
    }
}

char Scanner::nextChar()
{
    // tokenWasLF_ = false;
    char ch = getOS_IndependentChar();
    if (ch == LF)
    {
        ++lineNumber_;
        columnNumber_ = 0;
        ch = ' ';
    }
    return ch;
}

void Scanner::skipMultiLineComment()
{
    inMultiLineComment_ = true;
    ch_ = nextChar();
    while (true)
    {
        while (ch_ != '*' && ch_ != EOF)
            ch_ = nextChar();

        if (ch_ == EOF)
            return;

        ch_ = nextChar();

        if (ch_ == '/')
        {
            ch_ = nextChar();
            inMultiLineComment_ = false;
            break;
        }
    }
}

void Scanner::skipSingleLineComment()
{
    while (ch_ != LF && ch_ != EOF)
        ch_ = getOS_IndependentChar();
    if (ch_ != EOF)
        ch_ = nextChar();
    columnNumber_ = 1;
    ++lineNumber_;
}

void Scanner::skipBlanksAndComments()
{
    while (ch_ == ' ' || ch_ == '\t' || ch_ == '/')
    {
        if (ch_ == ' ' || ch_ == '\t')
            ch_ = nextChar();
        else
        {
            if (reader_.peek() == '/' || reader_.peek() == '*')
            {
                ch_ = getOS_IndependentChar();
                if (ch_ == '/')
                    skipSingleLineComment();
                else if (ch_ == '*')
                    skipMultiLineComment();
            }
            else
            {
                break;
            }
        }
    }
}

void Scanner::getNumber()
{
    int singleDigit = 0;
    double scale = 0.;
    int evalue = 0;
    int exponentSign = 0;

    tokenRecord_.setInt(0);
    tokenRecord_.setFloat(0.);
    // Assume it's an integer
    tokenRecord_.setCode(TokenCode::tInteger);
    // check for decimal point just in case user has typed something like .5
    if (ch_ != '.')
    {
        while (true)
        {
            singleDigit = ch_ - '0';
            if (tokenRecord_.tInt() <= (std::numeric_limits<int>::max() - singleDigit) / 10)
            {
                tokenRecord_.setInt(10 * tokenRecord_.tInt() + singleDigit);
                ch_ = nextChar();
            }
            else
            {
                error_.setError("integer overflow, constant value too large to read", lineNumber_, columnNumber_);
                return;
            }

            if (!isdigit(ch_))
                break;
        }
    }
    scale = 1;
    if (ch_ == '.')
    {
        tokenRecord_.setCode(TokenCode::tFloat);
        tokenRecord_.setFloat(tokenRecord_.tInt());
        ch_ = nextChar();
        while (isdigit(ch_))
        {
            scale *= 0.1;
            singleDigit = ch_ - '0';
            tokenRecord_.setFloat(tokenRecord_.tFloat() + singleDigit * scale);
            ch_ = nextChar();
        }
    }
    exponentSign = 1;
    // Next check for scientific notation
    if (ch_ == 'e' || ch_ == 'E')
    {
        if (tokenRecord_.code() == TokenCode::tInteger)
        {
            tokenRecord_.setCode(TokenCode::tFloat);
            tokenRecord_.setFloat(tokenRecord_.tInt());
        }
        ch_ = nextChar();
        if (ch_ == '-' || ch_ == '+')
        {
            if (ch_ == '-')
                exponentSign = -1;
            ch_ = nextChar();
        }
        if (!isdigit(ch_))
        {
            error_.setError("syntax error: number expected in exponent", lineNumber_, columnNumber_);
            return;
        }

        evalue = 0;
        while (true)
        {
            singleDigit = ch_ - '0';
            if (evalue <= (MAX_EXPONENT - singleDigit) / 10)
            {
                evalue = 10 * evalue + singleDigit;
                ch_ = nextChar();
            }
            else
            {
                error_.setError("exponent overflow maximum value for exponent is " + std::to_string(MAX_EXPONENT), lineNumber_, columnNumber_);
                return;
            }
            if (!isdigit(ch_))
                break;
        }

        evalue *= exponentSign;
        if (tokenRecord_.code() == TokenCode::tInteger)
        {
            tokenRecord_.setFloat(tokenRecord_.tInt() * std::pow(10, evalue));
        }
        else
        {
            tokenRecord_.setFloat(tokenRecord_.tFloat() * std::pow(10.0, evalue));
        }
    }
}

void Scanner::getHexNumber()
{
    std::string astr;
    tokenRecord_.setInt(0);
    tokenRecord_.setCode(TokenCode::tInteger);
    ch_ = nextChar();
    while (isHexDigit(ch_))
    {
        astr += ch_;
        ch_ = nextChar();
    }
    tokenRecord_.setInt(std::stoi("$" + astr));
}

bool Scanner::isHexDigit(char ch)
{
    return (std::isdigit(ch) || (ch >= 'A' && ch <= 'F'));
}

void Scanner::getWord()
{
    tokenRecord_.setString("");
    while (isLetter(ch_) || isdigit(ch_))
    {
        tokenRecord_.setString(tokenRecord_.tString() + ch_);
        ch_ = nextChar();
    }

    tokenRecord_.setCode(getKeywordCode(tokenRecord_.tString()));
}

bool Scanner::isLetter(char ch) const
{
    return isalpha(ch) || ch == '_';
}

void Scanner::getString()
{
    tokenRecord_.setString("");
    tokenRecord_.setCode(TokenCode::tString);
    ch_ = nextChar();
    while (ch_ != EOF)
    {
        if (ch_ == '"')
        {
            ch_ = nextChar();
            return;
        }
        else
        {
            tokenRecord_.setString(tokenRecord_.tString() + ch_);
            ch_ = readRawChar();
        }
    }
    error_.setError("String without terminating quotation mark at line " + std::to_string(lineNumber_) + ", column ", lineNumber_, columnNumber_);
}

void Scanner::getSpecial()
{
    tokenRecord_.setChar(ch_);
    switch (ch_)
    {
    case '+':
        tokenRecord_.setCode(TokenCode::tPlus);
        break;
    case '^':
        tokenRecord_.setCode(TokenCode::tPower);
        break;
    case '(':
        tokenRecord_.setCode(TokenCode::tLeftParenthesis);
        break;
    case ')':
        tokenRecord_.setCode(TokenCode::tRightParenthesis);
        break;
    case '[':
        tokenRecord_.setCode(TokenCode::tLeftBracket);
        break;
    case ']':
        tokenRecord_.setCode(TokenCode::tRightBracket);
        break;
    case '{':
        tokenRecord_.setCode(TokenCode::tLeftCurleyBracket);
        break;
    case '}':
        tokenRecord_.setCode(TokenCode::tRightCurleyBracket);
        break;
    case '!':
    {
        if (reader_.peek() == '=')
        {
            ch_ = nextChar();
            tokenRecord_.setCode(TokenCode::tNotEqual);
        }
        else
        {
            error_.setError("unexpecting '=' character after explanation point: " + std::to_string(ch_), lineNumber_, columnNumber_);
        }
        break;
    }
    case '>':
    {
        if (reader_.peek() == '=')
        {
            ch_ = nextChar();
            tokenRecord_.setCode(TokenCode::tMoreThanOrEqual);
        }
        else
        {
            tokenRecord_.setCode(TokenCode::tMoreThan);
        }
        break;
    }
    case '<':
    {
        if (reader_.peek() == '=')
        {
            ch_ = nextChar();
            tokenRecord_.setCode(TokenCode::tLessThanOrEqual);
        }
        else
        {
            tokenRecord_.setCode(TokenCode::tLessThan);
        }
        break;
    }
    case '=':
    {
        if (reader_.peek() == '=')
        {
            ch_ = nextChar();
            tokenRecord_.setCode(TokenCode::tEquivalence);
        }
        else
        {
            tokenRecord_.setCode(TokenCode::tAssign);
        }
        break;
    }
    case ';':
        tokenRecord_.setCode(TokenCode::tSemicolon);
        break;
    case ':':
        tokenRecord_.setCode(TokenCode::tColon);
        break;
    case ',':
        tokenRecord_.setCode(TokenCode::tComma);
        break;
    case '.':
        tokenRecord_.setCode(TokenCode::tPeriod);
        break;
    case '-':
        tokenRecord_.setCode(TokenCode::tMinus);
        break;
    case '/':
        tokenRecord_.setCode(TokenCode::tDivide);
        break;
    case '*':
        tokenRecord_.setCode(TokenCode::tMult);
        break;
    case '@':
        tokenRecord_.setCode(TokenCode::tDotproduct);
        break;
    default:
        tokenRecord_.setCode(TokenCode::tError);
        break;
    }
    ch_ = nextChar();
}

void Scanner::nextToken()
{
    skipBlanksAndComments();
    tokenRecord_.setLineNumber(lineNumber_);
    tokenRecord_.setColumnNumber(columnNumber_);

    if (isLetter(ch_))
    {
        getWord();
    }
    else if (isdigit(ch_) || ch_ == '.')
    {
        getNumber();
    }
    else if (ch_ == '#')
    {
        getHexNumber();
    }
    else if (ch_ == '"')
    {
        getString();
    }
    else if (ch_ == EOF)
    {
        tokenRecord_.setCode(TokenCode::tEndofStream);
        if (inMultiLineComment_)
        {
            inMultiLineComment_ = false;
            error_.setError("detected unterminated comment, expecting '*/'", lineNumber_, columnNumber_);
        }
    }
    else
    {
        getSpecial();
    }
}

std::string Scanner::tokenToString(TokenCode code)
{
    switch (code)
    {
    case TokenCode::tIdentifier:
        return "identifier';// <' + tokenElement.FTokenString + '>";
    case TokenCode::tInteger:
        return "integer';// <' + inttostr (tokenElement.FTokenInteger) >";
    case TokenCode::tFloat:
        return "float';// + floattostr (tokenElement.FTokenFloat) + '>";
    case TokenCode::tString:
        return "string " + tokenRecord_.tString();
    case TokenCode::tMinus:
        return "character: '-'";
    case TokenCode::tPlus:
        return "character: '+'";
    case TokenCode::tMult:
        return "character: '*'";
    case TokenCode::tDivide:
        return "character: '/'";
    case TokenCode::tDotproduct:
        return "character: '@'";
    case TokenCode::tPower:
        return "character: '^'";
    case TokenCode::tRightParenthesis:
        return "character: ')'";
    case TokenCode::tLeftParenthesis:
        return "character: '('";
    case TokenCode::tRightBracket:
        return "character: ']'";
    case TokenCode::tLeftBracket:
        return "character: '['";
    case TokenCode::tLeftCurleyBracket:
        return "character: '{'";
    case TokenCode::tRightCurleyBracket:
        return "character: '}'";
    case TokenCode::tAssign:
        return "character: '='";
    case TokenCode::tEquivalence:
        return "spcharacterecial: '=='";
    case TokenCode::tMoreThan:
        return "character: '>'";
    case TokenCode::tLessThan:
        return "character: '<'";
    case TokenCode::tMoreThanOrEqual:
        return "character: '>='";
    case TokenCode::tLessThanOrEqual:
        return "character: '<='";
    case TokenCode::tApostrophy:
        return "character: Apostrphy";
    case TokenCode::tSemicolon:
        return "character: ';'";
    case TokenCode::tColon:
        return "character: ':'";
    case TokenCode::tComma:
        return "character: ','";
    case TokenCode::tPeriod:
        return "character: '.'";
    case TokenCode::tDollar:
        return "character: '$'";
    case TokenCode::tArrow:
        return "character:  '->'";
    case TokenCode::tEnd:
        return "key word: <end>";
    case TokenCode::tIf:
        return "key word: <if>";
    case TokenCode::tThen:
        return "key word: <then>";
    case TokenCode::tFor:
        return "Key word: <for>";
    case TokenCode::tTo:
        return "key word: <to>";
    case TokenCode::tDownTo:
        return "key word: <downto>";
    case TokenCode::tWhile:
        return "key word: <while>";
    case TokenCode::tDo:
        return "key word: <do>";
    case TokenCode::tElse:
        return "key word: <else>";
    case TokenCode::tRepeat:
        return "key word: <repeat>";
    case TokenCode::tUntil:
        return "key word: <until>";
    case TokenCode::tCase:
        return "key word: <case>";
    case TokenCode::tSwitch:
        return "key word: <switch>";
    case TokenCode::tReturn:
        return "key word: <return>";
    case TokenCode::tImport:
        return "key word: <import>";
    case TokenCode::tGlobal:
        return "key word: <global>";
    case TokenCode::tFunction:
        return "key word: <function>";
    case TokenCode::tBreak:
        return "key word: <break>";
    case TokenCode::tStep:
        return "key word: <step>";
    case TokenCode::tFalse:
        return "False";
    case TokenCode::tTrue:
        return "True";
    case TokenCode::tPrint:
        return "print";
    case TokenCode::tPrintln:
        return "println";
    case TokenCode::tEndofStream:
        return "End of Stream";
    case TokenCode::tLet:
        return "keyword <let>";
    case TokenCode::tNotEqual:
        return "character: '!='";
    case TokenCode::tAnd:
        return "character: '&&'";
    case TokenCode::tOr:
        return "character: '||'";
    case TokenCode::tNot:
        return "character: 'not'";
    case TokenCode::tError:
        return "error";
    }
    return "";
}

/* ------------------------------------------------ */

std::unordered_map<std::string, Token::Type>
    Lexer::keywords_ = {
        {"fn", Token::Type::Function},
        {"let", Token::Type::Let},
        {"true", Token::Type::True},
        {"false", Token::Type::False},
        {"if", Token::Type::If},
        {"else", Token::Type::Else},
        {"return", Token::Type::Return}};

static bool isLetter(char ch)
{
    return isalpha(ch) || ch == '_';
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

void Scanner::addKeywords()
{
    tKeywords_.emplace("let", TokenCode::tLet);
    tKeywords_.emplace("if", TokenCode::tIf);
    tKeywords_.emplace("do", TokenCode::tDo);
    tKeywords_.emplace("to", TokenCode::tTo);
    tKeywords_.emplace("or", TokenCode::tOr);
    // tKeywords_.emplace("in", TokenCode::tIn);

    tKeywords_.emplace("end", TokenCode::tEnd);
    tKeywords_.emplace("for", TokenCode::tFor);
    tKeywords_.emplace("and", TokenCode::tAnd);
    // tKeywords_.emplace("xor", TokenCode::tXor);
    tKeywords_.emplace("not", TokenCode::tNot);
    // tKeywords_.emplace("div", TokenCode::tDivI);
    //  tKeywords_.emplace("mod", TokenCode::tMod);
    //  tKeywords_.emplace("inc", TokenCode::tInc);
    //  tKeywords_.emplace("dec", TokenCode::tDec);

    tKeywords_.emplace("then", TokenCode::tThen);
    tKeywords_.emplace("else", TokenCode::tElse);
    tKeywords_.emplace("true", TokenCode::tTrue);
    tKeywords_.emplace("false", TokenCode::tFalse);
    tKeywords_.emplace("print", TokenCode::tPrint);
    tKeywords_.emplace("step", TokenCode::tStep);

    tKeywords_.emplace("while", TokenCode::tWhile);
    tKeywords_.emplace("until", TokenCode::tUntil);
    tKeywords_.emplace("break", TokenCode::tBreak);
    tKeywords_.emplace("println", TokenCode::tPrintln);
    // tKeywords_.emplace("setColor", TokenCode::tSetColor);

    tKeywords_.emplace("repeat", TokenCode::tRepeat);
    tKeywords_.emplace("downto", TokenCode::tDownTo);

    // tKeywords_.emplace("assertTrue", TokenCode::tAssertTrue);
    // tKeywords_.emplace("assertFalse", TokenCode::tAssertFalse);

    tKeywords_.emplace("fn", TokenCode::tFunction);
    tKeywords_.emplace("return", TokenCode::tReturn);
    tKeywords_.emplace("global", TokenCode::tGlobal);
    tKeywords_.emplace("import", TokenCode::tImport);
    // tKeywords_.emplace("ref", TokenCode::tRef);
    tKeywords_.emplace("switch", TokenCode::tSwitch);
    tKeywords_.emplace("case", TokenCode::tCase);
}

TokenCode Scanner::getKeywordCode(const std::string &value)
{
    auto it = tKeywords_.find(value);
    if (it != tKeywords_.end())
    {
        return it->second;
    }
    return TokenCode::tIdentifier;
}