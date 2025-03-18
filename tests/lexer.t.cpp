#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include "Token.hpp"
#include "lexer.hpp"
#include <vector>
#include <sstream>

struct NextTokenExpected {
    TokenCode type_;
    std::string literal_;
    int columnNumber_;
    int lineNumber_;
};

TEST_CASE("Test_NextToken") {
    std::string input("=+(){},;");
    std::vector<NextTokenExpected> expected_tokens{
        {TokenCode::tAssign, "=", 1, 1},
        {TokenCode::tPlus, "+", 2, 1},
        {TokenCode::tLeftParenthesis, "(", 3, 1},
        {TokenCode::tRightParenthesis, ")", 4, 1},
        {TokenCode::tLeftCurleyBracket, "{", 5, 1},
        {TokenCode::tRightCurleyBracket, "}", 6, 1},
        {TokenCode::tComma, ",", 7, 1},
        {TokenCode::tSemicolon, ";", 8, 1},
        {TokenCode::tEndofStream, "<eof>", 9, 1},
    };

    std::istringstream iss(input);
    Scanner scanner(iss);
    for (const auto &expected_token: expected_tokens) {
        scanner.nextToken();
        auto got = scanner.token();
        REQUIRE(got.code() == expected_token.type_);
        REQUIRE(got.literal() == expected_token.literal_);
        REQUIRE(got.columnNumber() == expected_token.columnNumber_);
        REQUIRE(got.lineNumber() == expected_token.lineNumber_);
    }
}

TEST_CASE("Test_NextToken2") {
    std::string input(
        "let five = 5;\
        let ten = 10;\
        let add = fn(x, y) {\
            x + y;\
        };\
        let result = add(five, ten);\
        >-/)5;\
        5 < 10 > 5;\
        if (5 < 10) {\
            return true;\
        } else {\
            return false;\
        }\
        \
        10 == 10;\
        10 != 9;");

    std::vector<NextTokenExpected> expected_tokens{{TokenCode::tLet, "<let>", 0, 1},
                                                   {TokenCode::tIdentifier, "five", 4, 1},
                                                   {TokenCode::tAssign, "=", 9, 1},
                                                   {TokenCode::tInteger, "5", 11, 1},
                                                   {TokenCode::tSemicolon, ";", 12, 1},
                                                   {TokenCode::tLet, "<let>", 21, 1},
                                                   {TokenCode::tIdentifier, "ten", 25, 1},
                                                   {TokenCode::tAssign, "=", 29, 1},
                                                   {TokenCode::tInteger, "10", 31, 1},
                                                   {TokenCode::tSemicolon, ";", 33, 1},
                                                   {TokenCode::tLet, "<let>", 42, 1},
                                                   {TokenCode::tIdentifier, "add", 46, 1},
                                                   {TokenCode::tAssign, "=", 50, 1},
                                                   {TokenCode::tFunction, "<fn>", 52, 1},
                                                   {TokenCode::tLeftParenthesis, "(", 54, 1},
                                                   {TokenCode::tIdentifier, "x", 55, 1},
                                                   {TokenCode::tComma, ",", 56, 1},
                                                   {TokenCode::tIdentifier, "y", 58, 1},
                                                   {TokenCode::tRightParenthesis, ")", 59, 1},
                                                   {TokenCode::tLeftCurleyBracket, "{", 61, 1},
                                                   {TokenCode::tIdentifier, "x", 74, 1},
                                                   {TokenCode::tPlus, "+", 76, 1},
                                                   {TokenCode::tIdentifier, "y", 78, 1},
                                                   {TokenCode::tSemicolon, ";", 79, 1},
                                                   {TokenCode::tRightCurleyBracket, "}", 88, 1},
                                                   {TokenCode::tSemicolon, ";", 89, 1},
                                                   {TokenCode::tLet, "<let>", 98, 1},
                                                   {TokenCode::tIdentifier, "result", 102, 1},
                                                   {TokenCode::tAssign, "=", 109, 1},
                                                   {TokenCode::tIdentifier, "add", 111, 1},
                                                   {TokenCode::tLeftParenthesis, "(", 114, 1},
                                                   {TokenCode::tIdentifier, "five", 115, 1},
                                                   {TokenCode::tComma, ",", 119, 1},
                                                   {TokenCode::tIdentifier, "ten", 121, 1},
                                                   {TokenCode::tRightParenthesis, ")", 124, 1},
                                                   {TokenCode::tSemicolon, ";", 125, 1},
                                                   {TokenCode::tMoreThan, ">", 134, 1},
                                                   {TokenCode::tMinus, "-", 135, 1},
                                                   {TokenCode::tDivide, "/", 136, 1},
                                                   {TokenCode::tRightParenthesis, ")", 137, 1},
                                                   {TokenCode::tInteger, "5", 138, 1},
                                                   {TokenCode::tSemicolon, ";", 139, 1},
                                                   {TokenCode::tInteger, "5", 148, 1},
                                                   {TokenCode::tLessThan, "<", 150, 1},
                                                   {TokenCode::tInteger, "10", 152, 1},
                                                   {TokenCode::tMoreThan, ">", 155, 1},
                                                   {TokenCode::tInteger, "5", 157, 1},
                                                   {TokenCode::tSemicolon, ";", 158, 1},
                                                   {TokenCode::tIf, "<if>", 167, 1},
                                                   {TokenCode::tLeftParenthesis, "(", 170, 1},
                                                   {TokenCode::tInteger, "5", 171, 1},
                                                   {TokenCode::tLessThan, "<", 173, 1},
                                                   {TokenCode::tInteger, "10", 175, 1},
                                                   {TokenCode::tRightParenthesis, ")", 177, 1},
                                                   {TokenCode::tLeftCurleyBracket, "{", 179, 1},
                                                   {TokenCode::tReturn, "<return>", 192, 1},
                                                   {TokenCode::tTrue, "<true>", 199, 1},
                                                   {TokenCode::tSemicolon, ";", 203, 1},
                                                   {TokenCode::tRightCurleyBracket, "}", 212, 1},
                                                   {TokenCode::tElse, "<else>", 214, 1},
                                                   {TokenCode::tLeftCurleyBracket, "{", 219, 1},
                                                   {TokenCode::tReturn, "<return>", 232, 1},
                                                   {TokenCode::tFalse, "<false>", 239, 1},
                                                   {TokenCode::tSemicolon, ";", 244, 1},
                                                   {TokenCode::tRightCurleyBracket, "}", 253, 1},
                                                   {TokenCode::tInteger, "10", 270, 1},
                                                   {TokenCode::tEquivalence, "==", 273, 1},
                                                   {TokenCode::tInteger, "10", 276, 1},
                                                   {TokenCode::tSemicolon, ";", 278, 1},
                                                   {TokenCode::tInteger, "10", 287, 1},
                                                   {TokenCode::tNotEqual, "!=", 290, 1},
                                                   {TokenCode::tInteger, "9", 293, 1},
                                                   {TokenCode::tSemicolon, ";", 294, 1},
                                                   {TokenCode::tEndofStream, "<eof>", 295, 1}};

    std::istringstream iss(input);
    Scanner scanner(iss);
    for (const auto &expected_token: expected_tokens) {
        scanner.nextToken();
        auto got = scanner.token();
        REQUIRE(got.code() == expected_token.type_);
        REQUIRE(got.literal() == expected_token.literal_);
        REQUIRE(got.columnNumber() == expected_token.columnNumber_ + 1);
        REQUIRE(got.lineNumber() == expected_token.lineNumber_);
    }
}

