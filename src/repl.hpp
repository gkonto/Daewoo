#ifndef REPL_HPP_INCLUDED
#define REPL_HPP_INCLUDED

#include <string>

class Repl
{
public:
    void start();

private:
    static constexpr const char *prompt = ">>> ";
};

#endif