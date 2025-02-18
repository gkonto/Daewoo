// #define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"

struct ExpectedStatement
{
    std::string value_;
};

struct ExpectedPrefix
{
    std::string input;
    std::string op;
    int value;
};

struct ExpectedPrefixBoolean
{
    std::string input;
    std::string op;
    bool value;
};

struct ExpectedInfixBoolean
{
    std::string input;
    bool lhs;
    std::string op;
    bool rhs;
};

struct ExpectedInfix
{
    std::string input;
    int lhs;
    std::string op;
    int rhs;
};

struct FunctionParameterExpected
{
    std::string input;
    std::vector<std::string> expected;
};

struct OperatorPrecedenceExpected
{
    std::string input;
    std::string expected;
};

template <typename T>
struct LetStatementsExpected
{
    std::string input;
    std::string expected_identifier;
    T expected_value;
};

static void testLetStatement(const Statement *got, const std::string &value)
{
    const LetStatement *letStmt = dynamic_cast<const LetStatement *>(got);
    REQUIRE(letStmt != nullptr);
    REQUIRE(letStmt->name() == value);
}

static void checkParserErrors(Parser &p)
{
    const auto &errors = p.errors();
    INFO("parser has " << errors.size() << " errors");
    REQUIRE(errors.empty());
}

static void testIntegerLiteral(const Expression *exp, int value)
{
    const IntegerLiteral *integ = dynamic_cast<const IntegerLiteral *>(exp);
    REQUIRE(integ != nullptr);
    REQUIRE(integ->value() == value);
}

static void testIdentifier(const Expression *exp, const std::string &value)
{
    const Identifier *ident = dynamic_cast<const Identifier *>(exp);
    REQUIRE(ident != nullptr);
    REQUIRE(ident->value() == value);
}

static void testBooleanLiteral(const Expression *exp, bool value)
{
    const Boolean *bo = dynamic_cast<const Boolean *>(exp);
    REQUIRE(bo != nullptr);
    REQUIRE(bo->value() == value);
}

static void testLiteralExpression(const Expression *exp, int expected)
{
    testIntegerLiteral(exp, expected);
}

static void testLiteralExpression(const Expression *exp, const std::string &value)
{
    testIdentifier(exp, value);
}

static void testLiteralExpression(const Expression *exp, bool value)
{
    testBooleanLiteral(exp, value);
}

template <typename T>
static void testLetStatement(const LetStatementsExpected<T> &tt)
{
    Lexer l(tt.input);
    Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    REQUIRE(program->size() == 1);

    testLetStatement(program->at(0), tt.expected_identifier);
    auto *letStmt = dynamic_cast<const LetStatement *>(program->at(0));
    REQUIRE(letStmt != nullptr);
    auto val = letStmt->value();
    testLiteralExpression(val, tt.expected_value);
}

template <typename Left, typename Right>
static void testInfixExpression(const Expression *exp, Left left, const std::string &op, Right right)
{
    const InfixExpression *opExp = dynamic_cast<const InfixExpression *>(exp);
    REQUIRE(opExp != nullptr);
    testLiteralExpression(opExp->left(), left);
    REQUIRE(opExp->op() == op);
    testLiteralExpression(opExp->right(), right);
}

TEST_CASE("Test_LetStatements")
{
    std::string input("\
    let x = 5;\
    let y = 10;\
    let foobar = 838383;");

    Lexer lexer(input);
    Parser parser(lexer);

    auto program = parser.parseProgram();
    checkParserErrors(parser);

    REQUIRE(program.get() != nullptr);
    REQUIRE(program->size() == 3);

    std::vector<std::string> expected{
        "x",
        "y",
        "foobar"};

    auto e_i = 0;
    for (const auto &stmt : *program)
    {
        testLetStatement(stmt.get(), expected[e_i++]);
    }
}

TEST_CASE("Test_LetStatementsInt")
{
    std::vector<LetStatementsExpected<int>> expected = {
        {"let x = 5;", "x", 5},
    };
    for (const auto &tt : expected)
    {
        testLetStatement(tt);
    }
}

TEST_CASE("Test_LetStatementsBool")
{
    std::vector<LetStatementsExpected<bool>> expected = {
        {"let y = true;", "y", true},
    };
    for (const auto &tt : expected)
    {
        testLetStatement(tt);
    }
}

TEST_CASE("Test_LetStatementsString")
{
    std::vector<LetStatementsExpected<std::string>> expected = {
        {"let foobar = y;", "foobar", "y"}};

    for (const auto &tt : expected)
    {
        testLetStatement(tt);
    }
}

