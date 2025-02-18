#include "ast.hpp"

#include <string>
#include <sstream>
#include <iterator>
#include <numeric>
#include <cstdarg>
#include <vector>

#include "globals.hpp"

namespace
{
	class Error
	{
	public:
		Error() = default;
		template <typename T>
		Error &operator<<(const T &value);
		std::string msg() const { return message_.str(); };

	private:
		std::ostringstream message_;
	};
}

template <typename T>
Error &Error::operator<<(const T &value)
{
	message_ << value;
	return *this;
}

size_t Program::size() const
{
	return statements_.size();
}

Program::Statements::const_iterator Program::begin() const
{
	return statements_.begin();
}

Program::Statements::const_iterator Program::end() const
{
	return statements_.end();
}

const std::string &LetStatement::name() const
{
	return name_->value();
}

void Program::add(std::unique_ptr<Statement> &&s)
{
	statements_.emplace_back(std::move(s));
}

Identifier::Identifier(const std::string &value)
	: value_(value)
{
}

LetStatement::LetStatement(const std::string &name, std::unique_ptr<Expression> value)
	: name_(std::make_unique<Identifier>(name)), value_(std::move(value))
{
}

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> &&exp)
	: expression_(std::move(exp))
{
}

IntegerLiteral::IntegerLiteral(int v)
	: value_(v)
{
}

ReturnStatement::ReturnStatement(std::unique_ptr<Expression> ret_value)
	: return_value_(std::move(ret_value))
{
}

PrefixExpression::PrefixExpression(const std::string &op, std::unique_ptr<Expression> right)
	: operator_(op), right_(std::move(right))
{
}

InfixExpression::InfixExpression(std::unique_ptr<Expression> lhs, const std::string &op, std::unique_ptr<Expression> rhs)
	: lhs_(std::move(lhs)), operator_(op), rhs_(std::move(rhs))
{
}

Boolean::Boolean(bool v)
	: value_(v)
{
}

std::string Program::toString() const
{
	std::stringstream out;
	for (const auto &s : statements_)
	{
		out << s->toString();
	}
	return out.str();
}

std::string LetStatement::toString() const
{
	std::stringstream out;
	out << name_->toString();
	out << " = ";
	if (value_)
	{
		out << value_->toString();
	}
	return out.str();
}

std::string Identifier::toString() const
{
	return value_;
}

std::string ReturnStatement::toString() const
{
	std::stringstream out;
	if (return_value_)
	{
		out << return_value_->toString();
	}
	out << ";";
	return out.str();
}

std::string IntegerLiteral::toString() const
{
	return std::to_string(value_);
}

std::string PrefixExpression::toString() const
{
	std::stringstream out;
	out << "(";
	out << operator_;
	out << right_->toString();
	out << ")";
	return out.str();
}

std::string InfixExpression::toString() const
{
	std::stringstream out;
	out << "(";
	out << lhs_->toString();
	out << " " << operator_ << " ";
	out << rhs_->toString();
	out << ")";
	return out.str();
}

std::string Boolean::toString() const
{
	return value_ ? "true" : "false";
}

std::string BlockStatement::toString() const
{
	std::stringstream out;
	for (const auto &stmt : statements_)
	{
		out << stmt->toString();
	}
	return out.str();
}

std::string IfExpression::toString() const
{
	std::stringstream out;
	out << "if";
	out << condition_->toString();
	out << " ";
	out << consequence_->toString();
	if (alternative_)
	{
		out << "else";
		out << alternative_->toString();
	}
	return out.str();
}

IfExpression::IfExpression(std::unique_ptr<Expression> condition,
						   std::unique_ptr<BlockStatement> consequence,
						   std::unique_ptr<BlockStatement> alternative)
	: condition_(std::move(condition)), consequence_(std::move(consequence)),
	  alternative_(std::move(alternative))
{
}

BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>> statements)
	: statements_(std::move(statements))
{
}

FunctionLiteral::FunctionLiteral(std::unique_ptr<BlockStatement> body, std::vector<std::unique_ptr<Identifier>> parameters)
	: body_(std::move(body)), parameters_(std::move(parameters))
{
}

CallExpression::CallExpression(std::unique_ptr<Expression> function, std::vector<std::unique_ptr<Expression>> args)
	: function_(std::move(function)), arguments_(std::move(args))
{
}

