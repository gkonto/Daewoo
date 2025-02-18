#ifndef EVALUATOR_HPP_INCLUDED
#define EVALUATOR_HPP_INCLUDED

#include <memory>
#include "object.hpp"

class Node;

std::shared_ptr<EvalObject> Eval(Node *n);

#endif