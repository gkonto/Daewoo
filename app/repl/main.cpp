#include <iostream>
#include "repl.hpp"

int main(void)
{
    std::cout << "Welcome to Monkey REPL. Feel free to type anything" << std::endl;

    Repl repl;
    repl.start();

    return 0;
}