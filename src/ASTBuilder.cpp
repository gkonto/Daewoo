#include "ASTBuilder.hpp"
#include <memory>
#include <stdexcept>

#include "ASTNode.hpp"
#include "ASTNodeTypes.hpp"

ASTBuilder::ASTBuilder(TokensTable &ttable)
    : sc_(ttable) {}

// program = statementList
std::unique_ptr<ASTProgram> ASTBuilder::build() {
    sc_.reset();
    sc_.setMode(TokensTable::Mode::Reading);
    nextToken();

    if (code() != TokenCode::tEndofStream) {
        return std::make_unique<ASTProgram>(statementList());
    }

    return nullptr;
}

// statementList = statement { [ ';' ] statement }
std::unique_ptr<ASTNodeList> ASTBuilder::statementList() {
    auto stat = std::make_unique<ASTNodeList>(ASTNodeType::ntStatementList);
    stat->add(statement());

    while (true) {
        if (code() == TokenCode::tSemicolon) expect(TokenCode::tSemicolon);

        if (code() == TokenCode::tEnd || code() == TokenCode::tElse || code() == TokenCode::tEndofStream) {
            return stat;
        }
        stat->add(statement());
    }
    return stat;
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
std::unique_ptr<ASTNode> ASTBuilder::expect(TokenCode tc) {
    if (code() != tc) {
        return std::make_unique<ASTErrorNode>("expecting " + tokenToString(tc), lineNumber(), columnNumber());
    } else {
        nextToken();
    }
    return nullptr;
}

// statement = assignment | endOfStream | exprStatement | ifStatement | returnStatement | function | letStatement
std::unique_ptr<ASTNode> ASTBuilder::statement() {
    switch (code()) {
        case TokenCode::tIf:
            return ifStatement();
        case TokenCode::tReturn:
            return returnStmt();
        case TokenCode::tFunction:
            return parseUserDefinedFunction();
        case TokenCode::tEnd:
            return nullptr;
        case TokenCode::tEndofStream:
            return nullptr;
        case TokenCode::tLet:
            return letStatement();
        default:
            return exprStatement();
    }
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
std::unique_ptr<ASTNode> ASTBuilder::exprStatement() {
    auto node = expression();
    if (code() == TokenCode::tAssign) {
        nextToken();
        auto expressionNode = std::make_unique<ASTExpression>(expression(), lineNumber());
        // if node.nodeType <> ntPrimary then
        if (node->type() != ASTNodeType::ntPrimary) {
            return std::make_unique<ASTErrorNode>("Expecting an identifier on the left-hand side", lineNumber(),
                                                  columnNumber());
        }

        return nullptr;  // temp
        // return std::make_unique<ASTAssignment>(std::move(node), std::move(expressionNode), lineNumber());
    } else {
        return std::make_unique<ASTExpressionStatement>(std::move(node), lineNumber());
    }
}

// ifStatement = IF expression THEN statement ifEnd
// ifEnd = END | ELSE statementList END
// AST:
// (if) -> (condition) and (thenStatementList) and (elseStatementList)
std::unique_ptr<ASTNode> ASTBuilder::ifStatement() {
    expect(TokenCode::tIf);
    auto condition = expression();
    expect(TokenCode::tThen);
    auto listOfStatements = statementList();

    if (code() == TokenCode::tElse) {
        nextToken();
        auto listOfElseStatements = statementList();
        expect(TokenCode::tEnd);
        return std::make_unique<ASTIf>(std::move(condition), std::move(listOfStatements),
                                       std::move(listOfElseStatements), lineNumber());
    } else {
        expect(TokenCode::tEnd);
        return std::make_unique<ASTIf>(std::move(condition), std::move(listOfStatements), nullptr, lineNumber());
    }
}

// letStatement = "let" identifier '=' expression
std::unique_ptr<ASTNode> ASTBuilder::letStatement() {
    expect(TokenCode::tLet);
    auto lhs = variable();
    expect(TokenCode::tAssign);
    auto rhs = expression();

    return std::make_unique<ASTLetStatement>(std::move(lhs), std::move(rhs), lineNumber());
}

std::unique_ptr<ASTNode> ASTBuilder::returnStmt() {
    if (!inUserFunctionParsing) {
        return std::make_unique<ASTErrorNode>("You cannot use a return statement outside a user function", lineNumber(),
                                              columnNumber());
    }
    expect(TokenCode::tReturn);
    return std::make_unique<ASTReturn>(std::make_unique<ASTExpression>(expression(), lineNumber()), lineNumber());
}

// function = function identifier '(' argumentList ')' statementList
std::unique_ptr<ASTNode> ASTBuilder::parseUserDefinedFunction() {
    std::string functionName;
    nextToken();
    if (code() == TokenCode::tIdentifier) {
        functionName = sc_.token().tString();
    } else {
        return std::make_unique<ASTErrorNode>("expecting function name", lineNumber(), columnNumber());
    }

    globalVariableList.clear();
    enterUserFunctionScope();
    nextToken();
    std::unique_ptr<ASTNodeList> argList;

    if (code() == TokenCode::tLeftParenthesis) {
        nextToken();
        argList = functionArgumentList();
        expect(TokenCode::tRightParenthesis);
    }

    auto statementListNode = statementList();
    exitUserFunctionScope();
    globalVariableList.clear();
    expect(TokenCode::tEnd);

    return std::make_unique<ASTUserFunction>(std::string(""), functionName, std::move(argList),
                                             std::move(statementListNode), lineNumber());
}

// expression = simpleExpression | simpleExpression relationalOp simpleExpression
std::unique_ptr<ASTNode> ASTBuilder::expression() {
    auto leftNode = relationalOperators();
    while (code() == TokenCode::tOr || code() == TokenCode::tAnd) {
        TokenCode op = code();
        nextToken();
        auto rightNode = relationalOperators();
        if (op == TokenCode::tOr) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntOR,
                                                  lineNumber());
        } else if (op == TokenCode::tAnd) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntAND,
                                                  lineNumber());
        }
    }
    return leftNode;
}

