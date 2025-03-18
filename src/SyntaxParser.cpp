#include "SyntaxParser.hpp"
#include "lexer.hpp"

SyntaxParser::SyntaxParser(Scanner &sc)
    : sc_(sc) {}

std::optional<SyntaxError> SyntaxParser::syntaxCheck() {
    tokenVector_.clearCode();
    nextToken();

    if (tokenVector_.token().code() != TokenCode::tEndofStream) return statementList();

    return std::nullopt;
}

// statementList = statement { [ ; ] statement }
std::optional<SyntaxError> SyntaxParser::statementList() {
    auto err = statement();
    if (err.has_value()) return err;

    while (true) {
        if (tokenVector_.token().code() == TokenCode::tSemicolon) {
            err = expect(TokenCode::tSemicolon);
            if (err) return err;
        }

        // all the following tokens can and a statement.
        if (tokenVector_.token().code() == TokenCode::tEnd || tokenVector_.token().code() == TokenCode::tElse ||
            tokenVector_.token().code() == TokenCode::tEndofStream) {
            return std::nullopt;
        }

        err = statement();
        if (err.has_value()) return err;
    }
}

// statement = exprStatement | ifStatement | returnStatment | function | letStatement
std::optional<SyntaxError> SyntaxParser::statement() {
    std::optional<SyntaxError> err;

    auto tokenCode = tokenVector_.token().code();
    if (tokenCode == TokenCode::tIf) {
        err = ifStatement();
    } else if (tokenCode == TokenCode::tReturn) {
        return err = returnStatement();
    } else if (tokenCode == TokenCode::tFunction) {
        err = parseUserDefinedFunction();
    } else if (tokenCode == TokenCode::tEnd || tokenCode == TokenCode::tEndofStream) {
        return err;
    } else if (tokenCode == TokenCode::tLet) {
        err = letStatement();
    } else {
        err = expressionStatement();
    }
    return err;
}

// Expect works in the following way. If the function finds the expected
// token then it return nil indicating success.
// If it fails to find the token it builds a special Error node and returns that.
// The error node contains some useful information. If a caller receives a non-nil
// node it can safely assume that the node is the error node. When that happens it
// should free up any partially build parts of the ast tree, then return the error node
// to the next caller. This means that the error node will get inserted into the ast tree
// At compilation, if an error node is encountered the compiler can report the error at
// that point in time.
std::optional<SyntaxError> SyntaxParser::expect(TokenCode tc) {
    if (tokenVector_.token().code() != tc) {
        if (error_.has_value()) {
            return SyntaxError("expecting token: '" + tokenToString(tc) + "' got: '" +
                               tokenToString(tokenVector_.token().code()) + "'");
        }
    } else {
        nextToken();
    }
    return std::nullopt;
}

void SyntaxParser::nextToken() {
    sc_.nextToken();
    tokenVector_.add(sc_.token());
}

// exprStatement = expression '=' expression
// There are however some restrictions on the left-hand expression

// Left-hand sides can only be:
// ident '='
// ident '[' expression ']' '='

// They cannot be expressions such as 1 + 5, a + b, etc
// A valid left-hand side will always, when parsed, appear to have load or lvecIdx as the last bytecode

// We first read the left-side into a local program code space, if there
// is no '=' then we exit the method, else we also parse the right-hand side.
// We now append the code for the left-hand side on to the right-hand code
// and patch the last byte code to be either store or svecIdx

// This approach will allow us to identify illegal left-hand sides
// as well as permit indices for lists to be of any complexity

// AST:
// (exprStatement) -> (identifier) and (expression)
// OR (exprStatement) -> (expression)
std::optional<SyntaxError> SyntaxParser::expressionStatement() {
    auto err = expression();
    if (err) return err;

    if (tokenVector_.token().code() == TokenCode::tAssign) {
        nextToken();
        err = expression();
        if (err) return err;
    }

    return std::nullopt;
}