std::string FunctionLiteral::toString() const
{
	std::vector<std::string> params;
	for (const auto &p : parameters_)
	{
		params.emplace_back(p->toString());
	}
	std::string joined = std::accumulate(
		std::next(params.begin()), params.end(), params[0],
		[](const std::string &a, const std::string &b)
		{
			return a + ", " + b;
		});

	std::stringstream out;
	out << "(";
	out << joined;
	out << ") ";
	out << body_->toString();

	return out.str();
}

std::string CallExpression::toString() const
{
	std::stringstream out;
	std::vector<std::string> args;
	for (const auto &a : arguments_)
	{
		args.emplace_back(a->toString());
	}

	std::string joined = std::accumulate(
		std::next(args.begin()), args.end(), args[0],
		[](const std::string &a, const std::string &b)
		{
			return a + ", " + b;
		});

	out << function_->toString();
	out << "(";
	out << joined;
	out << ")";
	return out.str();
}

static std::shared_ptr<EvalObject> evalBangOperatorExpression(std::shared_ptr<EvalObject> right)
{
	auto &g = Globals::getInstance();
	if (right == g.getTrue())
	{
		return g.getFalse();
	}
	else if (right == g.getFalse())
	{
		return g.getTrue();
	}
	else
	{
		return g.getFalse();
	}
}

static std::shared_ptr<EvalObject> evalMinusPrefixOperatorExpression(std::shared_ptr<EvalObject> right)
{
	if (right->type != ObjType::Integer)
	{
		Error error;
		error << "unknown operator: -" << typeStr(right->type);
		auto e = std::make_unique<EvalObject>();
		e->type = ObjType::Error;
		e->value = error.msg();
		return e;
	}

	auto o = std::make_shared<EvalObject>();
	o->type = ObjType::Integer;
	o->value = -right->getInt();
	return o;
}

static std::shared_ptr<EvalObject> evalPrefixExpression(const std::string op, std::shared_ptr<EvalObject> right)
{
	if (!op.compare("!"))
	{
		return evalBangOperatorExpression(right);
	}
	else if (!op.compare("-"))
	{
		return evalMinusPrefixOperatorExpression(right);
	}
	else
	{
		Error err;
		err << "unknown operator: " << op << typeStr(right->type);
		auto e = std::make_unique<EvalObject>();
		e->type = ObjType::Error;
		e->value = err.msg();
		return e;
	}
}

// static std::shared_ptr<EvalObject> evalStatements(const std::vector<std::unique_ptr<Statement>> &statements)
// {
// 	std::shared_ptr<EvalObject> result = nullptr;
// 	for (const auto &stmt : statements)
// 	{
// 		result = stmt->evaluate();
// 		if (result->type == ObjType::Return)
// 		{
// 			return result->getObject();
// 		}
// 	}
// 	return result;
// }

static std::shared_ptr<EvalObject> nativeBoolToBooleanObject(bool value)
{
	auto g = Globals::getInstance();
	return value ? g.getTrue() : g.getFalse();
}

static std::shared_ptr<EvalObject> evalIntegerInfixExpression(
	const std::string &op, std::shared_ptr<EvalObject> l, std::shared_ptr<EvalObject> r)
{
	if (!op.compare("+"))
	{
		auto o = std::make_shared<EvalObject>();
		o->type = ObjType::Integer;
		o->value = l->getInt() + r->getInt();
		return o;
	}
	else if (!op.compare("-"))
	{
		auto o = std::make_shared<EvalObject>();
		o->type = ObjType::Integer;
		o->value = l->getInt() - r->getInt();
		return o;
	}
	else if (!op.compare("*"))
	{
		auto o = std::make_shared<EvalObject>();
		o->type = ObjType::Integer;
		o->value = l->getInt() * r->getInt();
		return o;
	}
	else if (!op.compare("/"))
	{
		auto o = std::make_shared<EvalObject>();
		o->type = ObjType::Integer;
		o->value = l->getInt() / r->getInt();
		return o;
	}
	else if (!op.compare("<"))
	{
		return nativeBoolToBooleanObject(l->getInt() < r->getInt());
	}
	else if (!op.compare(">"))
	{
		return nativeBoolToBooleanObject(l->getInt() > r->getInt());
	}
	else if (!op.compare("=="))
	{
		return nativeBoolToBooleanObject(l->getInt() == r->getInt());
	}
	else if (!op.compare("!="))
	{
		return nativeBoolToBooleanObject(l->getInt() != r->getInt());
	}
	else
	{
		Error error;
		error << "unknown operator: " << typeStr(l->type) << " " << op << " " << typeStr(r->type);
		auto e = std::make_unique<EvalObject>();
		e->type = ObjType::Error;
		e->value = error.msg();
		return e;
	}
}

