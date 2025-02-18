#include "parser.hpp"
#include <memory>
#include <sstream>

#include "ast.hpp"

Parser::Parser(Lexer &l)
    : lexer_(l)
{
    nextToken();
    nextToken();

    registerPrefix(Token::Type::Ident, [this]()
                   { return parseIdentifier(); });
    registerPrefix(Token::Type::Int, [this]()
                   { return parseIntegerLiteral(); });
    registerPrefix(Token::Type::Bang, [this]()
                   { return parsePrefixExpression(); });
    registerPrefix(Token::Type::Minus, [this]()
                   { return parsePrefixExpression(); });
    registerPrefix(Token::Type::True, [this]()
                   { return parseBoolean(); });
    registerPrefix(Token::Type::False, [this]()
                   { return parseBoolean(); });
    registerPrefix(Token::Type::LParen, [this]()
                   { return parseGroupedExpression(); });
    registerPrefix(Token::Type::If, [this]()
                   { return parseIfExpression(); });
    registerPrefix(Token::Type::Function, [this]()
                   { return parseFunctionLiteral(); });

    registerInfix(Token::Type::Plus, [this](std::unique_ptr<Expression> lhs)
                  { return parseInfixExpression(std::move(lhs)); });
    registerInfix(Token::Type::Minus, [this](std::unique_ptr<Expression> lhs)
                  { return parseInfixExpression(std::move(lhs)); });
    registerInfix(Token::Type::Slash, [this](std::unique_ptr<Expression> lhs)
                  { return parseInfixExpression(std::move(lhs)); });
    registerInfix(Token::Type::Asterisk, [this](std::unique_ptr<Expression> lhs)
                  { return parseInfixExpression(std::move(lhs)); });
    registerInfix(Token::Type::EQ, [this](std::unique_ptr<Expression> lhs)
                  { return parseInfixExpression(std::move(lhs)); });
    registerInfix(Token::Type::NotEQ, [this](std::unique_ptr<Expression> lhs)
                  { return parseInfixExpression(std::move(lhs)); });
    registerInfix(Token::Type::LessThan, [this](std::unique_ptr<Expression> lhs)
                  { return parseInfixExpression(std::move(lhs)); });
    registerInfix(Token::Type::GreaterThan, [this](std::unique_ptr<Expression> lhs)
                  { return parseInfixExpression(std::move(lhs)); });
    registerInfix(Token::Type::LParen, [this](std::unique_ptr<Expression> lhs)
                  { return parseCallExpression(std::move(lhs)); });

    registerPrecedence(Token::Type::EQ, Precedence::Equals);
    registerPrecedence(Token::Type::NotEQ, Precedence::Equals);
    registerPrecedence(Token::Type::LessThan, Precedence::LessGreater);
    registerPrecedence(Token::Type::GreaterThan, Precedence::LessGreater);
    registerPrecedence(Token::Type::Plus, Precedence::Sum);
    registerPrecedence(Token::Type::Minus, Precedence::Sum);
    registerPrecedence(Token::Type::Slash, Precedence::Product);
    registerPrecedence(Token::Type::Asterisk, Precedence::Product);
    registerPrecedence(Token::Type::LParen, Precedence::Call);
}

void Parser::nextToken()
{
    cur_token_ = peek_token_;
    peek_token_ = lexer_.nextToken();
}

std::unique_ptr<Program> Parser::parseProgram()
{
    std::unique_ptr<Program> program = std::make_unique<Program>();

    while (cur_token_.type_ != Token::Type::Eof)
    {
        std::unique_ptr<Statement> stmt = parseStatement();
        if (stmt.get() != nullptr)
        {
            program->add(std::move(stmt));
        }
        nextToken();
    }

    return program;
}

std::unique_ptr<Statement> Parser::parseStatement()
{
    switch (cur_token_.type_)
    {
    case Token::Type::Let:
        return parseLetStatement();
    case Token::Type::Return:
        return parseReturnStatement();
    default:
        return parseExpressionStatement();
    }
}

