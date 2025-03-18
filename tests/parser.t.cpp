// #define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "SyntaxParser.hpp"
#include "ASTBuilder.hpp"
#include "ASTNode.hpp"

struct ExpectedStatement {
    std::string value_;
};

struct ExpectedPrefix {
    std::string input;
    std::string op;
    int value;
};

struct ExpectedPrefixBoolean {
    std::string input;
    std::string op;
    bool value;
};

struct ExpectedInfixBoolean {
    std::string input;
    bool lhs;
    ASTNodeType op;
    bool rhs;
};

struct ExpectedInfix {
    std::string input;
    int lhs;
    ASTNodeType op;
    int rhs;
};

struct FunctionParameterExpected {
    std::string input;
    std::vector<std::string> expected;
};

struct OperatorPrecedenceExpected {
    std::string input;
    std::string expected;
};

template <typename T>
struct LetStatementsExpected {
    std::string input;
    std::string expected_identifier;
    T expected_value;
};

static void testLetStatement(const ASTNode *got, const std::string &value) {
    const ASTLetStatement *letStmt = dynamic_cast<const ASTLetStatement *>(got);
    REQUIRE(letStmt != nullptr);
    REQUIRE(letStmt->identifierValue() == value);
}

static void checkParserErrors(Parser &p) {
    const auto &errors = p.errors();
    INFO("parser has " << errors.size() << " errors");
    REQUIRE(errors.empty());
}

static void checkSyntaxParserErrors(std::optional<SyntaxError> error) {
    INFO("SyntaxError found: " << (error.has_value() ? error.value().msg() : ""));
    REQUIRE(!error.has_value());
}

static void testIntegerLiteral(const ASTNode *exp, int value) {
    REQUIRE(exp->type() == ASTNodeType::ntPrimary);
    const auto *primary = dynamic_cast<const ASTPrimary *>(exp);
    const auto *factor = primary->factor();
    REQUIRE(factor->type() == ASTNodeType::ntInteger);
    const auto *integ = dynamic_cast<const ASTInteger *>(factor);
    REQUIRE(integ != nullptr);
    REQUIRE(integ->value() == value);
}

static void testIdentifierLiteral(const ASTNode *exp, const std::string &value) {
    REQUIRE(exp->type() == ASTNodeType::ntPrimary);
    const auto *primary = dynamic_cast<const ASTPrimary *>(exp);
    const auto *factor = primary->factor();
    REQUIRE(factor->type() == ASTNodeType::ntIdentifier);
    const auto *ident = dynamic_cast<const ASTIdentifier *>(factor);
    REQUIRE(ident != nullptr);
    REQUIRE(ident->value() == value);
}

static void testBooleanLiteral(const ASTNode *exp, bool value) {
    REQUIRE(exp->type() == ASTNodeType::ntPrimary);
    const auto *primary = dynamic_cast<const ASTPrimary *>(exp);
    const auto *factor = primary->factor();
    REQUIRE(factor->type() == ASTNodeType::ntBoolean);
    const auto *bo = dynamic_cast<const ASTBoolean *>(factor);
    REQUIRE(bo != nullptr);
    REQUIRE(bo->value() == value);
}

static void testLiteralExpression(const ASTExpression *exp, int expected) {
    testIntegerLiteral(exp, expected);
}

static void testLiteralExpression(const ASTExpression *exp, const std::string &value) {
    testIdentifierLiteral(exp, value);
}

static void testLiteralExpression(const ASTNode *exp, bool value) {
    testBooleanLiteral(exp, value);
}

template <typename Left, typename Right>
static void testInfixExpression(const Expression *exp, Left left, const std::string &op, Right right) {
    const InfixExpression *opExp = dynamic_cast<const InfixExpression *>(exp);
    REQUIRE(opExp != nullptr);
    testLiteralExpression(opExp->left(), left);
    REQUIRE(opExp->op() == op);
    testLiteralExpression(opExp->right(), right);
}

