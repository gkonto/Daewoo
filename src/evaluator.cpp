#include "evaluator.hpp"
#include "ast.hpp"
#include "object.hpp"

std::shared_ptr<EvalObject> Eval(Node *n)
{
    return n->evaluate();
}