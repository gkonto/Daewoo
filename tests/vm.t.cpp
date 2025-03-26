#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <tuple>
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
#include "VM.hpp"

static void checkSyntaxParserErrors(std::optional<SyntaxError> error) {
    INFO("SyntaxError found: " << (error.has_value() ? error.value().msg() : ""));
    REQUIRE(!error.has_value());
}

static void
testVM(const std::string &input, TStackRecordType expected_type, double expected_value) {
    std::istringstream iss(input);
    Scanner sc(iss);
    SyntaxParser sp(sc);
    auto err = sp.syntaxCheck();
    checkSyntaxParserErrors(err);
    TByteCodeBuilder builder(sp.tokens());
    auto module = std::make_shared<TModule>();
    constantValueTable.clear();
    builder.build(module.get());

    VM vm(10);
    vm.runModule(module);
    const auto &result = vm.top();
    REQUIRE(result.type() == expected_type);
    if (result.type() == TStackRecordType::stInteger) {
        REQUIRE(result.ivalue() == expected_value);
    } else if (result.type() == TStackRecordType::stDouble) {
        REQUIRE(result.dvalue() == expected_value);
    } else {
        REQUIRE(false);
    }
}

TEST_CASE("Test_VM_arithmetic") {
    std::vector<std::tuple<std::string, TStackRecordType, double>> tests = {
        {"1E2",      TStackRecordType::stDouble,  100  },
        {"1.2E2",    TStackRecordType::stDouble,  120  },
        {"0.1E2 ",   TStackRecordType::stDouble,  10   },
        {"1E-2 ",    TStackRecordType::stDouble,  0.01 },
        {"1.2E-2 ",  TStackRecordType::stDouble,  0.012},
        {"-1E-2 ",   TStackRecordType::stDouble,  -0.01},
        {"2 + 3 ",   TStackRecordType::stInteger, 5    },
        {"2 + 3.0 ", TStackRecordType::stDouble,  5    },
        {"2 - 3 ",   TStackRecordType::stInteger, -1   },
        {"3 - 2 ",   TStackRecordType::stInteger, 1    },
        {"3 - 2.0 ", TStackRecordType::stDouble,  1    },
        {"2 * 3 ",   TStackRecordType::stInteger, 6    },
        {"2 * 3.0 ", TStackRecordType::stDouble,  6    },
        {"10 / 2 ",  TStackRecordType::stInteger, 5    },
        {"10 / 2.0", TStackRecordType::stDouble,  5    },
        {"2 ^ 4 ",   TStackRecordType::stDouble,  16   },
    };

    for (const auto &[input, expected_type, expected_value]: tests) {
        testVM(input, expected_type, expected_value);
    }
}

TEST_CASE("Test_VM_IntegerOperations") {
    std::vector<std::tuple<std::string, TStackRecordType, bool>> tests = {
        //{"let a = 2; a == 2", TStackRecordType::stBoolean, true},
        // {"let a = 2; let b = 5;b == 5", },
        // {"let a = 2; let b = 5; a + b == 7", },
        // {"let a = 2; let b = 5; b + a == 7", },
        // {"let a = 2; let b = 5; a - b == -3", },
        // {"let a = 2; let b = 5; b - a == 3", },
        // {"let a = 2; let b = 5; a * b == 10", },
        // {"let a = 2; let b = 5; b * a == 10", },
        // {"let a = 2; let b = 5; b / a == 2.5", },
        // {"let a = 2; let b = 5; a / b == 0.4", },
        // {"let a = 2; let b = 5; b div a == 2"},
        // {"let a = 2; let b = 5; b mod 1 == 0", },
        // {"let a = 2; let b = 5; b mod 2 == 1", },
        // {"let a = 2; let b = 5; b mod 3 == 2", },
    };

    for (const auto &[input, expected_type, expected_value]: tests) {
        testVM(input, expected_type, expected_value);
    }
}