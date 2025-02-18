#ifndef GLOBALS_HPP_INCLUDED
#define GLOBALS_HPP_INCLUDED

#include <memory>

struct EvalObject;

class Globals
{
public:
    static Globals &getInstance();
    std::shared_ptr<EvalObject> getTrue() const;
    std::shared_ptr<EvalObject> getFalse() const;
    std::shared_ptr<EvalObject> getNull() const;

private:
    Globals();
    std::shared_ptr<EvalObject> TRUE;
    std::shared_ptr<EvalObject> FALSE;
    std::shared_ptr<EvalObject> NIL;
};

#endif