// argumentList = argument { ',' argument }
// AST:
// nodeList -> (arg) and (arg) and (arg) and ....
std::unique_ptr<ASTNodeList> ASTBuilder::functionArgumentList() {
    auto alist = std::make_unique<ASTNodeList>(ASTNodeType::ntNodeList);
    if (code() == TokenCode::tIdentifier) {
        alist->add(functionArgument());
    }

    while (code() == TokenCode::tComma) {
        nextToken();
        alist->add(functionArgument());
    }

    return alist;
}

// argument = identifier | REF identifier
std::unique_ptr<ASTNode> ASTBuilder::functionArgument() {
    return variable();
}

// Parse a function argument in a function definition
std::unique_ptr<ASTIdentifier> ASTBuilder::variable() {
    auto ret = std::make_unique<ASTIdentifier>(sc_.token().tString(), lineNumber());
    nextToken();
    return ret;
}

// expression = simpleExpression | simpleExpression relationalOp simpleExpression
std::unique_ptr<ASTNode> ASTBuilder::relationalOperators() {
    auto leftNode = simpleExpression();
    while (code() == TokenCode::tLessThan || code() == TokenCode::tLessThanOrEqual || code() == TokenCode::tMoreThan
           || code() == TokenCode::tMoreThanOrEqual || code() == TokenCode::tNotEqual
           || code() == TokenCode::tEquivalence) {
        auto op = code();
        nextToken();
        auto rightNode = simpleExpression();

        if (op == TokenCode::tEquivalence) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntEQ,
                                                  lineNumber());
        } else if (op == TokenCode::tLessThan) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntLT,
                                                  lineNumber());
        } else if (op == TokenCode::tMoreThan) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntGT,
                                                  lineNumber());
        } else if (op == TokenCode::tMoreThanOrEqual) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntGE,
                                                  lineNumber());
        } else if (op == TokenCode::tLessThanOrEqual) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntLE,
                                                  lineNumber());
        } else if (op == TokenCode::tNotEqual) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntNE,
                                                  lineNumber());
        }
    }
    return leftNode;
}

// expression = term { ('+' | '-' | MOD | DIV) power }
std::unique_ptr<ASTNode> ASTBuilder::simpleExpression() {
    auto leftNode = term();
    while (code() == TokenCode::tPlus || code() == TokenCode::tMinus) {
        auto op = code();
        nextToken();
        auto rightNode = term();
        if (op == TokenCode::tPlus) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntAdd,
                                                  lineNumber());
        } else {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntSub,
                                                  lineNumber());
        }
    }
    return leftNode;
}

// term = power { ('*', '/', MOD, DIV) power }
std::unique_ptr<ASTNode> ASTBuilder::term() {
    auto leftNode = power();
    while (code() == TokenCode::tMult || code() == TokenCode::tDivide) {
        auto op = code();
        nextToken();
        auto rightNode = power();

        if (op == TokenCode::tMult) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntMult,
                                                  lineNumber());
        } else if (op == TokenCode::tDivide) {
            leftNode = std::make_unique<ASTBinOp>(std::move(leftNode), std::move(rightNode), ASTNodeType::ntDiv,
                                                  lineNumber());
        }
    }
    return leftNode;
}

