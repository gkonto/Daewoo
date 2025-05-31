#include "config.hpp"
#include "repl.hpp"
#include <iostream>

int main(void)
{
    std::cout << "Welcome to " << project_name << " " << project_version_major
              << "." << project_version_minor << "." << project_version_patch
              << std::endl;
    std::cout << "Feel free to type anything!" << std::endl;

    Repl repl;
    repl.start();

    return 0;
}