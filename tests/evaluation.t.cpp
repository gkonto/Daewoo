#include <catch2/catch_test_macros.hpp>

#include <string>

#include "lexer.hpp"
#include "parser.hpp"
#include "object.hpp"
#include "evaluator.hpp"
#include "ast.hpp"
#include "globals.hpp"

struct IntegerExpected
{
    std::string input;
    int expected;
};

struct BooleanExpected
{
    std::string input;
    bool expected;
};

struct StringExpected
{
    std::string input;
    std::string expected;
};

static std::shared_ptr<EvalObject>
testEval(const std::string input)
{
    Lexer l(input);
    Parser p(l);
    auto program = p.parseProgram();
    return Eval(program.get());
}

static void testIntegerObject(const EvalObject *obj, int expected)
{
    REQUIRE(obj != nullptr);
    REQUIRE(obj->type == ObjType::Integer);
    REQUIRE(obj->getInt() == expected);
}

static void testBooleanObject(const EvalObject *obj, bool expected)
{
    REQUIRE(obj != nullptr);
    REQUIRE(obj->type == ObjType::Boolean);
    REQUIRE(obj->getBool() == expected);
}

TEST_CASE("Test_EvalIntegerExpression")
{
    std::vector<IntegerExpected> tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5- 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 +-50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 *-10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 +-10", 50},
    };
    for (const auto &tt : tests)
    {
        auto evaluated = testEval(tt.input);
        testIntegerObject(evaluated.get(), tt.expected);
    }
}

TEST_CASE("Test_EvalBooleanExpression")
{
    std::vector<BooleanExpected> tests = {
        {"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true},
    };

    for (const auto &tt : tests)
    {
        auto evaluated = testEval(tt.input);
        testBooleanObject(evaluated.get(), tt.expected);
    }
}

TEST_CASE("Test_BandOperator")
{
    std::vector<BooleanExpected> tests = {
        {"!true", false},
        {"!false", true},
        {"!5", false},
        {"!!true", true},
        {"!!false", false},
        {"!!5", true},
    };

    for (const auto &tt : tests)
    {
        auto evaluated = testEval(tt.input);
        testBooleanObject(evaluated.get(), tt.expected);
    }
}

TEST_CASE("Test_IfElseExpressionInteger")
{
    std::vector<IntegerExpected> tests = {
        {"if (true) { 10 }", 10},
        {"if (1) { 10 }", 10},
        {"if (1 < 2) { 10 }", 10},
        {"if (1 > 2) { 10 } else { 20 }", 20},
        {"if (1 < 2) { 10 } else { 20 }", 10},
    };
    for (const auto &tt : tests)
    {
        auto evaluated = testEval(tt.input);
        testIntegerObject(evaluated.get(), tt.expected);
    }
}

TEST_CASE("Test_IfElseExpressionNull")
{
    std::vector<std::string> tests = {
        "if (false) { 10 }",
        "if (1 > 2) { 10 }"};
    for (const auto &tt : tests)
    {
        auto evaluated = testEval(tt);
        REQUIRE(evaluated == Globals::getInstance().getNull());
    }
}

TEST_CASE("Test_EvalReturnStatements")
{
    std::vector<IntegerExpected> tests = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
    };

    for (const auto &tt : tests)
    {
        auto evaluated = testEval(tt.input);
        testIntegerObject(evaluated.get(), tt.expected);
    }
}

TEST_CASE("Test_ErrorHandling")
{
    std::vector<StringExpected> tests = {
        {
            "5 + true;",
            "type mismatch: INTEGER + BOOLEAN",
        },
        {
            "5 + true; 5;",
            "type mismatch: INTEGER + BOOLEAN",
        },
        {
            "-true",
            "unknown operator: -BOOLEAN",
        },
        {
            "true + false;",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
            "5; true + false; 5",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
            "if (10 > 1) { true + false; }",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
            "if (10 > 1) { if (10 > 1) { return true + false;} return 1; }",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
    };
    for (const auto &tt : tests)
    {
        auto evaluated = testEval(tt.input);
        REQUIRE(evaluated->type == ObjType::Error);
        REQUIRE(evaluated->getString() == tt.expected);
    }
}