static std::shared_ptr<EvalObject> evalInfixExpression(const std::string &op, std::shared_ptr<EvalObject> l, std::shared_ptr<EvalObject> r)
{
	if (l->type == ObjType::Integer && r->type == ObjType::Integer)
	{
		return evalIntegerInfixExpression(op, l, r);
	}
	else if (!op.compare("=="))
	{
		return nativeBoolToBooleanObject(l == r);
	}
	else if (!op.compare("!="))
	{
		return nativeBoolToBooleanObject(l != r);
	}
	else if (l->type != r->type)
	{
		Error error;
		error << "type mismatch: " << typeStr(l->type) << " " << op << " " << typeStr(r->type);
		auto e = std::make_unique<EvalObject>();
		e->type = ObjType::Error;
		e->value = error.msg();
		return e;
	}
	else
	{
		Error error;
		error << "unknown operator: " << typeStr(l->type) << " " << op << " " << typeStr(r->type);
		auto e = std::make_unique<EvalObject>();
		e->type = ObjType::Error;
		e->value = error.msg();
		return e;
	}
}

std::shared_ptr<EvalObject> IntegerLiteral::evaluate()
{
	auto o = std::make_unique<EvalObject>();
	o->type = ObjType::Integer;
	o->value = value_;
	return o;
}

std::shared_ptr<EvalObject> Identifier::evaluate()
{
}

std::shared_ptr<EvalObject> Program::evaluate()
{
	std::shared_ptr<EvalObject> result = nullptr;
	for (const auto &stmt : statements_)
	{
		result = stmt->evaluate();
		if (result->type == ObjType::Return)
		{
			return result->getObject();
		}
		else if (result->type == ObjType::Error)
		{
			return result;
		}
	}
	return result;
}

std::shared_ptr<EvalObject> ExpressionStatement::evaluate()
{
	return expression_->evaluate();
}

std::shared_ptr<EvalObject> Boolean::evaluate()
{
	return nativeBoolToBooleanObject(value_);
}

std::shared_ptr<EvalObject> PrefixExpression::evaluate()
{
	auto right = right_->evaluate();
	return evalPrefixExpression(operator_, right);
}

std::shared_ptr<EvalObject> InfixExpression::evaluate()
{
	auto left = lhs_->evaluate();
	auto right = rhs_->evaluate();
	return evalInfixExpression(operator_, left, right);
}

std::shared_ptr<EvalObject> BlockStatement::evaluate()
{
	std::shared_ptr<EvalObject> result = nullptr;
	for (const auto &stmt : statements_)
	{
		result = stmt->evaluate();
		if (result)
		{
			if (result->type == ObjType::Return || result->type == ObjType::Error)
			{
				return result;
			}
		}
	}
	return result;
}

static bool isTruthy(const EvalObject *o)
{
	const auto &g = Globals::getInstance();
	if (o == g.getNull().get())
	{
		return false;
	}
	else if (o == g.getTrue().get())
	{
		return true;
	}
	else if (o == g.getFalse().get())
	{
		return false;
	}
	else
	{
		return true;
	}
}

std::shared_ptr<EvalObject> IfExpression::evaluate()
{
	// evalIfExpression
	auto condition = condition_->evaluate();
	if (isTruthy(condition.get()))
	{
		return consequence_->evaluate();
	}
	else if (alternative_)
	{
		return alternative_->evaluate();
	}
	else
	{
		return Globals::getInstance().getNull();
	}
}

std::shared_ptr<EvalObject> ReturnStatement::evaluate()
{
	auto val = return_value_->evaluate();
	auto ret = std::make_unique<EvalObject>();
	ret->type = ObjType::Return;
	ret->value = val;
	return ret;
}