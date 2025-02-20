#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"
#include "environment.hpp"

__Ptr<EvalObject> Eval(Node *n, __Ptr<Environment> env)
{
    return n->evaluate(env);
}