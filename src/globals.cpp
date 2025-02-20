#include "globals.hpp"
#include "object.hpp"

Globals::Globals()
{
    TRUE = std::make_shared<EvalObject>();
    TRUE->type = ObjType::Boolean;
    TRUE->value = true;

    FALSE = std::make_shared<EvalObject>();
    FALSE->type = ObjType::Boolean;
    FALSE->value = false;

    NIL = std::make_shared<EvalObject>();
}

Globals &Globals::getInstance()
{
    static Globals globals;
    return globals;
}

__Ptr<EvalObject> Globals::getTrue() const
{
    return TRUE;
}

__Ptr<EvalObject> Globals::getFalse() const
{
    return FALSE;
}

__Ptr<EvalObject> Globals::getNull() const
{
    return NIL;
}
