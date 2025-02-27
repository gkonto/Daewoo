#ifndef AST_HPP_INCLUDED
#define AST_HPP_INCLUDED

#include <vector>
#include <string>
#include <memory>
#include <cassert>

#include "object.hpp"

class Identifier;
class Environment;

__Ptr<EvalObject> nativeBoolToBooleanObject(bool value);

class Node
{
public:
    virtual std::string toString() const = 0;
    virtual ~Node() = default;
    virtual __Ptr<EvalObject> evaluate(__Ptr<Environment> env) = 0;
};

class Statement : public Node
{
public:
    virtual ~Statement() = default;
};

class Expression : public Node
{
public:
    virtual ~Expression() = default;
};

class ExpressionStatement : public Statement
{
public:
    explicit ExpressionStatement(__Ptr<Expression> &&exp)
        : expression_(std::move(exp)) {}
    const Expression *expression() const { return expression_.get(); }
    std::string toString() const override { return expression_->toString(); }
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override { return expression_->evaluate(env); }

private:
    __Ptr<Expression> expression_ = nullptr;
};

class Program : public Node
{
public:
    using Statements = std::vector<__Ptr<Statement>>;
    size_t size() const { return statements_.size(); }
    Statements::const_iterator begin() const { return statements_.begin(); }
    Statements::const_iterator end() const { return statements_.end(); };
    const Statement *at(size_t i) const { return statements_[i].get(); }
    void add(__Ptr<Statement> &&s) { statements_.emplace_back(std::move(s)); }
    std::string toString() const override;
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    Statements statements_;
};

class Identifier : public Expression
{
public:
    explicit Identifier(const std::string &value)
        : value_(value) {}

    const std::string &value() const { return value_; }
    std::string toString() const override { return value_; }
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    std::string value_;
    // static constexpr size_t max_length = 15; // TODO consider allowing smaller length values.
    // char value_[max_length + 1] = {};
};

class IntegerLiteral : public Expression
{
public:
    explicit IntegerLiteral(int v)
        : value_(v) {}
    int value() const { return value_; }
    std::string toString() const override { return std::to_string(value_); }
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    int value_ = 0;
};

class LetStatement : public Statement
{
public:
    explicit LetStatement(const std::string &name, __Ptr<Expression> value)
        : name_(std::make_shared<Identifier>(name)), value_(std::move(value)) {}
    const std::string &name() const { return name_->value(); }
    std::string toString() const override;
    const Expression *value() const { return value_.get(); }
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    __Ptr<Identifier> name_ = nullptr;
    __Ptr<Expression> value_ = nullptr;
};

class ReturnStatement : public Statement
{
public:
    explicit ReturnStatement(__Ptr<Expression> ret_value)
        : return_value_(std::move(ret_value)) {}
    std::string toString() const override;
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    __Ptr<Expression> return_value_ = nullptr;
};

class PrefixExpression : public Expression
{
public:
    PrefixExpression(const std::string &op, __Ptr<Expression> right)
        : operator_(op), right_(std::move(right)) {}
    const std::string &op() const { return operator_; }
    const Expression *right() const { return right_.get(); }
    std::string toString() const override;
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    std::string operator_;
    __Ptr<Expression> right_ = nullptr;
};

class InfixExpression : public Expression
{
public:
    InfixExpression(__Ptr<Expression> lhs, const std::string &op, __Ptr<Expression> rhs)
        : lhs_(std::move(lhs)), operator_(op), rhs_(std::move(rhs)) {}
    const Expression *left() const { return lhs_.get(); }
    const Expression *right() const { return rhs_.get(); }
    const std::string op() const { return operator_; }
    std::string toString() const override;
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    __Ptr<Expression> lhs_;
    std::string operator_;
    __Ptr<Expression> rhs_;
};

class Boolean : public Expression
{
public:
    explicit Boolean(bool value)
        : value_(value) {}
    bool value() const { return value_; }
    std::string toString() const override { return value_ ? "true" : "false"; }
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override { return nativeBoolToBooleanObject(value_); }

private:
    bool value_;
};

class BlockStatement : public Statement
{
public:
    explicit BlockStatement(std::vector<__Ptr<Statement>> statements)
        : statements_(std::move(statements)) {}
    std::string toString() const;
    size_t size() const { return statements_.size(); }
    const Statement *at(size_t i) const { return statements_.at(i).get(); }
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    std::vector<__Ptr<Statement>> statements_;
};

class IfExpression : public Expression
{
public:
    IfExpression(__Ptr<Expression> condition,
                 __Ptr<BlockStatement> consequence,
                 __Ptr<BlockStatement> alternative)
        : condition_(std::move(condition)), consequence_(std::move(consequence)),
          alternative_(std::move(alternative)) {}
    std::string toString() const override;
    const Expression *condition() const { return condition_.get(); }
    const BlockStatement *consequence() const { return consequence_.get(); }
    const BlockStatement *alternative() const { return alternative_.get(); }
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    __Ptr<Expression> condition_;
    __Ptr<BlockStatement> consequence_;
    __Ptr<BlockStatement> alternative_;
};

class FunctionLiteral : public Expression
{
public:
    FunctionLiteral(__Ptr<BlockStatement> body, std::vector<__Ptr<Identifier>> parameters)
        : body_(std::move(body)), parameters_(std::move(parameters)) {}
    std::string toString() const override;
    size_t paramsSize() const { return parameters_.size(); }
    const Identifier *param(size_t i) const { return parameters_.at(i).get(); }
    size_t size() const { return body_->size(); }
    const BlockStatement *body() const { return body_.get(); }
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    __Ptr<BlockStatement> body_;
    std::vector<__Ptr<Identifier>> parameters_;
};

class CallExpression : public Expression
{
public:
    CallExpression(__Ptr<Expression> function, std::vector<__Ptr<Expression>> args)
        : function_(std::move(function)), arguments_(std::move(args)) {}
    std::string toString() const override;
    const Expression *function() const { return function_.get(); }
    size_t argsSize() const { return arguments_.size(); }
    const Expression *argument(size_t i) const { return arguments_.at(i).get(); }
    __Ptr<EvalObject> evaluate(__Ptr<Environment> env) override;

private:
    __Ptr<Expression> function_;
    std::vector<__Ptr<Expression>> arguments_;
};

#endif