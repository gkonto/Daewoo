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

std::shared_ptr<EvalObject> Globals::getTrue() const
{
    return TRUE;
}

std::shared_ptr<EvalObject> Globals::getFalse() const
{
    return FALSE;
}

std::shared_ptr<EvalObject> Globals::getNull() const
{
    return NIL;
}
