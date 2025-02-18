#ifndef AST_HPP_INCLUDED
#define AST_HPP_INCLUDED

#include <vector>
#include <string>
#include <memory>

#include "object.hpp"

class Identifier;

class Node
{
public:
    virtual std::string toString() const = 0;
    virtual ~Node() = default;
    virtual std::shared_ptr<EvalObject> evaluate() { return nullptr; }
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
    explicit ExpressionStatement(std::unique_ptr<Expression> &&exp);
    const Expression *expression() const { return expression_.get(); }
    std::string toString() const override { return expression_->toString(); }
    std::shared_ptr<EvalObject> evaluate() override;

private:
    std::unique_ptr<Expression> expression_ = nullptr;
};

class Program : public Node
{
public:
    using Statements = std::vector<std::unique_ptr<Statement>>;
    size_t size() const;
    Statements::const_iterator begin() const;
    Statements::const_iterator end() const;
    const Statement *at(size_t i) const { return statements_[i].get(); }
    void add(std::unique_ptr<Statement> &&s);
    std::string toString() const override;
    std::shared_ptr<EvalObject> evaluate() override;

private:
    Statements statements_;
};

class LetStatement : public Statement
{
public:
    explicit LetStatement(const std::string &name, std::unique_ptr<Expression> value);
    const std::string &name() const;
    std::string toString() const override;
    const Expression *value() const { return value_.get(); }

private:
    std::unique_ptr<Identifier> name_ = nullptr;
    std::unique_ptr<Expression> value_ = nullptr;
};

class Identifier : public Expression
{
public:
    explicit Identifier(const std::string &value);
    const std::string &value() const { return value_; }
    std::string toString() const override;

private:
    std::string value_;
    // static constexpr size_t max_length = 15; // TODO consider allowing smaller length values.
    // char value_[max_length + 1] = {};
};

class ReturnStatement : public Statement
{
public:
    explicit ReturnStatement(std::unique_ptr<Expression> ret_value);
    std::string toString() const override;
    std::shared_ptr<EvalObject> evaluate() override;

private:
    std::unique_ptr<Expression> return_value_ = nullptr;
};

class IntegerLiteral : public Expression
{
public:
    explicit IntegerLiteral(int v);
    int value() const { return value_; }
    std::string toString() const override;
    std::shared_ptr<EvalObject> evaluate() override;

private:
    int value_ = 0;
};

class PrefixExpression : public Expression
{
public:
    PrefixExpression(const std::string &op, std::unique_ptr<Expression> right);
    const std::string &op() const { return operator_; }
    const Expression *right() const { return right_.get(); }
    std::string toString() const override;
    std::shared_ptr<EvalObject> evaluate() override;

private:
    std::string operator_;
    std::unique_ptr<Expression> right_ = nullptr;
};

class InfixExpression : public Expression
{
public:
    InfixExpression(std::unique_ptr<Expression> lhs, const std::string &op, std::unique_ptr<Expression> rhs);
    const Expression *left() const { return lhs_.get(); }
    const Expression *right() const { return rhs_.get(); }
    const std::string op() const { return operator_; }
    std::string toString() const override;
    std::shared_ptr<EvalObject> evaluate() override;

private:
    std::unique_ptr<Expression> lhs_;
    std::string operator_;
    std::unique_ptr<Expression> rhs_;
};

class Boolean : public Expression
{
public:
    explicit Boolean(bool value);
    bool value() const { return value_; }
    std::string toString() const override;
    std::shared_ptr<EvalObject> evaluate() override;

private:
    bool value_;
};

class BlockStatement : public Statement
{
public:
    explicit BlockStatement(std::vector<std::unique_ptr<Statement>> statements);
    std::string toString() const;
    size_t size() const { return statements_.size(); }
    const Statement *at(size_t i) const { return statements_.at(i).get(); }
    std::shared_ptr<EvalObject> evaluate() override;

private:
    std::vector<std::unique_ptr<Statement>> statements_;
};

class IfExpression : public Expression
{
public:
    IfExpression(std::unique_ptr<Expression> condition,
                 std::unique_ptr<BlockStatement> consequence,
                 std::unique_ptr<BlockStatement> alternative);
    std::string toString() const override;
    const Expression *condition() const { return condition_.get(); }
    const BlockStatement *consequence() const { return consequence_.get(); }
    const BlockStatement *alternative() const { return alternative_.get(); }
    std::shared_ptr<EvalObject> evaluate() override;

private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<BlockStatement> consequence_;
    std::unique_ptr<BlockStatement> alternative_;
};

class FunctionLiteral : public Expression
{
public:
    FunctionLiteral(std::unique_ptr<BlockStatement> body, std::vector<std::unique_ptr<Identifier>> parameters);
    std::string toString() const override;
    size_t paramsSize() const { return parameters_.size(); }
    const Identifier *param(size_t i) const { return parameters_.at(i).get(); }
    size_t size() const { return body_->size(); }
    const BlockStatement *body() const { return body_.get(); }

private:
    std::unique_ptr<BlockStatement> body_;
    std::vector<std::unique_ptr<Identifier>> parameters_;
};

class CallExpression : public Expression
{
public:
    CallExpression(std::unique_ptr<Expression> function, std::vector<std::unique_ptr<Expression>> args);
    std::string toString() const override;
    const Expression *function() const { return function_.get(); }
    size_t argsSize() const { return arguments_.size(); }
    const Expression *argument(size_t i) const { return arguments_.at(i).get(); }

private:
    std::unique_ptr<Expression> function_;
    std::vector<std::unique_ptr<Expression>> arguments_;
};

#endif