std::unique_ptr<Expression> Parser::parseIdentifier()
{
    return std::make_unique<Identifier>(lexer_.getLiteral(cur_token_));
}

std::unique_ptr<LetStatement> Parser::parseLetStatement()
{
    if (!expectPeek(Token::Type::Ident))
    {
        return nullptr;
    }

    std::string name(lexer_.getLiteral(cur_token_));
    if (!expectPeek(Token::Type::Assign))
    {
        return nullptr;
    }

    nextToken();
    auto value = parseExpression(Precedence::Lowest);
    if (peekTokenIs(Token::Type::Semicolon))
    {
        nextToken();
    }

    return std::make_unique<LetStatement>(name, std::move(value));
}

std::unique_ptr<ReturnStatement> Parser::parseReturnStatement()
{
    nextToken();

    auto return_value = parseExpression(Precedence::Lowest);

    if (peekTokenIs(Token::Type::Semicolon))
    {
        nextToken();
    }
    return std::make_unique<ReturnStatement>(std::move(return_value));
}

std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement()
{
    auto stmt = std::make_unique<ExpressionStatement>(parseExpression(Precedence::Lowest));

    if (peekTokenIs(Token::Type::Semicolon))
    {
        nextToken();
    }

    return stmt;
}

bool Parser::curTokenIs(Token::Type t) const
{
    return cur_token_.type_ == t;
}

bool Parser::peekTokenIs(Token::Type t) const
{
    return peek_token_.type_ == t;
}

bool Parser::expectPeek(Token::Type t)
{
    if (peekTokenIs(t))
    {
        nextToken();
        return true;
    }
    else
    {
        peekError(t);
        return false;
    }
}

void Parser::peekError(Token::Type t)
{
    std::stringstream ss;
    ss << "expected next token to be " << tokenType2String(t) << ", got " << tokenType2String(peek_token_.type_) << " instead";
    errors_.emplace_back(ss.str());
}

void Parser::registerPrefix(Token::Type t, PrefixParseF f)
{
    prefix_parse_fns_[static_cast<std::size_t>(t)] = f;
}

void Parser::registerInfix(Token::Type t, InfixParseF f)
{
    infix_parse_fns_[static_cast<std::size_t>(t)] = f;
}

void Parser::registerPrecedence(Token::Type t, Precedence p)
{
    precedences_[static_cast<std::size_t>(t)] = p;
}

std::unique_ptr<Expression> Parser::parseExpression(Precedence p)
{
    const auto &prefix = prefix_parse_fns_[static_cast<std::size_t>(cur_token_.type_)];
    if (!prefix)
    {
        noPrefixParseFnError(cur_token_.type_);
        return nullptr;
    }
    auto leftExp = prefix();

    while (!peekTokenIs(Token::Type::Semicolon) && p < peekPrecedence())
    {
        auto infix = infix_parse_fns_[static_cast<std::size_t>(peek_token_.type_)];
        if (!infix)
        {
            return leftExp;
        }
        nextToken();
        leftExp = infix(std::move(leftExp));
    }

    return leftExp;
}

std::unique_ptr<IntegerLiteral> Parser::parseIntegerLiteral()
{
    std::string value_s(lexer_.getLiteral(cur_token_));
    return std::make_unique<IntegerLiteral>(std::stoi(value_s));
}

std::unique_ptr<Expression> Parser::parsePrefixExpression()
{
    std::string op(lexer_.getLiteral(cur_token_));
    nextToken();
    return std::make_unique<PrefixExpression>(op, parseExpression(Precedence::Prefix));
}

std::unique_ptr<Boolean> Parser::parseBoolean()
{
    return std::make_unique<Boolean>(curTokenIs(Token::Type::True));
}

std::unique_ptr<Expression> Parser::parseGroupedExpression()
{
    nextToken();
    auto exp = parseExpression(Precedence::Lowest);
    if (!expectPeek(Token::Type::RParen))
    {
        return nullptr;
    }
    return exp;
}

