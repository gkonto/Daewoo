#include <assert.h>
#include "TByteCodeBuilder.hpp"
#include "TModule.hpp"
#include "lexer.hpp"

void TByteCodeBuilder::expect(TokenCode tcode) {
    if (code() != tcode) {
        // TODO this is an error but i do not want to raise exception
    } else {
        nextToken();
    }
}

// program = statementList
void TByteCodeBuilder::build(TModule *module) {
    sc_.reset();
    sc_.setMode(TokensTable::Mode::Reading);
    nextToken();

    module_ = module;
    statementList(module_->code());
    module_->code().addByteCode(OpCode::Halt);
}

// statementList = statement { statement }
void TByteCodeBuilder::statementList(TProgram &program) {
    statement(program);
    while (true) {
        if (code() == TokenCode::tSemicolon) {
            expect(TokenCode::tSemicolon);
        }

        if (code() == TokenCode::tEnd || code() == TokenCode::tElse ||
            code() == TokenCode::tEndofStream) {
            return;
        }
        statement(program);
    }
}

// statement = exprStatement | forStatement | ifStatement
//                           | whileStatement | repeatStatement
//                           | returnStatment | breakStatement
//                           | function
// exprStatement =   assignment
//                 | rightHandSide
// assignment = leftHandSide '=' expression
// rightHandside = expression
// leftHandSide = identifier ( '[' expression ']' )
void TByteCodeBuilder::statement(TProgram &program) {
    switch (code()) {
        // case TokenCode::tIf:
        //     return ifStatement();
        // case TokenCode::tReturn:
        //     return returnStmt();
        // case TokenCode::tFunction:
        //     return parseUserDefinedFunction();
        // case TokenCode::tLet:
        //     return letStatement();
        default:
            return exprStatement(program);
    }
}

// exprStatement = expression '=' expression
// There are however some restrictions on the left-hand expression

// Left-hand sides can only be:
// ident '='
// ident '[' expression ']' '='

// They cannot be expressions such as 1 + 5, a + b, etc
// A valid left-hand side will always, when parsed, appear to have load or
// lvecIdx as the last bytecode

// We first read the left-side into a local program code space, if there
// is no '=' then we exit the method, else we also parse the right-hand side.
// We now append the code for the left-hand side on to the right-hand code
// and patch the last byte code to be either store or svecIdx

// This approach will allow us to identify illegal left-hand sides
// qw well as permit indices for lists to be of any complexity
void TByteCodeBuilder::exprStatement(TProgram &program) {
    TProgram progFragment;
    expression(progFragment);
    if (code() == TokenCode::tAssign) {
        assert(false);
    } else {
        if (inUserFunctionParsing_) {
            progFragment.addByteCode(OpCode::Pop);
        }
        program.appendProgram(progFragment);
    }
}

void TByteCodeBuilder::expression(TProgram &program) {
    relationalOperators(program);
    while (code() == TokenCode::tOr || code() == TokenCode::tAnd) {
        TokenCode op = code();
        nextToken();
        relationalOperators(program);
        if (op == TokenCode::tOr) {
            program.addByteCode(OpCode::Or);
        } else if (op == TokenCode::tAnd) {
            program.addByteCode(OpCode::And);
        }
    }
}

// expression = simpleExpression | simpleExpression relationalOp
// simpleExpression
void TByteCodeBuilder::relationalOperators(TProgram &program) {
    simpleExpression(program);
    while (code() == TokenCode::tLessThan || code() == TokenCode::tLessThanOrEqual ||
           code() == TokenCode::tMoreThan || code() == TokenCode::tMoreThanOrEqual ||
           code() == TokenCode::tNotEqual || code() == TokenCode::tEquivalence) {
        auto op = code();
        nextToken();
        simpleExpression(program);
        if (op == TokenCode::tEquivalence) {
            program.addByteCode(OpCode::IsEq);
        } else if (op == TokenCode::tLessThan) {
            program.addByteCode(OpCode::IsLt);
        } else if (op == TokenCode::tMoreThan) {
            program.addByteCode(OpCode::IsGt);
        } else if (op == TokenCode::tMoreThanOrEqual) {
            program.addByteCode(OpCode::IsGte);
        } else if (op == TokenCode::tLessThanOrEqual) {
            program.addByteCode(OpCode::IsLte);
        } else if (op == TokenCode::tNotEqual) {
            program.addByteCode(OpCode::IsNotEq);
        }
    }
}

