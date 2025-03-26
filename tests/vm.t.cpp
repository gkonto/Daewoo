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

template <typename T>
static void testVM(const std::string &input, TStackRecordType expected_type, T expected_value) {
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
    INFO(input + " Expected: Type>" + TStackRecordTypeToStr(expected_type) + " Value> " +
         std::to_string(expected_value));
    REQUIRE(result.type() == expected_type);

    if (result.type() == TStackRecordType::stInteger) {
        REQUIRE(result.ivalue() == expected_value);
    } else if (result.type() == TStackRecordType::stDouble) {
        REQUIRE(std::abs(result.dvalue() - expected_value) < 1e-6);
    } else if (result.type() == TStackRecordType::stBoolean) {
        REQUIRE(result.bvalue() == expected_value);
    } else {
        REQUIRE(false);
    }
}

TEST_CASE("Test_VM_arithmetic") {
    SECTION("Simple") {
        std::vector<std::tuple<std::string, TStackRecordType, double>> tests = {
            {"1E2",                 TStackRecordType::stDouble,  100     },
            {"1.2E2",               TStackRecordType::stDouble,  120     },
            {"0.1E2 ",              TStackRecordType::stDouble,  10      },
            {"1E-2 ",               TStackRecordType::stDouble,  0.01    },
            {"1.2E-2 ",             TStackRecordType::stDouble,  0.012   },
            {"-1E-2 ",              TStackRecordType::stDouble,  -0.01   },
            {"2 + 3 ",              TStackRecordType::stInteger, 5       },
            {"2 + 3.0 ",            TStackRecordType::stDouble,  5       },
            {"2 - 3 ",              TStackRecordType::stInteger, -1      },
            {"3 - 2 ",              TStackRecordType::stInteger, 1       },
            {"3 - 2.0 ",            TStackRecordType::stDouble,  1       },
            {"2 * 3 ",              TStackRecordType::stInteger, 6       },
            {"2 * 3.0 ",            TStackRecordType::stDouble,  6       },
            {"10 / 2 ",             TStackRecordType::stInteger, 5       },
            {"10 / 2.0",            TStackRecordType::stDouble,  5       },
            {"2 ^ 4 ",              TStackRecordType::stDouble,  16      },
            {"let a = 123.456; a",  TStackRecordType::stDouble,  123.456 },
            {"let a = 0.00001; a",  TStackRecordType::stDouble,  0.00001 },
            {"let a = -0.00001; a", TStackRecordType::stDouble,  -0.00001},
            {"let a = 123.; a",     TStackRecordType::stDouble,  123     },
            {"let a = .987; a",     TStackRecordType::stDouble,  0.987   },
        };

        for (const auto &[input, expected_type, expected_value]: tests) {
            testVM(input, expected_type, expected_value);
        }
    }

    SECTION("Boolean") {
        std::vector<std::tuple<std::string, TStackRecordType, bool>> tests = {
            {"let a = 2; a == 2",           TStackRecordType::stBoolean, true },
            {"let a = 2; let b = 5;b == 3", TStackRecordType::stBoolean, false},
            {"false == (2 < 1)",            TStackRecordType::stBoolean, true },
        };
        for (const auto &[input, expected_type, expected_value]: tests) {
            testVM(input, expected_type, expected_value);
        }
    }

    SECTION("Integer") {
        std::vector<std::tuple<std::string, TStackRecordType, int>> tests = {
            {"let a = 2; let b = 5; a",     TStackRecordType::stInteger, 2 },
            {"let a = 2; let b = 5; b",     TStackRecordType::stInteger, 5 },
            {"let a = 2; let b = 5; a-b",   TStackRecordType::stInteger, -3},
            {"let a = 2; let b = 5; b-a",   TStackRecordType::stInteger, 3 },
            {"let a = 2; let b = 5; a * b", TStackRecordType::stInteger, 10},
            {"let a = 2; let b = 5; b * a", TStackRecordType::stInteger, 10},
            {"let a = 2; let b = 5; b / a", TStackRecordType::stInteger, 2 },
            {"let a = 2; let b = 5; a / b", TStackRecordType::stInteger, 0 },
        };

        for (const auto &[input, expected_type, expected_value]: tests) {
            testVM(input, expected_type, expected_value);
        }
    }

    SECTION("Float") {
        std::vector<std::tuple<std::string, TStackRecordType, double>> tests = {
            {"let a = 2.0; let b = 5.0; a",                     TStackRecordType::stDouble, 2      },
            {"let a = 2; let b = 5.0; b",                       TStackRecordType::stDouble, 5      },
            {"let a = 2; let b = 5.0; a-b",                     TStackRecordType::stDouble, -3     },
            {"let a = 2; let b = 5.0; b-a",                     TStackRecordType::stDouble, 3      },
            {"let a = 2; let b = 5.0; a * b",                   TStackRecordType::stDouble, 10     },
            {"let a = 2; let b = 5.0; b * a",                   TStackRecordType::stDouble, 10     },
            {"let a = 2; let b = 5.0; b / a",                   TStackRecordType::stDouble, 2.5    },
            {"let a = 2; let b = 5.0; a / b",                   TStackRecordType::stDouble, 0.4    },
            {"let a = 2.5; let b = 5.5; a+b",                   TStackRecordType::stDouble, 8.0    },
            {"let a = 2.5; let b = 5.5; b+a",                   TStackRecordType::stDouble, 8.0    },
            {"let a = 2.5; let b = 5.5; a-b",                   TStackRecordType::stDouble, -3.0   },
            {"let a = 2.5; let b = 5.5; b-a",                   TStackRecordType::stDouble, 3.0    },
            {"let a = 2.5; let b = 5.5; a*b",                   TStackRecordType::stDouble, 13.75  },
            {"let a = 2.5; let b = 5.5; b*a",                   TStackRecordType::stDouble, 13.75  },
            {"let a = 2.5; let b = 5.5; b/a",                   TStackRecordType::stDouble, 2.2    },
            {"let a = 2.5; let b = 5.5; b = 7.5; a/b",          TStackRecordType::stDouble, 1 / 3.0},
            {"let a = 2.5; let b = 5.5; b = 2.0; a = 1.5; a^b", TStackRecordType::stDouble, 2.25   },
            {"8/2.5",                                           TStackRecordType::stDouble, 3.2    },
            {"2.5/8",                                           TStackRecordType::stDouble, 0.3125 },
        };

        for (const auto &[input, expected_type, expected_value]: tests) {
            testVM(input, expected_type, expected_value);
        }
    }

    SECTION("Precedence") {
        std::vector<std::tuple<std::string, TStackRecordType, int>> tests = {
            {"5 * 4 + 2",   TStackRecordType::stInteger, 22},
            {"2 + 5 * 4",   TStackRecordType::stInteger, 22},
            {"(2 + 5) * 4", TStackRecordType::stInteger, 28},
            {"2 + 6 / 3",   TStackRecordType::stInteger, 4 },
            {"2 - 6 / 3",   TStackRecordType::stInteger, 0 },
        };

        for (const auto &[input, expected_type, expected_value]: tests) {
            testVM(input, expected_type, expected_value);
        }
    }
}
