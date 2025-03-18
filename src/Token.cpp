#include <unordered_map>
#include "Token.hpp"

std::string tokenToString(TokenCode code) {
    switch (code) {
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
        case TokenCode::tMult:
            return "character: '*'";
        case TokenCode::tDivide:
            return "character: '/'";
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
            return "character: '->'";
        case TokenCode::tEnd:
            return "key word: <end>";
        case TokenCode::tIf:
            return "key word: <if>";
        case TokenCode::tThen:
            return "key word: <then> ";
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
    }
}

Token::Token(Type type, BufferSize sp, Length l)
    : type_(type), position_(sp), length_(l) {}

std::string tokenType2String(Token::Type type) {
    switch (type) {
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

std::string TokenRecord::literal() const {
    switch (code_) {
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