// expression = simpleExpression | simpleExpression relationalOp simpleExpression
std::optional<SyntaxError> SyntaxParser::relationalOperators() {
    auto err = simpleExpression();
    if (err) return err;

    while (true) {
        auto tc = tokenVector_.token().code();
        if (tc == TokenCode::tLessThan || tc == TokenCode::tLessThanOrEqual || tc == TokenCode::tMoreThan ||
            tc == TokenCode::tMoreThanOrEqual || tc == TokenCode::tNotEqual || tc == TokenCode::tEquivalence) {
            nextToken();
            err = simpleExpression();
            if (err) return err;
        } else {
            break;
        }
    }
    return std::nullopt;
}

// expression = simpleExpression | simpleExpression relationalOp simpleExpression
std::optional<SyntaxError> SyntaxParser::expression() {
    auto err = relationalOperators();

    while (tokenVector_.token().code() == TokenCode::tOr || tokenVector_.token().code() == TokenCode::tAnd) {
        nextToken();
        err = relationalOperators();
        if (err) return err;
    }
    return std::nullopt;
}

// expression = term { ('+' | '-' | MOD | DIV) power }
std::optional<SyntaxError> SyntaxParser::simpleExpression() {
    auto err = term();

    while (tokenVector_.token().code() == TokenCode::tPlus || tokenVector_.token().code() == TokenCode::tMinus) {
        nextToken();
        err = term();
        if (err) return err;
    }
    return err;
}

// term = power { ('*', '/', MOD, DIV) power }
std::optional<SyntaxError> SyntaxParser::term() {
    auto err = power();
    if (err) return err;

    while (tokenVector_.token().code() == TokenCode::tMult || tokenVector_.token().code() == TokenCode::tDivide) {
        nextToken();
        err = power();
        if (err) return err;
    }
    return std::nullopt;
}

std::optional<SyntaxError> SyntaxParser::power() {
    while (tokenVector_.token().code() == TokenCode::tMinus || tokenVector_.token().code() == TokenCode::tPlus) {
        if (tokenVector_.token().code() == TokenCode::tPlus) {
            return SyntaxError("Too many plus symbols");
        }
        nextToken();
    }

    auto err = primary();
    if (err) return err;

    if (tokenVector_.token().code() == TokenCode::tPower) {
        nextToken();
        err = power();
        if (err) return err;
    }
    return std::nullopt;
}

// primary => factor primaryPlus
std::optional<SyntaxError> SyntaxParser::primary() {
    auto err = factor();
    if (err) return err;

    err = primaryPlus();
    if (err) return err;

    return std::nullopt;
}

std::optional<SyntaxError> SyntaxParser::factor() {
    std::optional<SyntaxError> err;
    switch (tokenVector_.token().code()) {
        case (TokenCode::tInteger):
            nextToken();
            break;
        case (TokenCode::tFloat):
            nextToken();
            break;
        case (TokenCode::tIdentifier):
            nextToken();
            break;
        case (TokenCode::tString):
            nextToken();
            break;
        case (TokenCode::tFalse):
            nextToken();
            break;
        case (TokenCode::tTrue):
            nextToken();
            break;
        case (TokenCode::tLeftParenthesis): {
            nextToken();
            err = expression();
            if (err) return err;
            err = expect(TokenCode::tRightParenthesis);
            if (err) return err;
            break;
        }
        case (TokenCode::tNot): {
            nextToken();
            auto node = expression();
            if (err) {
                return err;
            }

            break;
        }
        case (TokenCode::tError):
            err = SyntaxError("Expecting a factor [literal, identifier]");
            break;
        default:
            err = SyntaxError("Expecting a factor [literal, identifier]");
            break;
    }
    return err;
}

std::optional<SyntaxError> SyntaxParser::primaryPlus() {
    if (tokenVector_.token().code() == TokenCode::tPeriod) {
        nextToken();
        auto err = expect(TokenCode::tIdentifier);
        if (err) return err;

        err = primaryPlus();
        if (err) return err;
    } else if (tokenVector_.token().code() == TokenCode::tLeftParenthesis) {
        nextToken();
        auto err = parseFunctionCall();
        if (err) return err;

        err = primaryPlus();
        if (err) return err;
    }
    return std::nullopt;
}

