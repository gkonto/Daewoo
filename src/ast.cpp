#include "ast.hpp"

#include <string>
#include <sstream>
#include <iterator>
#include <numeric>
#include <cstdarg>
#include <vector>

#include "globals.hpp"
#include "environment.hpp"
#include "macros.hpp"

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

void Program::add(__Ptr<Statement> &&s)
{
	statements_.emplace_back(std::move(s));
}

Identifier::Identifier(const std::string &value)
	: value_(value)
{
}

LetStatement::LetStatement(const std::string &name, __Ptr<Expression> value)
	: name_(std::make_shared<Identifier>(name)), value_(std::move(value))
{
}

ExpressionStatement::ExpressionStatement(__Ptr<Expression> &&exp)
	: expression_(std::move(exp))
{
}

IntegerLiteral::IntegerLiteral(int v)
	: value_(v)
{
}

ReturnStatement::ReturnStatement(__Ptr<Expression> ret_value)
	: return_value_(std::move(ret_value))
{
}

PrefixExpression::PrefixExpression(const std::string &op, __Ptr<Expression> right)
	: operator_(op), right_(std::move(right))
{
}

InfixExpression::InfixExpression(__Ptr<Expression> lhs, const std::string &op, __Ptr<Expression> rhs)
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

IfExpression::IfExpression(__Ptr<Expression> condition,
						   __Ptr<BlockStatement> consequence,
						   __Ptr<BlockStatement> alternative)
	: condition_(std::move(condition)), consequence_(std::move(consequence)),
	  alternative_(std::move(alternative))
{
}

BlockStatement::BlockStatement(std::vector<__Ptr<Statement>> statements)
	: statements_(std::move(statements))
{
}

FunctionLiteral::FunctionLiteral(__Ptr<BlockStatement> body, std::vector<__Ptr<Identifier>> parameters)
	: body_(std::move(body)), parameters_(std::move(parameters))
{
}

CallExpression::CallExpression(__Ptr<Expression> function, std::vector<__Ptr<Expression>> args)
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

static __Ptr<EvalObject> evalBangOperatorExpression(__Ptr<EvalObject> right)
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

static __Ptr<EvalObject> evalMinusPrefixOperatorExpression(__Ptr<EvalObject> right)
{
	if (right->type != ObjType::Integer)
	{
		Error error;
		error << "unknown operator: -" << typeStr(right->type);
		auto e = std::make_shared<EvalObject>();
		e->type = ObjType::Error;
		e->value = error.msg();
		return e;
	}

	auto o = std::make_shared<EvalObject>();
	o->type = ObjType::Integer;
	o->value = -right->getInt();
	return o;
}

static __Ptr<EvalObject> evalPrefixExpression(const std::string op, __Ptr<EvalObject> right)
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
		auto e = std::make_shared<EvalObject>();
		e->type = ObjType::Error;
		e->value = err.msg();
		return e;
	}
}

// static __Ptr<EvalObject> evalStatements(const std::vector<__Ptr<Statement>> &statements)
// {
// 	__Ptr<EvalObject> result = nullptr;
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

static __Ptr<EvalObject> nativeBoolToBooleanObject(bool value)
{
	auto g = Globals::getInstance();
	return value ? g.getTrue() : g.getFalse();
}

static __Ptr<EvalObject> evalIntegerInfixExpression(
	const std::string &op, __Ptr<EvalObject> l, __Ptr<EvalObject> r)
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
		auto e = std::make_shared<EvalObject>();
		e->type = ObjType::Error;
		e->value = error.msg();
		return e;
	}
}

static __Ptr<EvalObject> evalInfixExpression(const std::string &op, __Ptr<EvalObject> l, __Ptr<EvalObject> r)
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
		auto e = std::make_shared<EvalObject>();
		e->type = ObjType::Error;
		e->value = error.msg();
		return e;
	}
	else
	{
		Error error;
		error << "unknown operator: " << typeStr(l->type) << " " << op << " " << typeStr(r->type);
		auto e = std::make_shared<EvalObject>();
		e->type = ObjType::Error;
		e->value = error.msg();
		return e;
	}
}

__Ptr<EvalObject> IntegerLiteral::evaluate(__Ptr<Environment> env)
{
	auto o = std::make_shared<EvalObject>();
	o->type = ObjType::Integer;
	o->value = value_;
	return o;
}

