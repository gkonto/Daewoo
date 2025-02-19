#ifndef EVAL_OBJECT_HPP_INCLUDED
#define EVAL_OBJECT_HPP_INCLUDED

#include <string>
#include <memory>
#include <variant>

enum class ObjType
{
    Integer,
    Boolean,
    Null,
    Return,
    Error,
};

struct EvalObject
{
    using Enclosed = std::shared_ptr<EvalObject>;
    int getInt() const { return std::get<int>(value); }
    bool getBool() const { return std::get<bool>(value); }
    Enclosed getObject() const { return std::get<Enclosed>(value); }
    std::string getString() const { return std::get<std::string>(value); }

    ObjType type = ObjType::Null;
    std::variant<int, bool, Enclosed, std::string> value;
};

std::string inspect(const EvalObject &o);
std::string typeStr(ObjType);

#endif