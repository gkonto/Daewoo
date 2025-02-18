#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include "token.hpp"
#include "lexer.hpp"
#include <vector>

struct NextTokenExpected
{
    Token::Type type_;
    std::string literal_;
    BufferSize position_;
    Length length_;
};

TEST_CASE("Test_NextToken")
{
    std::string input("=+(){},;");

    std::vector<NextTokenExpected> expected_tokens{
        {Token::Type::Assign, "=", 0, 1},
        {Token::Type::Plus, "+", 1, 1},
        {Token::Type::LParen, "(", 2, 1},
        {Token::Type::RParen, ")", 3, 1},
        {Token::Type::LBrace, "{", 4, 1},
        {Token::Type::RBrace, "}", 5, 1},
        {Token::Type::Comma, ",", 6, 1},
        {Token::Type::Semicolon, ";", 7, 1},
        {Token::Type::Eof, "", 8, 0},
    };

    Lexer lexer(input);
    for (const auto &expected_token : expected_tokens)
    {
        auto got = lexer.nextToken();
        REQUIRE(got.type_ == expected_token.type_);
        REQUIRE(lexer.getLiteral(got) == expected_token.literal_);
        REQUIRE(got.position_ == expected_token.position_);
        REQUIRE(got.length_ == expected_token.length_);
    }
}

TEST_CASE("Test_NextToken2")
{
    std::string input("let five = 5;\
        let ten = 10;\
        let add = fn(x, y) {\
            x + y;\
        };\
        let result = add(five, ten);\
        !-/*5;\
        5 < 10 > 5;\
        if (5 < 10) {\
            return true;\
        } else {\
            return false;\
        }\
        \
        10 == 10;\
        10 != 9;");

    std::vector<NextTokenExpected> expected_tokens{
        {Token::Type::Let, "let", 0, 3},
        {Token::Type::Ident, "five", 4, 4},
        {Token::Type::Assign, "=", 9, 1},
        {Token::Type::Int, "5", 11, 1},
        {Token::Type::Semicolon, ";", 12, 1},
        {Token::Type::Let, "let", 21, 3},
        {Token::Type::Ident, "ten", 25, 3},
        {Token::Type::Assign, "=", 29, 1},
        {Token::Type::Int, "10", 31, 2},
        {Token::Type::Semicolon, ";", 33, 1},
        {Token::Type::Let, "let", 42, 3},
        {Token::Type::Ident, "add", 46, 3},
        {Token::Type::Assign, "=", 50, 1},
        {Token::Type::Function, "fn", 52, 2},
        {Token::Type::LParen, "(", 54, 1},
        {Token::Type::Ident, "x", 55, 1},
        {Token::Type::Comma, ",", 56, 1},
        {Token::Type::Ident, "y", 58, 1},
        {Token::Type::RParen, ")", 59, 1},
        {Token::Type::LBrace, "{", 61, 1},
        {Token::Type::Ident, "x", 74, 1},
        {Token::Type::Plus, "+", 76, 1},
        {Token::Type::Ident, "y", 78, 1},
        {Token::Type::Semicolon, ";", 79, 1},
        {Token::Type::RBrace, "}", 88, 1},
        {Token::Type::Semicolon, ";", 89, 1},
        {Token::Type::Let, "let", 98, 3},
        {Token::Type::Ident, "result", 102, 6},
        {Token::Type::Assign, "=", 109, 1},
        {Token::Type::Ident, "add", 111, 3},
        {Token::Type::LParen, "(", 114, 1},
        {Token::Type::Ident, "five", 115, 4},
        {Token::Type::Comma, ",", 119, 1},
        {Token::Type::Ident, "ten", 121, 3},
        {Token::Type::RParen, ")", 124, 1},
        {Token::Type::Semicolon, ";", 125, 1},
        {Token::Type::Bang, "!", 134, 1},
        {Token::Type::Minus, "-", 135, 1},
        {Token::Type::Slash, "/", 136, 1},
        {Token::Type::Asterisk, "*", 137, 1},
        {Token::Type::Int, "5", 138, 1},
        {Token::Type::Semicolon, ";", 139, 1},
        {Token::Type::Int, "5", 148, 1},
        {Token::Type::LessThan, "<", 150, 1},
        {Token::Type::Int, "10", 152, 2},
        {Token::Type::GreaterThan, ">", 155, 1},
        {Token::Type::Int, "5", 157, 1},
        {Token::Type::Semicolon, ";", 158, 1},
        {Token::Type::If, "if", 167, 2},
        {Token::Type::LParen, "(", 170, 1},
        {Token::Type::Int, "5", 171, 1},
        {Token::Type::LessThan, "<", 173, 1},
        {Token::Type::Int, "10", 175, 2},
        {Token::Type::RParen, ")", 177, 1},
        {Token::Type::LBrace, "{", 179, 1},
        {Token::Type::Return, "return", 192, 6},
        {Token::Type::True, "true", 199, 4},
        {Token::Type::Semicolon, ";", 203, 1},
        {Token::Type::RBrace, "}", 212, 1},
        {Token::Type::Else, "else", 214, 4},
        {Token::Type::LBrace, "{", 219, 1},
        {Token::Type::Return, "return", 232, 6},
        {Token::Type::False, "false", 239, 5},
        {Token::Type::Semicolon, ";", 244, 1},
        {Token::Type::RBrace, "}", 253, 1},
        {Token::Type::Int, "10", 270, 2},
        {Token::Type::EQ, "==", 273, 2},
        {Token::Type::Int, "10", 276, 2},
        {Token::Type::Semicolon, ";", 278, 1},
        {Token::Type::Int, "10", 287, 2},
        {Token::Type::NotEQ, "!=", 290, 2},
        {Token::Type::Int, "9", 293, 1},
        {Token::Type::Semicolon, ";", 294, 1},
        {Token::Type::Eof, "", 295, 0}};

    Lexer lexer(input);
    for (const auto &expected_token : expected_tokens)
    {
        auto got = lexer.nextToken();
        INFO("Type    : Got - Expected: " << tokenType2String(got.type_) << " - " << tokenType2String(expected_token.type_)); // Additional info
        REQUIRE(got.type_ == expected_token.type_);
        INFO("Literal : Got - Expected: " << lexer.getLiteral(got) << " - " << expected_token.literal_); // Additional info
        REQUIRE(lexer.getLiteral(got) == expected_token.literal_);
        INFO("Position: Got - Expected: " << got.position_ << " - " << expected_token.position_); // Additional info
        REQUIRE(got.position_ == expected_token.position_);
        INFO("Length: Got - Expected: " << got.length_ << " - " << expected_token.length_); // Additional info
        REQUIRE(got.length_ == expected_token.length_);
    }
}