TEST_CASE("Test_LetStatements") {
    std::string input(
        "\
    let x = 5;\
    let y = 10;\
    let foobar = 838383;");

    std::istringstream iss(input);
    Scanner sc(iss);

    SyntaxParser sp(sc);
    auto err = sp.syntaxCheck();
    checkSyntaxParserErrors(err);

    ASTBuilder ast_builder(sp.tokens());
    auto ast = ast_builder.build();

    REQUIRE(ast.get() != nullptr);
    REQUIRE(ast->size() == 3);

    std::vector<std::string> expected{"x", "y", "foobar"};

    size_t index = 0;
    for (const auto &stmt: *ast) {
        testLetStatement(stmt.get(), expected[index++]);
    }
}

TEST_CASE("Test_LetStatementsInt") {
    std::vector<LetStatementsExpected<int>> expected = {
        {"let x = 5;", "x", 5},
    };
    for (const auto &tt: expected) {
        std::istringstream iss(tt.input);
        Scanner sc(iss);

        SyntaxParser sp(sc);
        auto err = sp.syntaxCheck();
        checkSyntaxParserErrors(err);

        ASTBuilder ast_builder(sp.tokens());
        auto ast = ast_builder.build();

        REQUIRE(ast.get() != nullptr);
        REQUIRE(ast->size() == 1);

        testLetStatement(ast->at(0), tt.expected_identifier);
        const auto *letStmt = dynamic_cast<const ASTLetStatement *>(ast->at(0));
        REQUIRE(letStmt != nullptr);
        const auto *rhs = letStmt->rhs();
        testIntegerLiteral(rhs, tt.expected_value);
    }
}

TEST_CASE("Test_LetStatementsBool") {
    std::vector<LetStatementsExpected<bool>> expected = {
        {"let y = true;", "y", true},
    };
    for (const auto &tt: expected) {
        std::istringstream iss(tt.input);
        Scanner sc(iss);

        SyntaxParser sp(sc);
        auto err = sp.syntaxCheck();
        checkSyntaxParserErrors(err);

        ASTBuilder ast_builder(sp.tokens());
        auto ast = ast_builder.build();

        REQUIRE(ast.get() != nullptr);
        REQUIRE(ast->size() == 1);

        testLetStatement(ast->at(0), tt.expected_identifier);
        const auto *letStmt = dynamic_cast<const ASTLetStatement *>(ast->at(0));
        REQUIRE(letStmt != nullptr);
        const auto *rhs = letStmt->rhs();
        testBooleanLiteral(rhs, tt.expected_value);
    }
}

TEST_CASE("Test_LetStatementsString") {
    std::vector<LetStatementsExpected<std::string>> expected = {{"let foobar = y;", "foobar", "y"}};

    for (const auto &tt: expected) {
        std::istringstream iss(tt.input);
        Scanner sc(iss);

        SyntaxParser sp(sc);
        auto err = sp.syntaxCheck();
        checkSyntaxParserErrors(err);

        ASTBuilder ast_builder(sp.tokens());
        auto ast = ast_builder.build();

        REQUIRE(ast.get() != nullptr);
        REQUIRE(ast->size() == 1);

        testLetStatement(ast->at(0), tt.expected_identifier);
        const auto *letStmt = dynamic_cast<const ASTLetStatement *>(ast->at(0));
        REQUIRE(letStmt != nullptr);
        const auto *rhs = letStmt->rhs();
        testIdentifierLiteral(rhs, tt.expected_value);
    }
}

TEST_CASE("Test_ReturnStatements") {
    std::string input(
        " return 5;\
        return 10;\
        return 993322;");

    std::istringstream iss(input);
    Scanner sc(iss);

    SyntaxParser sp(sc);
    auto err = sp.syntaxCheck();

    REQUIRE(err.has_value());
    REQUIRE(err.value().msg() == "You cannot use a return statement outside a user function");
}