TEST_CASE("Test_ReturnStatements")
{
    std::string input(" return 5;\
        return 10;\
        return 993322;");
    Lexer l(input);
    Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    REQUIRE(program.get() != nullptr);
    REQUIRE(program->size() == 3);

    for (const auto &stmt : *program)
    {
        const ReturnStatement *returnStmt = dynamic_cast<const ReturnStatement *>(stmt.get());
        REQUIRE(returnStmt != nullptr);
    }
}

TEST_CASE("Test_Identifier")
{
    std::string input("foobar;");
    Lexer l(input);
    Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    REQUIRE(program.get() != nullptr);
    REQUIRE(program->size() == 1);

    for (const auto &stmt : *program)
    {
        const ExpressionStatement *expr_stmt = dynamic_cast<const ExpressionStatement *>(stmt.get());
        REQUIRE(expr_stmt != nullptr);
        const Identifier *ident = dynamic_cast<const Identifier *>(expr_stmt->expression());
        REQUIRE(ident != nullptr);
        REQUIRE(ident->value() == "foobar");
    }
}

TEST_CASE("Test_IntegerLiteralExpression")
{
    std::string input("5");

    Lexer l(input);
    Parser p(l);
    auto program = p.parseProgram();
    checkParserErrors(p);

    REQUIRE(program.get() != nullptr);
    REQUIRE(program->size() == 1);

    for (const auto &stmt : *program)
    {
        const ExpressionStatement *expr_stmt = dynamic_cast<const ExpressionStatement *>(stmt.get());
        REQUIRE(expr_stmt != nullptr);
        const IntegerLiteral *integer_l = dynamic_cast<const IntegerLiteral *>(expr_stmt->expression());
        REQUIRE(integer_l != nullptr);
        REQUIRE(integer_l->value() == 5);
    }
}

TEST_CASE("Test_ParsingPrefixExpressions")
{
    std::vector<ExpectedPrefix> expected{
        {"!5;", "!", 5},
        {"-15;", "-", 15},
    };

    for (const auto &e : expected)
    {
        Lexer l(e.input);
        Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);
        const ExpressionStatement *expr_stmt = dynamic_cast<const ExpressionStatement *>(program->at(0));
        REQUIRE(expr_stmt != nullptr);
        const PrefixExpression *exp = dynamic_cast<const PrefixExpression *>(expr_stmt->expression());
        REQUIRE(exp->op() == e.op);
        testIntegerLiteral(exp->right(), e.value);
    }
}

TEST_CASE("Test_ParsingPrefixExpressionsBoolean")
{
    std::vector<ExpectedPrefixBoolean> expected{
        {"!true;", "!", true},
        {"!false;", "!", false},
    };

    for (const auto &e : expected)
    {
        Lexer l(e.input);
        Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);
        const ExpressionStatement *expr_stmt = dynamic_cast<const ExpressionStatement *>(program->at(0));
        REQUIRE(expr_stmt != nullptr);
        const PrefixExpression *exp = dynamic_cast<const PrefixExpression *>(expr_stmt->expression());
        REQUIRE(exp->op() == e.op);
        testLiteralExpression(exp->right(), e.value);
    }
}

TEST_CASE("Test_ParsingInfixExpressions")
{
    std::vector<ExpectedInfix> expected{
        {"5 + 5;", 5, "+", 5},
        {"5- 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5},
    };

    for (const auto &tt : expected)
    {
        Lexer l(tt.input);
        Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);
        REQUIRE(program->size() == 1);
        const ExpressionStatement *expr_stmt = dynamic_cast<const ExpressionStatement *>(program->at(0));
        REQUIRE(expr_stmt != nullptr);
        const InfixExpression *exp = dynamic_cast<const InfixExpression *>(expr_stmt->expression());
        testIntegerLiteral(exp->left(), tt.lhs);
        REQUIRE(exp->op() == tt.op);
        testIntegerLiteral(exp->right(), tt.rhs);
    }
}

TEST_CASE("Test_ParsingInfixExpressionsBoolean")
{
    std::vector<ExpectedInfixBoolean> expected{
        {"true == true", true, "==", true},
        {"true != false", true, "!=", false},
        {"false == false", false, "==", false},
    };

    for (const auto &tt : expected)
    {
        Lexer l(tt.input);
        Parser p(l);
        auto program = p.parseProgram();
        checkParserErrors(p);
        REQUIRE(program->size() == 1);
        const ExpressionStatement *expr_stmt = dynamic_cast<const ExpressionStatement *>(program->at(0));
        REQUIRE(expr_stmt != nullptr);
        const InfixExpression *exp = dynamic_cast<const InfixExpression *>(expr_stmt->expression());
        testLiteralExpression(exp->left(), tt.lhs);
        REQUIRE(exp->op() == tt.op);
        testLiteralExpression(exp->right(), tt.rhs);
    }
}

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