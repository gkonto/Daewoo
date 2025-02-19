#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"
#include "environment.hpp"

std::shared_ptr<EvalObject> Eval(Node *n, Environment *env)
{
    return n->evaluate(env);
}