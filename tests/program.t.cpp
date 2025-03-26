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
#include "ConstantTable.hpp"

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

static TProgram simple_13() {
    // "--32;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 32);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_14() {
    // "---32;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 32);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Umi);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram simple_15() {
    // "++++32;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 32);
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

static TProgram relational_operators_8() {
    // "let x = 1; x < x and x > x;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 1);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::IsLt);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::IsGt);
    program.addByteCode(OpCode::And);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram relational_operators_9() {
    // "not (1 > 2);"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 1);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::IsGt);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram relational_operators_10() {
    // "let x = 15; let y = 10; (x >= y) and not (x<= y);"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 15);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Pushi, 10);
    program.addByteCode(OpCode::Store, 1);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::Load, 1);
    program.addByteCode(OpCode::IsGte);
    program.addByteCode(OpCode::Load, 1);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::IsLte);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::And);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram relational_operators_11() {
    // "let x = 15; let y = 10; ((x >= y) and not (y <= y)) == false"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 15);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Pushi, 10);
    program.addByteCode(OpCode::Store, 1);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::Load, 1);
    program.addByteCode(OpCode::IsGte);
    program.addByteCode(OpCode::Load, 1);
    program.addByteCode(OpCode::Load, 1);
    program.addByteCode(OpCode::IsLte);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::And);
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::IsEq);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram expression_1() {
    // "not (false or true);"
    TProgram program;
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Or);
    program.addByteCode(OpCode::Not);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram let_statement_1() {
    // "let x = 5;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram let_statement_2() {
    // "let x = 5.0;"
    TProgram program;
    program.addByteCode(OpCode::Pushd, 1);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram let_statement_3() {
    // "let x = 5; let y = x + 5;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Store, 1);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram let_statement_4() {
    // "let x = 5; let y = 2*x;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Store, 1);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram assignment_1() {
    // "let x = 5; x = 10;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Pushi, 10);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram assignment_2() {
    // "let x = 5; x = x + 10;"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::Pushi, 10);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram let_statement_5() {
    // "let x = false and true;"
    TProgram program;
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::And);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram let_statement_6() {
    //"let x = 5 + 14 - 32 + 3 * (25 + 2);"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Pushi, 14);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Pushi, 32);
    program.addByteCode(OpCode::Sub);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Pushi, 25);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Mult);
    program.addByteCode(OpCode::Add);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Halt);
    return program;
}

static TProgram let_statement_7() {
    // "let a = 2; a == 2"
    TProgram program;
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::IsEq);
    program.addByteCode(OpCode::Halt);
    return program;
}

static constexpr const char *input_if_1() {
    return "let a = false;\n"
           "if true then\n"
           "a = true;\n"
           "end;\n";
}

static TProgram expected_if_1() {
    TProgram program;
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::JmpIfFalse, 3);
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Halt);
    return program;
}

static constexpr const char *input_if_2() {
    return "let a = true;\n"
           "if true then\n"
           "a = false;\n"
           "end;\n";
}

static TProgram expected_if_2() {
    TProgram program;
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::JmpIfFalse, 3);
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Halt);
    return program;
}

static constexpr const char *input_if_3() {
    return "let  x = 5;\n"
           "let  y = 10;\n"
           "let a = true;\n"
           "if x > y then\n"
           " a = false\n"
           "else\n"
           " a = true\n"
           "end;\n";
}

static TProgram expected_if_3() {
    TProgram program;
    program.addByteCode(OpCode::Pushi, 5);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Pushi, 10);
    program.addByteCode(OpCode::Store, 1);
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Store, 2);
    program.addByteCode(OpCode::Load, 0);
    program.addByteCode(OpCode::Load, 1);
    program.addByteCode(OpCode::IsGt);
    program.addByteCode(OpCode::JmpIfFalse, 4);
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::Store, 2);
    program.addByteCode(OpCode::Jmp, 3);
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::Store, 2);
    program.addByteCode(OpCode::Halt);
    return program;
}

static constexpr const char *input_if_4() {
    return "if true then\n"
           "let a = 1;\n"
           "if true then\n"
           "a = 2;\n"
           "if false then\n"
           "a = 3;\n"
           "end;\n"
           "end;\n"
           "end;\n";
}

