#include <iostream>
#include <memory>
#include <chrono>
#include <sstream>
#include "repl.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "environment.hpp"
#include "evaluator.hpp"
#include "ast.hpp"
#include "VM.hpp"
#include "TByteCodeBuilder.hpp"
#include "SyntaxParser.hpp"

struct BenchmarkCase
{
    BenchmarkCase(std::string n, std::string i)
        : name(std::move(n)), input(std::move(i)) {}
    std::string name;
    std::string input;
};

static std::string inputFibonacci35() {
    return "fn fibonacci(n)\n"
        "    if n < 2 then\n"
        "        return n\n"
        "    end\n"
        "    return fibonacci(n - 1) + fibonacci(n - 2)\n"
        "end;\n"
        "\n"
        "fibonacci(35);\n";
}

static std::string inputFibonacci33() {
    return "fn fibonacci(n)\n"
        "    if n < 2 then\n"
        "        return n\n"
        "    end\n"
        "    return fibonacci(n - 1) + fibonacci(n - 2)\n"
        "end;\n"
        "\n"
        "fibonacci(33);\n";
}

static void VM_fibonacci35()
{
    auto start = std::chrono::high_resolution_clock::now();

    std::istringstream iss(inputFibonacci35());
    Scanner sc(iss);
    SyntaxParser sp(sc);
    auto err = sp.syntaxCheck();
 
    TByteCodeBuilder builder(sp.tokens());
    auto module = std::make_shared<TModule>();
    constantValueTable.clear();
    builder.build(module.get());

    VM vm;
    vm.runModule(module);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_us = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();

    long minutes = duration_us / 60'000;
    long seconds = (duration_us % 60'000) / 1'000;
    long milliseconds = duration_us % 1'000;

    std::cout << "fibonacci(35) - Execution time: [" << minutes << ":" << seconds << ":" << milliseconds << "] [m:s:ms]" << std::endl;
}


static void VM_fibonacci33()
{
    auto start = std::chrono::high_resolution_clock::now();

    std::istringstream iss(inputFibonacci33());
    Scanner sc(iss);
    SyntaxParser sp(sc);
    auto err = sp.syntaxCheck();

    TByteCodeBuilder builder(sp.tokens());
    auto module = std::make_shared<TModule>();
    constantValueTable.clear();
    builder.build(module.get());

    VM vm;
    vm.runModule(module);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_us = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();

    long minutes = duration_us / 60'000;
    long seconds = (duration_us % 60'000) / 1'000;
    long milliseconds = duration_us % 1'000;

    std::cout << "fibonacci(33) - Execution time: [" << minutes << ":" << seconds << ":" << milliseconds << "] [m:s:ms]" << std::endl;
}

int main(void)
{
    VM_fibonacci35();
    VM_fibonacci33();
    
    return 0;
}