
#include "evaluator.hpp"
#include "ast.hpp"
#include "environment.hpp"
#include "object.hpp"
#include <iostream>

__Ptr<EvalObject> Eval(Node *n, __Ptr<Environment> env)
{
    return n->evaluate(env);
}