__Ptr<EvalObject> Program::evaluate(__Ptr<Environment> env)
{
	__Ptr<EvalObject> result = nullptr;
	for (const auto &stmt : statements_)
	{
		result = stmt->evaluate(env);
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

__Ptr<EvalObject> ExpressionStatement::evaluate(__Ptr<Environment> env)
{
	return expression_->evaluate(env);
}

__Ptr<EvalObject> Boolean::evaluate(__Ptr<Environment> env)
{
	return nativeBoolToBooleanObject(value_);
}

__Ptr<EvalObject> PrefixExpression::evaluate(__Ptr<Environment> env)
{
	auto right = right_->evaluate(env);
	return evalPrefixExpression(operator_, right);
}

__Ptr<EvalObject> InfixExpression::evaluate(__Ptr<Environment> env)
{
	auto left = lhs_->evaluate(env);
	auto right = rhs_->evaluate(env);
	return evalInfixExpression(operator_, left, right);
}

__Ptr<EvalObject> BlockStatement::evaluate(__Ptr<Environment> env)
{
	__Ptr<EvalObject> result = nullptr;
	for (const auto &stmt : statements_)
	{
		result = stmt->evaluate(env);
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

__Ptr<EvalObject> LetStatement::evaluate(__Ptr<Environment> env)
{
	auto val = value_->evaluate(env);
	if (val->type == ObjType::Error)
	{
		return val;
	}
	env->set(name_->value(), val);
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

__Ptr<EvalObject> IfExpression::evaluate(__Ptr<Environment> env)
{
	// evalIfExpression
	auto condition = condition_->evaluate(env);
	if (isTruthy(condition.get()))
	{
		return consequence_->evaluate(env);
	}
	else if (alternative_)
	{
		return alternative_->evaluate(env);
	}
	else
	{
		return Globals::getInstance().getNull();
	}
}

__Ptr<EvalObject> ReturnStatement::evaluate(__Ptr<Environment> env)
{
	auto val = return_value_->evaluate(env);
	auto ret = std::make_shared<EvalObject>();
	ret->type = ObjType::Return;
	ret->value = val;
	return ret;
}

static __Ptr<EvalObject> evalIdentifier(Identifier &node, const __Ptr<Environment> env)
{
	auto val = env->get(node.value());
	if (!val)
	{
		Error error;
		error << "identifier not found: " << node.value();
		auto e = std::make_shared<EvalObject>();
		e->type = ObjType::Error;
		e->value = error.msg();
		return e;
	}
	return val;
}

__Ptr<EvalObject> Identifier::evaluate(__Ptr<Environment> env)
{
	return evalIdentifier(*this, env);
}

__Ptr<EvalObject> FunctionLiteral::evaluate(__Ptr<Environment> env)
{
	auto f = std::make_shared<EvalObject>();
	f->type = ObjType::Function;
	f->value = Function(parameters_, body_, env);
	return f;
}

static std::vector<__Ptr<EvalObject>> evalExpressions(std::vector<__Ptr<Expression>> exps, __Ptr<Environment> env)
{
	std::vector<__Ptr<EvalObject>> result;
	for (auto e : exps)
	{
		auto evaluated = e->evaluate(env);
		if (evaluated->type == ObjType::Error)
		{
			return std::vector<__Ptr<EvalObject>>{evaluated};
		}
		result.emplace_back(evaluated);
	}
	return result;
}

static __Ptr<Environment> extendFunctionEnv(Function fn,
											std::vector<__Ptr<EvalObject>> args)
{
	auto env = std::make_shared<Environment>(fn.environment());
	auto params = fn.parameters();
	for (size_t paramIdx = 0; paramIdx < params.size(); ++paramIdx)
	{
		auto param = params[paramIdx];
		env->set(param->value(), args[paramIdx]);
	}
	return env;
}

static __Ptr<EvalObject> unwrapReturnValue(__Ptr<EvalObject> obj)
{
	if (obj->type == ObjType::Return)
	{
		return obj->getObject();
	}
	return obj;
}

static __Ptr<EvalObject> applyFunction(__Ptr<EvalObject> fn, std::vector<__Ptr<EvalObject>> args)
{

	if (fn->type != ObjType::Function)
	{
		Error error;
		error << "not a function: " << typeStr(fn->type);
		auto e = std::make_shared<EvalObject>();
		e->type = ObjType::Error;
		e->value = error.msg();
		return e;
	}

	auto function = fn->getFunction();

	auto extendedEnv = extendFunctionEnv(function, args);
	auto evaluated = function.body()->evaluate(extendedEnv);
	return unwrapReturnValue(evaluated);
}

__Ptr<EvalObject> CallExpression::evaluate(__Ptr<Environment> env)
{
	auto f = function_->evaluate(env);
	if (f->type == ObjType::Error)
	{
		return f;
	}
	auto args = evalExpressions(arguments_, env);
	if (args.size() == 1 && args[0]->type == ObjType::Error)
	{
		return args[0];
	}
	return applyFunction(f, args);
}