// power = {'+' | '-'} factor [ '^' power ]
std::unique_ptr<ASTNode> ASTBuilder::power() {
    int unaryMinus_count = 0;
    while (code() == TokenCode::tMinus || code() == TokenCode::tPlus) {
        if (code() == TokenCode::tMinus) {
            ++unaryMinus_count;
        }
        nextToken();
    }

    auto leftNode = primary();

    if (code() == TokenCode::tPower) {
        nextToken();
        auto rightNode = power();
        leftNode = std::make_unique<ASTPowerOp>(std::move(leftNode), std::move(rightNode), lineNumber());
    }

    for (int i = 0; i < unaryMinus_count; ++i) {
        leftNode = std::make_unique<ASTUniOp>(std::move(leftNode), ASTNodeType::ntUnaryMinus, lineNumber());
    }
    return leftNode;
}

// primary => factor primaryPlus
std::unique_ptr<ASTNode> ASTBuilder::primary() {
    return std::make_unique<ASTPrimary>(factor(), primaryPlus(), lineNumber());
}

// factor = '(' expression ')' | variable | number | string | NOT factor | functionCall
std::unique_ptr<ASTNode> ASTBuilder::factor() {
    if (code() == TokenCode::tInteger) {
        auto result = std::make_unique<ASTInteger>(token().tInt(), lineNumber());
        nextToken();
        return result;
    } else if (code() == TokenCode::tFloat) {
        auto result = std::make_unique<ASTFloat>(token().tFloat(), lineNumber());
        nextToken();
        return result;
    } else if (code() == TokenCode::tIdentifier) {
        auto result = std::make_unique<ASTIdentifier>(token().tString(), lineNumber());
        nextToken();
        return result;
    } else if (code() == TokenCode::tString) {
        auto result = std::make_unique<ASTString>(token().tString(), lineNumber());
        nextToken();
        return result;
    } else if (code() == TokenCode::tNot) {
        nextToken();
        auto node = expression();
        if (node->type() == ASTNodeType::ntError) {
            return node;
        }
        return std::make_unique<ASTNotOp>(std::move(node), lineNumber());
    } else if (code() == TokenCode::tFalse) {
        auto result = std::make_unique<ASTBoolean>(false, lineNumber());
        nextToken();
        return result;
    } else if (code() == TokenCode::tTrue) {
        auto result = std::make_unique<ASTBoolean>(true, lineNumber());
        nextToken();
        return result;
    } else if (code() == TokenCode::tNot) {
        nextToken();
        auto node = expression();
        if (node->type() == ASTNodeType::ntError) {
            return node;
        }
        return std::make_unique<ASTNotOp>(std::move(node), lineNumber());
    } else if (code() == TokenCode::tLeftParenthesis) {
        nextToken();
        auto result = expression();
        expect(TokenCode::tRightParenthesis);
        return result;
    } else if (code() == TokenCode::tLeftCurleyBracket) {
        throw std::runtime_error("Not implemented");
    } else if (code() == TokenCode::tError) {
        return std::make_unique<ASTErrorNode>("Expecting a factor...", lineNumber(), columnNumber());
    } else {
        return std::make_unique<ASTErrorNode>("Expecting a factor...", lineNumber(), columnNumber());
    }
}

std::unique_ptr<ASTNode> ASTBuilder::primaryPlus() {
    if (code() == TokenCode::tLeftParenthesis) {
        nextToken();
        return std::make_unique<ASTPrimaryFunction>(parseFunctionCall(), primaryPlus(), lineNumber());
    } else {
        return std::make_unique<ASTNull>();
    }
}

std::unique_ptr<ASTNodeList> ASTBuilder::parseFunctionCall() {
    std::unique_ptr<ASTNodeList> result = nullptr;
    if (code() != TokenCode::tRightParenthesis) {
        result = expressionList();
    }

    expect(TokenCode::tRightParenthesis);
    if (result == nullptr) {
        result = std::make_unique<ASTNodeList>(ASTNodeType::ntNodeList);
    }
    return result;
}

// argumentList = expression { ',' expression }
// Returns the number of expressions that were parsed
std::unique_ptr<ASTNodeList> ASTBuilder::expressionList() {
    auto nodeList = std::make_unique<ASTNodeList>(ASTNodeType::ntNodeList);
    nodeList->add(expression());

    while (code() == TokenCode::tComma) {
        nextToken();
        nodeList->add(expression());
    }
    return nodeList;
}