static TProgram expected_if_4() {
    TProgram program;
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::JmpIfFalse, 11);
    program.addByteCode(OpCode::Pushi, 1);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Pushb, true);
    program.addByteCode(OpCode::JmpIfFalse, 7);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::JmpIfFalse, 3);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Halt);
    return program;
}

static constexpr const char *input_if_5() {
    return "if false then\n"
           "    let a = 1;\n"
           "else\n"
           "    if false then\n"
           "        a = 2;\n"
           "    else\n"
           "        a = 3;\n"
           "    end;\n"
           "end;\n";
}

static TProgram expected_if_5() {
    TProgram program;
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::JmpIfFalse, 4);
    program.addByteCode(OpCode::Pushi, 1);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Jmp, 8);
    program.addByteCode(OpCode::Pushb, false);
    program.addByteCode(OpCode::JmpIfFalse, 4);
    program.addByteCode(OpCode::Pushi, 2);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Jmp, 3);
    program.addByteCode(OpCode::Pushi, 3);
    program.addByteCode(OpCode::Store, 0);
    program.addByteCode(OpCode::Halt);
    return program;
}

static constexpr const char *input_funcall_1() {
    return "fn rint()\n"
           "    return 3\n"
           "end\n"
           "rint();\n";
}

static TProgram expected_funcall_1() {
    TProgram program;
    program.addByteCode(OpCode::Pushi, 0);
    program.addByteCode(OpCode::Call);
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

TEST_CASE("Test_ParsingByteCodeGeneral") {
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
            {"--32;",  simple_13()},
            {"---32;", simple_14()},
            {"++++32", simple_15()}
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

    SECTION("LetStatement") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"let x = 5;",                          let_statement_1()},
            {"let x = 5.0;",                        let_statement_2()},
            {"let x = 5; let y = x + 5;",           let_statement_3()},
            {"let x = 5; let y = 2*x;",             let_statement_4()},
            {"let x = false and true;",             let_statement_5()},
            {"let x = 5 + 14 - 32 + 3 * (25 + 2);", let_statement_6()},
            {"let a = 2; a == 2",                   let_statement_7()}
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Assignment") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"let x = 5; x = 10;",     assignment_1()},
            {"let x = 5; x = x + 10;", assignment_2()},
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Relational Operators") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"2 < 3;",                                                       relational_operators_1() },
            {"2 > 3;",                                                       relational_operators_2() },
            {"2 <= 3;",                                                      relational_operators_3() },
            {"2 >= 3;",                                                      relational_operators_4() },
            {"2 == 3;",                                                      relational_operators_5() },
            {"2 != 3;",                                                      relational_operators_6() },
            {"2 + 3 == 5;",                                                  relational_operators_7() },
            {"let x = 1; x < x and x > x;",                                  relational_operators_8() },
            {"not (1 > 2);",                                                 relational_operators_9() },
            {"let x = 15; let y = 10; ((x >= y) and not (y <= x));",         relational_operators_10()},
            {"let x = 15; let y = 10; ((x >= y) and not (y <= y)) == false",
             relational_operators_11()                                                                }
        };

        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }

    SECTION("Expression") {
        std::vector<std::tuple<std::string, TProgram>> tests = {
            {"not (false or true);", expression_1()},
        };
        for (const auto &[input, expected]: tests) {
            testByteCodeCore(input, expected);
        }
    }
}

TEST_CASE("Test_ParsingByteCodeIfCondition") {
    std::vector<std::tuple<std::string, TProgram>> tests = {
        {input_if_1(), expected_if_1()},
        {input_if_2(), expected_if_2()},
        {input_if_3(), expected_if_3()},
        {input_if_4(), expected_if_4()},
        {input_if_5(), expected_if_5()},
    };

    for (const auto &[input, expected]: tests) {
        testByteCodeCore(input, expected);
    }
}

TEST_CASE("Test_ParsingByteCodeFunctionCall") {
    std::vector<std::tuple<std::string, TProgram>> tests = {
        {input_funcall_1(), expected_funcall_1()},
    };

    for (const auto &[input, expected]: tests) {
        testByteCodeCore(input, expected);
    }
}