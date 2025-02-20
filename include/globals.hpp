#ifndef GLOBALS_HPP_INCLUDED
#define GLOBALS_HPP_INCLUDED

#include <memory>
#include "macros.hpp"

struct EvalObject;

class Globals
{
public:
    static Globals &getInstance();
    __Ptr<EvalObject> getTrue() const;
    __Ptr<EvalObject> getFalse() const;
    __Ptr<EvalObject> getNull() const;

private:
    Globals();
    __Ptr<EvalObject> TRUE;
    __Ptr<EvalObject> FALSE;
    __Ptr<EvalObject> NIL;
};

#endif