#include <catch2/catch_test_macros.hpp>
#include <sstream>
#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "SyntaxParser.hpp"
#include "ASTBuilder.hpp"
#include "ASTNode.hpp"
#include "TByteCodeBuilder.hpp"
#include "TModule.hpp"

static void checkSyntaxParserErrors(std::optional<SyntaxError> error) {
    INFO("SyntaxError found: " << (error.has_value() ? error.value().msg() : ""));
    REQUIRE(!error.has_value());
}

static TProgram simple_1() {
    // "15;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 15);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_2() {
    // "+15;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 15);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_3() {
    // "15.0;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_4() {
    // "-15.0;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_5() {
    // "1E2;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_6() {
    // "1e2;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_7() {
    // "1e-2;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_8() {
    // "1e+2;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_9() {
    // "-1E-2;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_10() {
    // "1.2E2;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_11() {
    // "0.1E2;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_12() {
    // "1.2E-2;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram addition_1() {
    TProgram program;
    program.addByteCode(OpCode::Pushi, 1);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram addition_2() {
    TProgram program;
    program.addByteCode(OpCode::Pushi, 1);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram addition_3() {
    TProgram program;
    program.addByteCode(OpCode::Pushi, 1);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram subtraction_1() {
    TProgram program;
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Sub);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram subtraction_2() {
    //"4 - 5 - 2;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Sub);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Sub);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram subtraction_3() {
    // "-5 - 2;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Sub);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram unary_1() {
    // -5;
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram unary_2() {
    // "--5;";
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram unary_3() {
    // "---5;";
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram unary_4() {
    // "+-+-+-+--+---5;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram multiplication_1() {
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram multiplication_2() {
    // "4 * 5 * 2;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram division_1() {
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Divide);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram division_2() {
    // "2 / 5 / 6;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Divide);
    program.addByteCode(OpCode::Pushi, 6);
    program.addByteCode(OpCode::Divide);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram boolean_1() {
    // true;
    TProgram program;
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram boolean_2() {
    // false
    TProgram program;
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram boolean_3() {
    // "true && false;"
    TProgram program;
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::And);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram boolean_4() {
    // "true || false;"
    TProgram program;
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::Or);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram boolean_5() {
    // "not true;"
    TProgram program;
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram boolean_6() {
    // "not false;"
    TProgram program;
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram boolean_7() {
    // "not not true;"
    TProgram program;
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram boolean_8() {
    // "not not false;"
    TProgram program;
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram boolean_9() {
    // "not not not true;"
    TProgram program;
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram boolean_10() {
    // "not not not false;"
    TProgram program;
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram parenthesis_1() {
    // (2 + 3.0);
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram parenthesis_2() {
    // "2 + 3 * 4;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram parenthesis_3() {
    // "(2 + 3) * 4;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram parenthesis_4() {
    // "(2 + 3) * (4 + 5);"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram parenthesis_5() {
    // "2 / (5 + 6)"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Pushi, 6);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Divide);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram parenthesis_6() {
    // "10 - (2 - 3);"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 10);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Sub);
    program.addByteCode(OpCode::Sub);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram power_1() {
    // "2 ^ 3;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram power_2() {
    // "2 ^ 3 ^ 4;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram power_3() {
    // "(2 ^ 3) ^ 4;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram power_4() {
    // "2 ^ (3 ^ 4);"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram power_5() {
    // "-2 ^ 3;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram power_6() {
    // "+2 ^ 3;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram power_7() {
    // "-(2 ^ 3);"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram power_8() {
    // "+(2 ^ 3);"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram term_1() {
    // "2 ^ 3 * 4;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram term_2() {
    // "2 * 3 ^ 2 * 4;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram term_3() {
    // "2 * 3 ^ (2 * 4);"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram term_4() {
    // "2 * 3 ^ 2 * 4;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Power);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Pushi, 4);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram relational_operators_1() {
    //"2 < 3;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::IsLt);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram relational_operators_2() {
    //"2 > 3;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::IsGt);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram relational_operators_3() {
    //"2 <= 3;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::IsLte);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram relational_operators_4() {
    //"2 >= 3;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::IsGte);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram relational_operators_5() {
    //"2 == 3;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::IsEq);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram relational_operators_6() {
    //"2 != 3;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::IsNotEq);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram relational_operators_7() {
    //"2 + 3 == 5;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::IsEq);
    program.addByteCode(OpCode::Halt);
    return program;
}

static void testByteCodeCore(const std::string &input, const TProgram &expected) {
    std::istringstream iss(input);
    Scanner sc(iss);

    SyntaxParser sp(sc);
    auto err = sp.syntaxCheck();
    checkSyntaxParserErrors(err);

    TByteCodeBuilder builder(sp.tokens());
    TModule module;
    constantValueTable.clear();
    builder.build(&module);

    std::stringstream msg;
    msg << "Expected: " << std::endl
        << expected.string() << std::endl
        << "Got: " << std::endl
        << module.code().string();
    INFO(msg.str());
    REQUIRE(module.code().size() == expected.size());
    REQUIRE(module.code() == expected);
}

TEST_CASE("Test_ParsingByteCode") {
    SECTION("Simple") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"15;",    simple_1() },
            {"+15;",   simple_2() },
            {"15.0",   simple_3() },
            {"-15.0",  simple_4() },
            {"1E2",    simple_5() },
            {"1e2",    simple_6() },
            {"1e-2",   simple_7() },
            {"1e+2",   simple_8() },
            {"-1E-2",  simple_9() },
            {"1.2E2",  simple_10()},
            {"0.1E2",  simple_11()},
            {"1.2E-2", simple_12()},
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Addition") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"1 + 2;",           addition_1()},
            {"1 + 2 + 3;",       addition_2()},
            {"1 + 2 + (3 + 4);", addition_3()},
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Subtraction") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"4 - 5;",     subtraction_1()},
            {"4 - 5 - 2;", subtraction_2()},
            {"-5 - 2;",    subtraction_3()},
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Unary") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"-5;",             unary_1()},
            {"--5;",            unary_2()},
            {"---5;",           unary_3()},
            {"+-+-+-+--+---5;", unary_4()},
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Multiplication") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"2 * 4;",     multiplication_1()},
            {"4 * 5 * 2;", multiplication_2()},
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Division") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"2 / 5;",     division_1()},
            {"2 / 5 / 6;", division_2()},
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Boolean") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"true;",              boolean_1() },
            {"false;",             boolean_2() },
            {"true and false;",    boolean_3() },
            {"true or false;",     boolean_4() },
            {"not true;",          boolean_5() },
            {"not false;",         boolean_6() },
            {"not not true;",      boolean_7() },
            {"not not false;",     boolean_8() },
            {"not not not true;",  boolean_9() },
            {"not not not false;", boolean_10()},
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Order of operations") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"(2 + 3);",           parenthesis_1()},
            {"2 + 3 * 4;",         parenthesis_2()},
            {"(2 + 3) * 4;",       parenthesis_3()},
            {"(2 + 3) * (4 + 5);", parenthesis_4()},
            {"2 / (5 + 6)",        parenthesis_5()},
            {"10 - (2 - 3);",      parenthesis_6()}
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Power") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"2 ^ 3;",       power_1()},
            {"2 ^ 3 ^ 4;",   power_2()},
            {"(2 ^ 3) ^ 4;", power_3()},
            {"2 ^ (3 ^ 4);", power_4()},
            {"-2 ^ 3;",      power_5()}, // Should be interpreted as -(2^3), not (-2)^3
            {"+2 ^ 3;",      power_6()}, // Should be interpreted as +(2^3)
            {"-(2 ^ 3);",    power_7()}, // Explicit grouping: (-1) * (2^3)
            {"+(2 ^ 3);",    power_8()}
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Term") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"2 ^ 3 * 4;",       term_1()},
            {"2 * 3 ^ 2 * 4;",   term_2()},
            {"2 * 3 ^ (2 * 4);", term_3()},
            {"2 * 3 ^ 2 * 4;",   term_4()}
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Relational Operators") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"2 < 3;",      relational_operators_1()},
            {"2 > 3;",      relational_operators_2()},
            {"2 <= 3;",     relational_operators_3()},
            {"2 >= 3;",     relational_operators_4()},
            {"2 == 3;",     relational_operators_5()},
            {"2 != 3;",     relational_operators_6()},
            {"2 + 3 == 5;", relational_operators_7()}
        };

        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }
}