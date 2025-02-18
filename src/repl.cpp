
#include <iostream>
#include "repl.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "evaluator.hpp"

static std::string formatToken(const Token &t, const Lexer &l)
{
	std::string buffer;
	buffer += "{Type:" +
			  tokenType2String(t.type_) +
			  " Literal:" +
			  l.getLiteral(t) +
			  "}";
	return buffer;
}

static void printParserErrors(std::ostream &out, const Parser::Errors &errors)
{
	for (const std::string &error : errors)
	{
		std::cout << "\t" << error << std::endl;
	}
}

void Repl::start()
{
	std::string input;

	while (true)
	{
		std::cout << prompt;
		std::getline(std::cin, input);

		if (input.empty())
			break;

		Lexer l(input);
		Parser p(l);
		auto program = p.parseProgram();
		if (!p.errors().empty())
		{
			printParserErrors(std::cout, p.errors());
			continue;
		}

		auto evaluated = Eval(program.get());
		std::cout << inspect(*evaluated);
		std::cout << "\n";
	}
}