TEST_CASE("Test_Identifier") {
    std::string input("foobar;");
    std::istringstream iss(input);
    Scanner sc(iss);

    SyntaxParser sp(sc);
    auto err = sp.syntaxCheck();
    checkSyntaxParserErrors(err);
    ASTBuilder ast_builder(sp.tokens());
    auto ast = ast_builder.build();
    REQUIRE(ast.get() != nullptr);
    REQUIRE(ast->size() == 1);

    for (const auto &stmt: *ast) {
        REQUIRE(stmt->type() == ASTNodeType::ntExpressionStatement);
        const auto *expr_stmt = dynamic_cast<const ASTExpressionStatement *>(stmt.get());
        REQUIRE(expr_stmt != nullptr);
        const auto *expr = expr_stmt->expression();
        REQUIRE(expr->type() == ASTNodeType::ntPrimary);
        const auto *primary = dynamic_cast<const ASTPrimary *>(expr);
        REQUIRE(primary != nullptr);
        const auto *factor = primary->factor();
        REQUIRE(factor->type() == ASTNodeType::ntIdentifier);
        const auto *ident = dynamic_cast<const ASTIdentifier *>(factor);
        REQUIRE(ident != nullptr);
        REQUIRE(ident->value() == "foobar");
    }
}

TEST_CASE("Test_IntegerLiteralExpression") {
    std::string input("5;");
    std::istringstream iss(input);
    Scanner sc(iss);

    SyntaxParser sp(sc);
    auto err = sp.syntaxCheck();
    checkSyntaxParserErrors(err);
    ASTBuilder ast_builder(sp.tokens());
    auto ast = ast_builder.build();
    REQUIRE(ast.get() != nullptr);
    REQUIRE(ast->size() == 1);
    for (const auto &stmt: *ast) {
        REQUIRE(stmt->type() == ASTNodeType::ntExpressionStatement);
        const auto *expr_stmt = dynamic_cast<const ASTExpressionStatement *>(stmt.get());
        REQUIRE(expr_stmt != nullptr);
        const auto *expr = expr_stmt->expression();
        REQUIRE(expr->type() == ASTNodeType::ntPrimary);
        const auto *primary = dynamic_cast<const ASTPrimary *>(expr);
        REQUIRE(primary != nullptr);
        const auto *factor = primary->factor();
        REQUIRE(factor->type() == ASTNodeType::ntInteger);
        const auto *integer = dynamic_cast<const ASTInteger *>(factor);
        REQUIRE(integer != nullptr);
        REQUIRE(integer->value() == 5);
    }
}

TEST_CASE("Test_ParsingUnaryExpression") {
    std::vector<ExpectedPrefix> expected{
        {"-15;", "-", 15},
    };

    for (const auto &[input, op, value]: expected) {
        std::istringstream iss(input);
        Scanner sc(iss);

        SyntaxParser sp(sc);
        auto err = sp.syntaxCheck();
        checkSyntaxParserErrors(err);
        ASTBuilder ast_builder(sp.tokens());
        auto ast = ast_builder.build();
        REQUIRE(ast.get() != nullptr);
        REQUIRE(ast->size() == 1);

        const auto *stmt = ast->at(0);
        REQUIRE(stmt->type() == ASTNodeType::ntExpressionStatement);
        const auto *expr_stmt = dynamic_cast<const ASTExpressionStatement *>(stmt);
        REQUIRE(expr_stmt != nullptr);

        const auto *expr = expr_stmt->expression();
        REQUIRE(expr->type() == ASTNodeType::ntUnaryMinus);
        const auto *uniminus = dynamic_cast<const ASTUniOp *>(expr);
        REQUIRE(uniminus != nullptr);

        const auto *left = uniminus->left();
        REQUIRE(left->type() == ASTNodeType::ntPrimary);

        const auto *primary = dynamic_cast<const ASTPrimary *>(left);
        REQUIRE(primary != nullptr);

        const auto *factor = primary->factor();
        REQUIRE(factor->type() == ASTNodeType::ntInteger);

        const auto *integer = dynamic_cast<const ASTInteger *>(factor);
        REQUIRE(integer != nullptr);
        REQUIRE(integer->value() == value);

        const auto *primaryPlus = primary->primaryPlus();
        REQUIRE(primaryPlus->type() == ASTNodeType::ntNull);
    }
}