// ifStatement = IF expression THEN statement ifEnd
// ifEnd = END | ELSE statementList END
// AST:
// (if) -> (condition) and (thenStatementList) and (elseStatementList)
std::optional<SyntaxError> SyntaxParser::ifStatement() {
    auto err = expect(TokenCode::tIf);
    if (err) return err;

    err = expression();
    if (err) return err;

    err = expect(TokenCode::tThen);
    if (err) return err;

    err = statementList();
    if (err) return err;

    if (tokenVector_.token().code() == TokenCode::tElse) {
        nextToken();
        err = statementList();
        if (err) return err;

        err = expect(TokenCode::tEnd);
        if (err) return err;
    } else {
        err = expect(TokenCode::tEnd);
        if (err) return err;
    }
    return std::nullopt;
}

// returnStatement = RETURN expression
std::optional<SyntaxError> SyntaxParser::returnStatement() {
    if (!inUserFunctionParsing_) {
        return SyntaxError("You cannot use a return statement outside a user function");
    }

    auto err = expect(TokenCode::tReturn);
    if (err) return err;

    err = expression();
    if (err) return err;

    return std::nullopt;
}

// letStatement = "let" identifier '=' expression
std::optional<SyntaxError> SyntaxParser::letStatement() {
    auto err = expect(TokenCode::tLet);
    if (err) return err;

    err = variable();
    if (err) return err;

    err = expect(TokenCode::tAssign);
    if (err) return err;

    err = expression();
    if (err) return err;

    return std::nullopt;
}

// function = function identifier '(' argumentList ')' statementList
std::optional<SyntaxError> SyntaxParser::parseUserDefinedFunction() {
    std::optional<SyntaxError> err;
    std::string functionName;

    nextToken();
    if (tokenVector_.token().code() == TokenCode::tIdentifier) {
        functionName = tokenVector_.token().tString();
    }

    globalVariableList.clear();
    enterUserFunctionScope();
    nextToken();
    if (tokenVector_.token().code() == TokenCode::tLeftParenthesis) {
        nextToken();
        err = functionArgumentList();
        if (err) return err;

        err = expect(TokenCode::tRightParenthesis);
        if (err) return err;
    }
    err = statementList();
    if (err) return err;

    exitUserFunctionScope();
    globalVariableList.clear();
    err = expect(TokenCode::tEnd);
    return err;
}

// argumentList = argument { ',' argument }
// AST:
// nodeList -> (arg) and (arg) and (arg) and ....
std::optional<SyntaxError> SyntaxParser::functionArgumentList() {
    if (tokenVector_.token().code() == TokenCode::tIdentifier) {
        auto err = functionArgument();
        if (err) return err;
    }
    while (tokenVector_.token().code() == TokenCode::tComma) {
        nextToken();
        auto err = functionArgument();
        if (err) return err;
    }
    return std::nullopt;
}

// argument = identifier | REF identifier
std::optional<SyntaxError> SyntaxParser::functionArgument() {
    return variable();
}

// Parse a function argument in a function definition
std::optional<SyntaxError> SyntaxParser::variable() {
    if (tokenVector_.token().code() != TokenCode::tIdentifier) {
        return SyntaxError("expecting identifier in function argument definition");
    } else {
        nextToken();
    }
    return std::nullopt;
}

std::optional<SyntaxError> SyntaxParser::parseFunctionCall() {
    if (tokenVector_.token().code() != TokenCode::tRightParenthesis) {
        auto err = expressionList();
        if (err) return err;
    }

    auto err = expect(TokenCode::tRightParenthesis);
    if (err) return err;

    return std::nullopt;
}

// argumentList = expression { ',' expression }
// Returns the number of expressions that were parsed
std::optional<SyntaxError> SyntaxParser::expressionList() {
    auto err = expression();
    if (err) return err;

    while (tokenVector_.token().code() == TokenCode::tComma) {
        nextToken();
        err = expression();
        if (err) return err;
    }

    return std::nullopt;
}