std::unique_ptr<Expression> Parser::parseIfExpression()
{
    if (!expectPeek(Token::Type::LParen))
    {
        return nullptr;
    }
    nextToken();
    auto condition = parseExpression(Precedence::Lowest);
    if (!expectPeek(Token::Type::RParen))
    {
        return nullptr;
    }
    if (!expectPeek(Token::Type::LBrace))
    {
        return nullptr;
    }

    auto consequence = parseBlockStatement();
    std::unique_ptr<BlockStatement> alternative = nullptr;
    if (peekTokenIs(Token::Type::Else))
    {
        nextToken();
        if (!expectPeek(Token::Type::LBrace))
        {
            return nullptr;
        }
        alternative = parseBlockStatement();
    }

    return std::make_unique<IfExpression>(std::move(condition), std::move(consequence), std::move(alternative));
}

std::unique_ptr<Expression> Parser::parseInfixExpression(std::unique_ptr<Expression> left)
{
    std::string op(lexer_.getLiteral(cur_token_));
    auto precedence = curPrecedence();
    nextToken();
    return std::make_unique<InfixExpression>(std::move(left), op, parseExpression(precedence));
}

std::unique_ptr<Expression> Parser::parseCallExpression(std::unique_ptr<Expression> function)
{
    return std::make_unique<CallExpression>(std::move(function), parseCallArguments());
}

std::vector<std::unique_ptr<Expression>> Parser::parseCallArguments()
{
    std::vector<std::unique_ptr<Expression>> args;
    if (peekTokenIs(Token::Type::LParen))
    {
        nextToken();
        return args;
    }

    nextToken();
    args.emplace_back(parseExpression(Precedence::Lowest));
    while (peekTokenIs(Token::Type::Comma))
    {
        nextToken();
        nextToken();
        args.emplace_back(parseExpression(Precedence::Lowest));
    }
    if (!expectPeek(Token::Type::RParen))
    {
        return args;
    }

    return args;
}

void Parser::noPrefixParseFnError(Token::Type t)
{
    std::string msg("no prefix parse function ");
    msg += tokenType2String(t);
    msg += " found";
    errors_.emplace_back(msg);
}

Precedence Parser::peekPrecedence()
{
    return precedences_[static_cast<size_t>(peek_token_.type_)];
}

Precedence Parser::curPrecedence()
{
    return precedences_[static_cast<size_t>(cur_token_.type_)];
}

std::unique_ptr<BlockStatement> Parser::parseBlockStatement()
{
    std::vector<std::unique_ptr<Statement>> statements;
    nextToken();

    while (!curTokenIs(Token::Type::RBrace) && !curTokenIs(Token::Type::Eof))
    {
        auto stmt = parseStatement();
        if (stmt)
        {
            statements.emplace_back(std::move(stmt));
        }
        nextToken();
    }

    return std::make_unique<BlockStatement>(std::move(statements));
}

std::unique_ptr<FunctionLiteral> Parser::parseFunctionLiteral()
{
    if (!expectPeek(Token::Type::LParen))
    {
        return nullptr;
    }

    auto parameters = parseFunctionParameters();

    if (!expectPeek(Token::Type::LBrace))
    {
        return nullptr;
    }

    auto body = parseBlockStatement();
    return std::make_unique<FunctionLiteral>(std::move(body), std::move(parameters));
}

std::vector<std::unique_ptr<Identifier>> Parser::parseFunctionParameters()
{
    std::vector<std::unique_ptr<Identifier>> identifiers;
    if (peekTokenIs(Token::Type::RParen))
    {
        nextToken();
        return identifiers;
    }

    nextToken();
    identifiers.emplace_back(std::make_unique<Identifier>(lexer_.getLiteral(cur_token_)));

    while (peekTokenIs(Token::Type::Comma))
    {
        nextToken();
        nextToken();
        identifiers.emplace_back(std::make_unique<Identifier>(lexer_.getLiteral(cur_token_)));
    }
    if (!expectPeek(Token::Type::RParen))
    {
        return identifiers;
    }
    return identifiers;
}