TEST_CASE("Test_ParsingPrefixExpressionsBoolean") {
    std::vector<ExpectedPrefixBoolean> expected{
        {"not true;", "!", true},
        {"not false;", "!", false},
    };

    for (const auto &[input, op, value]: expected) {
        std::istringstream iss(input);
        Scanner sc(iss);

        SyntaxParser sp(sc);
        auto err = sp.syntaxCheck();
        checkSyntaxParserErrors(err);
        ASTBuilder ast_builder(sp.tokens());
        auto ast = ast_builder.build();
        REQUIRE(ast.get() != nullptr);
        REQUIRE(ast->size() == 1);

        const auto *stmt = ast->at(0);
        REQUIRE(stmt->type() == ASTNodeType::ntExpressionStatement);
        const auto *expr_stmt = dynamic_cast<const ASTExpressionStatement *>(stmt);
        REQUIRE(expr_stmt != nullptr);

        const auto *expr = expr_stmt->expression();
        REQUIRE(expr->type() == ASTNodeType::ntPrimary);

        const auto *primary = dynamic_cast<const ASTPrimary *>(expr);
        REQUIRE(primary != nullptr);

        const auto *factor = primary->factor();
        REQUIRE(factor->type() == ASTNodeType::ntNOT);

        const auto *notOp = dynamic_cast<const ASTNotOp *>(factor);
        REQUIRE(notOp != nullptr);

        const auto *notOpExpr = notOp->expression();
        REQUIRE(notOpExpr->type() == ASTNodeType::ntPrimary);

        const auto *notOpPrimary = dynamic_cast<const ASTPrimary *>(notOpExpr);
        REQUIRE(notOpPrimary != nullptr);

        const auto *v = notOpPrimary->factor();
        REQUIRE(v->type() == ASTNodeType::ntBoolean);
        // REQUIRE(notOp->expression() == bool);

        const auto *primaryPlus = notOpPrimary->primaryPlus();
        REQUIRE(primaryPlus->type() == ASTNodeType::ntNull);
    }
}

TEST_CASE("Test_ParsingInfixExpressions") {
    std::vector<ExpectedInfix> expected{
        {"5 + 5;", 5, ASTNodeType::ntAdd, 5},  {"5- 5;", 5, ASTNodeType::ntSub, 5},
        {"5 * 5;", 5, ASTNodeType::ntMult, 5}, {"5 / 5;", 5, ASTNodeType::ntDiv, 5},
        {"5 > 5;", 5, ASTNodeType::ntGT, 5},   {"5 < 5;", 5, ASTNodeType::ntLT, 5},
        {"5 == 5;", 5, ASTNodeType::ntEQ, 5},  {"5 != 6;", 5, ASTNodeType::ntNE, 6},
    };

    for (const auto &[input, lhs_exp, op, rhs_exp]: expected) {
        std::istringstream iss(input);
        Scanner sc(iss);

        SyntaxParser sp(sc);
        auto err = sp.syntaxCheck();
        checkSyntaxParserErrors(err);
        ASTBuilder ast_builder(sp.tokens());
        auto ast = ast_builder.build();
        REQUIRE(ast.get() != nullptr);
        REQUIRE(ast->size() == 1);

        const auto *stmt = ast->at(0);
        REQUIRE(stmt->type() == ASTNodeType::ntExpressionStatement);
        const auto *expr_stmt = dynamic_cast<const ASTExpressionStatement *>(stmt);
        REQUIRE(expr_stmt != nullptr);

        const auto *expr = expr_stmt->expression();
        REQUIRE(expr->type() == op);

        const auto *binop = dynamic_cast<const ASTBinOp *>(expr);
        REQUIRE(binop != nullptr);

        const auto *lhs = binop->lhs();
        REQUIRE(lhs->type() == ASTNodeType::ntPrimary);
        const auto *lhspr = dynamic_cast<const ASTPrimary *>(lhs);
        REQUIRE(lhspr != nullptr);

        const auto *lhs_fac = lhspr->factor();
        REQUIRE(lhs_fac->type() == ASTNodeType::ntInteger);
        const auto *lhs_i = dynamic_cast<const ASTInteger *>(lhs_fac);

        REQUIRE(lhs_i->value() == lhs_exp);

        const auto *rhs = binop->rhs();
        REQUIRE(rhs->type() == ASTNodeType::ntPrimary);
        const auto *rhspr = dynamic_cast<const ASTPrimary *>(rhs);
        REQUIRE(rhspr != nullptr);

        const auto *rhs_fac = rhspr->factor();
        REQUIRE(rhs_fac->type() == ASTNodeType::ntInteger);
        const auto *rhs_i = dynamic_cast<const ASTInteger *>(rhs_fac);

        REQUIRE(rhs_i->value() == rhs_exp);
    }
}

