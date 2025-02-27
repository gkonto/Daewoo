#ifndef EVAL_OBJECT_HPP_INCLUDED
#define EVAL_OBJECT_HPP_INCLUDED

#include <string>
#include <memory>
#include <variant>
#include <vector>
#include "macros.hpp"

class Environment;
class BlockStatement;
class Identifier;

enum class ObjType
{
    Integer,
    Boolean,
    Null,
    Return,
    Error,
    Function,
};

class Function
{
public:
    Function(const std::vector<__Ptr<Identifier>> &params, __Ptr<BlockStatement> &body, __Ptr<Environment> env)
        : parameters_(params), body_(body), env_(env) {}
    size_t parametersCount() const { return parameters_.size(); }
    const std::vector<__Ptr<Identifier>> &parameters() const { return parameters_; }
    const __Ptr<BlockStatement> &body() const { return body_; }
    __Ptr<Environment> environment() const { return env_; }

private:
    std::vector<__Ptr<Identifier>> parameters_;
    __Ptr<BlockStatement> body_;
    __Ptr<Environment> env_;
};

struct EvalObject
{
    using Enclosed = __Ptr<EvalObject>;

    int getInt() const { return std::get<int>(value); }
    bool getBool() const { return std::get<bool>(value); }
    Enclosed getObject() const { return std::get<Enclosed>(value); }
    std::string getString() const { return std::get<std::string>(value); }

    Function getFunction() const { return std::get<Function>(value); }

    ObjType type = ObjType::Null;
    std::variant<int, bool, Enclosed, std::string, Function> value;
};

std::string inspect(const EvalObject &o);
std::string typeStr(ObjType);

#endif