TEST_CASE("Test_NextToken3") {
    std::string input(
        "variable // Identifier\n"
        "\"string\" // String\n"
        "  12345 // Integer\n"
        "variable \"string\" 12345\n"
        "variable \"stri\nng\" \n"
        "\"str int\"\n"
        "\"string\tTab Test\"\n"
        "/* Comment test\n end of comment*/\n variable 12345 \"string\""
        "string\n\nstring\n"
        "0123 00123");

    std::vector<NextTokenExpected> expected_tokens{
        {TokenCode::tIdentifier, "variable", 1, 1},  {TokenCode::tString, "string", 1, 2},
        {TokenCode::tInteger, "12345", 3, 3},        {TokenCode::tIdentifier, "variable", 1, 4},
        {TokenCode::tString, "string", 10, 4},       {TokenCode::tInteger, "12345", 19, 4},
        {TokenCode::tIdentifier, "variable", 1, 5},  {TokenCode::tString, "stri\nng", 10, 5},
        {TokenCode::tString, "str int", 1, 6},       {TokenCode::tString, "string\tTab Test", 1, 7},
        {TokenCode::tIdentifier, "variable", 2, 10}, {TokenCode::tInteger, "12345", 11, 10},
        {TokenCode::tString, "string", 17, 10},      {TokenCode::tIdentifier, "string", 25, 10},
        {TokenCode::tIdentifier, "string", 1, 12},   {TokenCode::tInteger, "123", 1, 13},
        {TokenCode::tInteger, "123", 6, 13}};

    std::istringstream iss(input);
    Scanner scanner(iss);
    for (const auto &expected_token: expected_tokens) {
        scanner.nextToken();
        auto got = scanner.token();
        REQUIRE(got.code() == expected_token.type_);
        REQUIRE(got.literal() == expected_token.literal_);
        REQUIRE(got.columnNumber() == expected_token.columnNumber_);
        REQUIRE(got.lineNumber() == expected_token.lineNumber_);
    }
}

TEST_CASE("Test_TokenIntegers") {
    std::vector<std::tuple<std::string, double, TokenCode>> tests{
        {"3.1215", 3.1215, TokenCode::tFloat},
        {"0.1415", 0.1415, TokenCode::tFloat},
        {".5", 0.5, TokenCode::tFloat},
        {"1E-1", 0.1, TokenCode::tFloat},
        {"1E+1", 10, TokenCode::tFloat},
        {"1E-3", 0.001, TokenCode::tFloat},
        {"1.234E-3", 0.001234, TokenCode::tFloat},
        {"0.234E-3", 0.000234, TokenCode::tFloat},
    };
    for (const auto &[input, expected, code]: tests) {
        std::istringstream iss(input);
        Scanner scanner(iss);
        scanner.nextToken();
        auto got = scanner.token();
        REQUIRE(got.code() == code);
        REQUIRE(std::abs(got.tFloat() - expected) < std::numeric_limits<double>::epsilon());
    }
}