TEST_CASE("Test_ParsingInfixExpressionsBoolean") {
    std::vector<ExpectedInfixBoolean> expected{
        {"true == true", true, ASTNodeType::ntEQ, true},
        {"true != false", true, ASTNodeType::ntNE, false},
        {"false == false", false, ASTNodeType::ntEQ, false},
    };

    for (const auto &[input, lhs_exp, op, rhs_exp]: expected) {
        std::istringstream iss(input);
        Scanner sc(iss);

        SyntaxParser sp(sc);
        auto err = sp.syntaxCheck();
        checkSyntaxParserErrors(err);
        ASTBuilder ast_builder(sp.tokens());
        auto ast = ast_builder.build();
        REQUIRE(ast.get() != nullptr);
        REQUIRE(ast->size() == 1);

        const auto *stmt = ast->at(0);
        REQUIRE(stmt->type() == ASTNodeType::ntExpressionStatement);
        const auto *expr_stmt = dynamic_cast<const ASTExpressionStatement *>(stmt);
        REQUIRE(expr_stmt != nullptr);

        const auto *expr = expr_stmt->expression();
        REQUIRE(expr->type() == op);

        const auto *binop = dynamic_cast<const ASTBinOp *>(expr);
        REQUIRE(binop != nullptr);

        const auto *lhs = binop->lhs();
        REQUIRE(lhs->type() == ASTNodeType::ntPrimary);
        const auto *lhspr = dynamic_cast<const ASTPrimary *>(lhs);
        REQUIRE(lhspr != nullptr);

        const auto *lhs_fac = lhspr->factor();
        REQUIRE(lhs_fac->type() == ASTNodeType::ntBoolean);
        const auto *lhs_i = dynamic_cast<const ASTBoolean *>(lhs_fac);

        REQUIRE(lhs_i->value() == lhs_exp);

        const auto *rhs = binop->rhs();
        REQUIRE(rhs->type() == ASTNodeType::ntPrimary);
        const auto *rhspr = dynamic_cast<const ASTPrimary *>(rhs);
        REQUIRE(rhspr != nullptr);

        const auto *rhs_fac = rhspr->factor();
        REQUIRE(rhs_fac->type() == ASTNodeType::ntBoolean);
        const auto *rhs_i = dynamic_cast<const ASTBoolean *>(rhs_fac);

        REQUIRE(rhs_i->value() == rhs_exp);
    }
}
/*
TEST_CASE("Test_OperatorPrecedenceParsing")
{
    std::vector<OperatorPrecedenceExpected> expected{
        {
            "!-a",
            "(!(-a))",
        },
        {
            "a + b + c",
            "((a + b) + c)",
        },
        {
            "a + b - c",
            "((a + b) - c)",
        },
        {
            "a * b * c",
            "((a * b) * c)",
        },
        {
            "a * b / c",
            "((a * b) / c)",
        },
        {"a + b / c",
         "(a + (b / c))"},
        {
            "a + b * c + d / e - f",
            "(((a + (b * c)) + (d / e)) - f)",
        },
        {
            "3 + 4; -5 * 5",
            "(3 + 4)((-5) * 5)",
        },
        {
            "5 > 4 == 3 < 4",
            "((5 > 4) == (3 < 4))",
        },
        {
            "5 < 4 != 3 > 4",
            "((5 < 4) != (3 > 4))",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        },
        {
            "true",
            "true",
        },
        {
            "false",
            "false",
        },
        {
            "3 > 5 == false",
            "((3 > 5) == false)",
        },
        {
            "3 < 5 == true",
            "((3 < 5) == true)",
        },

        {
            "1 + (2 + 3) + 4",
            "((1 + (2 + 3)) + 4)",
        },
        {
            "(5 + 5) * 2",
            "((5 + 5) * 2)",
        },
        {
            "2 / (5 + 5)",
            "(2 / (5 + 5))",
        },
        {
            "-(5 + 5)",
            "(-(5 + 5))",
        },
        {"!(true == true)",
         "(!(true == true))"},
        {
            "a + add(b * c) + d",
            "((a + add((b * c))) + d)",
        },
        {
            "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
            "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
        },
        {
            "add(a + b + c * d / f + g)",
            "add((((a + b) + ((c * d) / f)) + g))",
        }};

    for (const auto &tt : expected)
    {
        Lexer l(tt.input);
        Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);
        auto got = program->toString();
        REQUIRE(got == tt.expected);
    }
}
*/
/*
TEST_CASE("Test_IfExpression")
{
    std::string input("if (x < y) { x }");
    Lexer l(input);
    Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);
    REQUIRE(program->size() == 1);
    auto *stmt = dynamic_cast<const ExpressionStatement *>(program->at(0));
    REQUIRE(stmt != nullptr);
    auto *exp = dynamic_cast<const IfExpression *>(stmt->expression());
    REQUIRE(exp != nullptr);
    testInfixExpression<std::string, std::string>(exp->condition(), "x", "<", "y");
    REQUIRE(exp->consequence()->size() == 1);
    auto *consequence = dynamic_cast<const ExpressionStatement *>(exp->consequence()->at(0));
    REQUIRE(consequence != nullptr);
    testIdentifier(consequence->expression(), "x");
    REQUIRE(exp->alternative() == nullptr);
}
    */
