#include <iostream>
#include <memory>
#include <chrono>
#include "repl.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "environment.hpp"
#include "evaluator.hpp"
#include "ast.hpp"

struct BenchmarkCase
{
    BenchmarkCase(std::string n, std::string i)
        : name(std::move(n)), input(std::move(i)) {}
    std::string name;
    std::string input;
};

static void runBenchmark(const std::string &input)
{
    Lexer l(input);
    Parser p(l);
    auto program = p.parseProgram();
    auto env = std::make_shared<Environment>();
    auto result = Eval(program.get(), env);
    std::cout << "Result        : " << result->getInt() << std::endl;
}

int main(void)
{
    std::vector<BenchmarkCase> tests =
        {
            {"fibonacci(33)", "let fibonacci = fn(x) { if (x == 0) { 0 } else { if (x == 1) { 1 } else { fibonacci(x - 1) + fibonacci(x - 2); } } }; fibonacci(33);"}};

    for (const auto &[name, input] : tests)
    {
        std::cout
            << "Evaluating input: " << name << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        runBenchmark(input);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration_us = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();

        long minutes = duration_us / 60'000;
        long seconds = (duration_us % 60'000) / 1'000;
        long milliseconds = duration_us % 1'000;

        std::cout << "Execution time: [" << minutes << ":" << seconds << ":" << milliseconds << "] [m:s:ms]" << std::endl;
    }

    return 0;
}