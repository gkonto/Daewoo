#include "token.hpp"
#include <unordered_map>

std::string tokenToString(TokenCode code)
{
    switch (code)
    {
    case TokenCode::tIdentifier:
        return "identifier";
    case TokenCode::tInteger:
        return "integer";
    case TokenCode::tFloat:
        return "float";
    case TokenCode::tString:
        return "string \"";
    case TokenCode::tMinus:
        return "character: '-'";
    case TokenCode::tPlus:
        return "character: '+'";
        break;
    case TokenCode::tMult:
        return "character: '*'";
        break;
    case TokenCode::tDivide:
        return "character: '/'";
        break;
    case TokenCode::tPower:
        return "character: '^'";
        break;
    case TokenCode::tRightParenthesis:
        return "character: ')'";
        break;
    case TokenCode::tLeftParenthesis:
        return "character: '('";
        break;
    case TokenCode::tRightBracket:
        return "character: ']'";
        break;
    case TokenCode::tLeftBracket:
        return "character: '['";
        break;
    case TokenCode::tLeftCurleyBracket:
        return "character: '{'";
        break;
    case TokenCode::tRightCurleyBracket:
        return "character: '}'";
        break;
    case TokenCode::tAssign:
        return "character: '='";
        break;
    case TokenCode::tEquivalence:
        return "spcharacterecial: '=='";
        break;
    case TokenCode::tMoreThan:
        return "character: '>'";
        break;
    case TokenCode::tLessThan:
        return "character: '<'";
        break;
    case TokenCode::tMoreThanOrEqual:
        return "character: '>='";
        break;
    case TokenCode::tLessThanOrEqual:
        return "character: '<='";
        break;
    case TokenCode::tApostrophy:
        return "character: Apostrphy";
        break;
    case TokenCode::tSemicolon:
        return "character: ';'";
        break;
    case TokenCode::tColon:
        return "character: ':'";
        break;
    case TokenCode::tComma:
        return "character: ','";
        break;
    case TokenCode::tPeriod:
        return "character: '.'";
        break;
    case TokenCode::tDollar:
        return "character: '$'";
        break;
    case TokenCode::tArrow:
        return "character: '->'";
        break;
    case TokenCode::tEnd:
        return "key word: <end>";
        break;
    case TokenCode::tIf:
        return "key word: <if>";
        break;
    case TokenCode::tThen:
        return "key word: <then> ";
        break;
    case TokenCode::tFor:
        return "Key word: <for>";
        break;
    case TokenCode::tTo:
        return "key word: <to>";
        break;
    case TokenCode::tDownTo:
        return "key word: <downto>";
        break;
    case TokenCode::tWhile:
        return "key word: <while>";
        break;
    case TokenCode::tDo:
        return "key word: <do>";
        break;
    case TokenCode::tElse:
        return "key word: <else>";
        break;
    case TokenCode::tRepeat:
        return "key word: <repeat>";
        break;
    case TokenCode::tUntil:
        return "key word: <until>";
        break;
    case TokenCode::tCase:
        return "key word: <case>";
        break;
    case TokenCode::tSwitch:
        return "key word: <switch>";
        break;
    case TokenCode::tReturn:
        return "key word: <return>";
        break;
    case TokenCode::tImport:
        return "key word: <import>";
        break;
    case TokenCode::tGlobal:
        return "key word: <global>";
        break;
    case TokenCode::tFunction:
        return "key word: <function>";
        break;
    case TokenCode::tBreak:
        return "key word: <break>";
        break;
    case TokenCode::tStep:
        return "key word: <step>";
        break;
    case TokenCode::tFalse:
        return "False";
        break;
    case TokenCode::tTrue:
        return "True";
        break;
    case TokenCode::tPrint:
        return "print";
        break;
    case TokenCode::tPrintln:
        return "println";
        break;
    case TokenCode::tEndofStream:
        return "End of Stream";
        break;
    }
}

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

std::string TokenRecord::literal() const
{
    switch (code_)
    {
    case TokenCode::tIdentifier:
        return tString_;
    case TokenCode::tInteger:
        return std::to_string(tInt_);
    case TokenCode::tFloat:
        return std::to_string(tFloat_);
    case TokenCode::tString:
        return tString_;
    case TokenCode::tMinus:
        return "-";
    case TokenCode::tPlus:
        return "+";
    case TokenCode::tMult:
        return " *";
    case TokenCode::tDivide:
        return "/";
    case TokenCode::tDotproduct:
        return "@";
    case TokenCode::tPower:
        return "^";
    case TokenCode::tRightParenthesis:
        return ")";
    case TokenCode::tLeftParenthesis:
        return "(";
    case TokenCode::tRightBracket:
        return "]";
    case TokenCode::tLeftBracket:
        return "[";
    case TokenCode::tLeftCurleyBracket:
        return "{";
    case TokenCode::tRightCurleyBracket:
        return "}";
    case TokenCode::tAssign:
        return "=";
    case TokenCode::tNotEqual:
        return "!=";
    case TokenCode::tEquivalence:
        return "==";
    case TokenCode::tMoreThan:
        return ">";
    case TokenCode::tLessThan:
        return "<";
    case TokenCode::tMoreThanOrEqual:
        return ">=";
    case TokenCode::tLessThanOrEqual:
        return "<=";
    case TokenCode::tApostrophy:
        return "Apostrphy";
    case TokenCode::tSemicolon:
        return ";";
    case TokenCode::tColon:
        return ":";
    case TokenCode::tComma:
        return ",";
    case TokenCode::tPeriod:
        return ".";
    case TokenCode::tDollar:
        return "$";
    case TokenCode::tArrow:
        return "->";
    case TokenCode::tEnd:
        return "<end>";
    case TokenCode::tIf:
        return "<if>";
    case TokenCode::tThen:
        return "<then>";
    case TokenCode::tFor:
        return "<for>";
    case TokenCode::tTo:
        return "<to>";
    case TokenCode::tDownTo:
        return "<downto>";
    case TokenCode::tWhile:
        return "<while>";
    case TokenCode::tDo:
        return "<do>";
    case TokenCode::tElse:
        return "<else>";
    case TokenCode::tRepeat:
        return "<repeat>";
    case TokenCode::tUntil:
        return "<until>";
    case TokenCode::tCase:
        return "<case>";
    case TokenCode::tSwitch:
        return "<switch>";
    case TokenCode::tReturn:
        return "<return>";
    case TokenCode::tImport:
        return "<import>";
    case TokenCode::tGlobal:
        return "<global>";
    case TokenCode::tFunction:
        return "<fn>";
    case TokenCode::tBreak:
        return "<break>";
    case TokenCode::tStep:
        return "<step>";
    case TokenCode::tFalse:
        return "<false>";
    case TokenCode::tTrue:
        return "<true>";
    case TokenCode::tPrint:
        return "print";
    case TokenCode::tPrintln:
        return "println";
    case TokenCode::tEndofStream:
        return "<eof>";
    case TokenCode::tError:
        return "Unrecognized characher";
    case TokenCode::tLet:
        return "<let>";
    case TokenCode::tOr:
        return "<or>";
    case TokenCode::tNot:
        return "<not>";
    case TokenCode::tAnd:
        return "<and>";
    }
    return "";
}