/*
TEST_CASE("Test_FunctionLiteranParsing")
{
    std::string input("fn(x, y) { x + y; }");
    Lexer l(input);
    Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);
    REQUIRE(program->size() == 1);
    auto *stmt = dynamic_cast<const ExpressionStatement *>(program->at(0));
    REQUIRE(stmt != nullptr);
    auto *function = dynamic_cast<const FunctionLiteral *>(stmt->expression());
    REQUIRE(function != nullptr);
    REQUIRE(function->paramsSize() == 2);
    testIdentifier(function->param(0), "x");
    testIdentifier(function->param(1), "y");
    REQUIRE(function->size() == 1);
    auto bodyStmt = dynamic_cast<const ExpressionStatement *>(function->body()->at(0));
    REQUIRE(bodyStmt != nullptr);
    testInfixExpression<std::string, std::string>(bodyStmt->expression(), "x", "+", "y");
}

TEST_CASE("Test_FunctionParameterParsing")
{
    std::vector<FunctionParameterExpected> tests = {
        {"fn() {};", std::vector<std::string>{}},
        {"fn(x) {};", std::vector<std::string>{"x"}},
        {"fn(x, y, z) {};", std::vector<std::string>{"x", "y", "z"}}};

    for (const auto &tt : tests)
    {
        Lexer l(tt.input);
        Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);
        auto *stmt = dynamic_cast<const ExpressionStatement *>(program->at(0));
        REQUIRE(stmt != nullptr);
        auto *function = dynamic_cast<const FunctionLiteral *>(stmt->expression());
        REQUIRE(function != nullptr);
        REQUIRE(function->paramsSize() == tt.expected.size());
        for (size_t i = 0; i < tt.expected.size(); ++i)
        {
            testLiteralExpression(function->param(i), tt.expected[i]);
        }
    }
}

TEST_CASE("Test_CallExpressionParsing")
{
    std::string input("add(1, 2 * 3, 4 + 5);");
    Lexer l(input);
    Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);
    REQUIRE(program->size() == 1);
    auto *stmt = dynamic_cast<const ExpressionStatement *>(program->at(0));
    REQUIRE(stmt != nullptr);
    auto *exp = dynamic_cast<const CallExpression *>(stmt->expression());
    REQUIRE(exp != nullptr);
    testIdentifier(exp->function(), "add");
    REQUIRE(exp->argsSize() == 3);
    testIntegerLiteral(exp->argument(0), 1);
    testInfixExpression<int, int>(exp->argument(1), 2, "*", 3);
    testInfixExpression<int, int>(exp->argument(2), 4, "+", 5);
}
*/