// expression = term { ('+' | '-' | MOD | DIV) power }
void TByteCodeBuilder::simpleExpression(TProgram &program) {
    term(program);
    while (code() == TokenCode::tPlus || code() == TokenCode::tMinus) {
        auto op = code();
        nextToken();
        term(program);
        if (op == TokenCode::tPlus) {
            program.addByteCode(OpCode::Add);
        } else if (op == TokenCode::tMinus) {
            program.addByteCode(OpCode::Sub);
        }
    }
}

// term = power { ('*', '/', MOD, DIV) power }
void TByteCodeBuilder::term(TProgram &program) {
    power(program);
    while (code() == TokenCode::tMult || code() == TokenCode::tDivide) {
        auto op = code();
        nextToken();
        power(program);
        if (op == TokenCode::tMult) {
            program.addByteCode(OpCode::Mult);
        } else if (op == TokenCode::tDivide) {
            program.addByteCode(OpCode::Divide);
        }
    }
}

// power = {'+' | '-'} factor [ '^' power ]
void TByteCodeBuilder::power(TProgram &program) {
    int unaryMinus_count = 0;
    while (code() == TokenCode::tMinus || code() == TokenCode::tPlus) {
        if (code() == TokenCode::tMinus) {
            ++unaryMinus_count;
        }
        nextToken();
    }
    factor(program);
    if (code() == TokenCode::tPower) {
        nextToken();
        power(program);
        program.addByteCode(OpCode::Power);
    }

    for (int i = 0; i < unaryMinus_count; ++i) {
        program.addByteCode(OpCode::Umi);
    }
}

// factor = integer | float | '(' expression ')' | etc
void TByteCodeBuilder::factor(TProgram &program) {
    if (code() == TokenCode::tInteger) {
        program.addByteCode(OpCode::Pushi, token().tInt());
        nextToken();
    } else if (code() == TokenCode::tFloat) {
        program.addByteCode(OpCode::Pushd, token().tFloat());
        nextToken();
    } else if (code() == TokenCode::tLeftParenthesis) {
        nextToken();
        expression(program);
        expect(TokenCode::tRightParenthesis);
    } else if (code() == TokenCode::tIdentifier) {
        parseIdentifier(program);
    } else if (code() == TokenCode::tString) {
        program.addByteCode(OpCode::Pushs, token().tString());
        nextToken();
    } else if (code() == TokenCode::tNot) {
        nextToken();
        expression(program);
        program.addByteCode(OpCode::Not);
    } else if (code() == TokenCode::tFalse) {
        program.addByteCode(OpCode::Pushb, false);
        nextToken();
    } else if (code() == TokenCode::tTrue) {
        program.addByteCode(OpCode::Pushb, true);
        nextToken();
    } else {
        throw std::runtime_error(
            "TByteCodeBuilder> expecting scalar, identifier or left parenthesis");
    }
}

// There are a number of places where we will find identifiers:
// 1. As a function call, eg func (a,b)
// 2. As an indexed variable, eg x[i]
// 3. An ordinary variable, eg x
void TByteCodeBuilder::parseIdentifier(TProgram &program) {
    // bool globalVariable = false;
    auto identifier = token().tString();
    nextToken();
    if (code() == TokenCode::tLeftParenthesis) {
        // It's the start of a function call, eg func (1,2)
        // Check that the function already exists in the main symbol table
        // We do a reverse search, look for most recent declared functions
        throw std::runtime_error("TByteCodeBuilder> Not yet implemented");
    }

    if (inUserFunctionScope()) {
        throw std::runtime_error("TByteCodeBuilder> Not yet implemented");
    } else {
        int index = 0;
        if (not module_->symboltable().find(identifier, index)) {
            index = module_->symboltable().addSymbol(identifier);
        }

        if (code() == TokenCode::tLeftBracket) {
            throw std::runtime_error("TByteCodeBuilder> Not yet implemented");
        } else {
            program.addByteCode(OpCode::Load, index);
        }
    }
}
