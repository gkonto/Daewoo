#include <assert.h>
#include <sstream>
#include "TByteCodeBuilder.hpp"
#include "TModule.hpp"
#include "lexer.hpp"

void TByteCodeBuilder::expect(TokenCode tcode) {
    if (code() != tcode) {
        std::stringstream ss;
        ss << "TByteCodeBuilder> expected variable: " << tokenToString(tcode)
           << " got: " << tokenToString(code());
        throw std::runtime_error(ss.str());
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
        case TokenCode::tIf:
            return ifStatement(program);
        case TokenCode::tReturn:
            return returnStmt(program);
        case TokenCode::tFunction:
            return functionDef(program);
        case TokenCode::tLet:
            return letStatement(program);
        case TokenCode::tEnd:
            return;
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
        nextToken();
        expression(program);
        auto &bytecode = progFragment.last();
        if (bytecode.opCode == OpCode::Load) {
            bytecode.opCode = OpCode::Store;
        } else {
            throw std::runtime_error("Left-hand side cannot be assigned to");
        }
        program.appendProgram(progFragment);
    } else {
        if (inUserFunctionParsing_) {
            progFragment.addByteCode(OpCode::Pop);
        }
        program.appendProgram(progFragment);
    }
}

// letStatement = "let" identifier '=' expression
void TByteCodeBuilder::letStatement(TProgram &program) {
    expect(TokenCode::tLet);

    auto identifier = token().tString();
    int index = -1;
    if (not module_->symboltable().find(identifier, index)) {
        index = module_->symboltable().addSymbol(identifier);
    } else {
        std::stringstream ss;
        ss << "TByteCodeBuilder> undefined variable: " << identifier;
        throw std::runtime_error(ss.str());
    }
    nextToken();
    expect(TokenCode::tAssign);
    expression(program);
    program.addByteCode(OpCode::Store, index);
}

// ifStatement = IF expression THEN statement ifEnd
// ifEnd = END | ELSE statementList END
void TByteCodeBuilder::ifStatement(TProgram &program) {
    expect(TokenCode::tIf);
    expression(program);
    int jmpLocation_1 = static_cast<int>(program.addByteCode(OpCode::JmpIfFalse));
    expect(TokenCode::tThen);
    statementList(program);
    if (code() == TokenCode::tElse) {
        int jmpLocation_2 = static_cast<int>(program.addByteCode(OpCode::Jmp));
        program.setGotoLabel(jmpLocation_1,
                             static_cast<int>(program.getCurrentInstructionPointer()) -
                                 jmpLocation_1);
        nextToken();
        statementList(program);
        program.setGotoLabel(jmpLocation_2,
                             static_cast<int>(program.getCurrentInstructionPointer()) -
                                 jmpLocation_2);
        expect(TokenCode::tEnd);
    } else {
        expect(TokenCode::tEnd);
        program.setGotoLabel(jmpLocation_1,
                             static_cast<int>(program.getCurrentInstructionPointer()) -
                                 jmpLocation_1);
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

// argumentList = expression { ',' expression }
// Returns the number of expressions that were parsed
int TByteCodeBuilder::expressionList(TProgram &program) {
    int nArguments = 0;
    expression(program);
    ++nArguments;
    while (code() == TokenCode::tComma) {
        nextToken();
        expression(program);
        ++nArguments;
    }
    return nArguments;
}

void TByteCodeBuilder::parseFunctionCall(TProgram &program, int expectedArguments) {
    int nArguments = 0;
    auto identifier = token().tString();
    nextToken();
    if (code() != TokenCode::tRightParenthesis) {
        nArguments = expressionList(program);
    }
    if (nArguments != expectedArguments) {
        throw std::runtime_error(
            "incorrect number of arguments in function call: [' + identifier + ']");
    }
    expect(TokenCode::tRightParenthesis);
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
        int index = 0;
        if (symboltable().find(identifier, index)) {
            if (symboltable().get(index).type() == TSymbolElementType::symUserFunc) {
                parseFunctionCall(program, symboltable().get(index).fvalue()->numberOfArguments());
                program.addByteCode(OpCode::Pushi, index);
                program.addByteCode(OpCode::Call);
                return;
            } else {
                throw std::runtime_error(
                    "A name was found in front of '(', but the name is not a user function name: "
                    "[" +
                    identifier + "]");
            }
        } else {
            throw std::runtime_error(
                "TByteCodeBuilder>Builtin function reading not yet implemented");
        }
    }

    // If it's not a function then it must be an ordinary variable.
    // If we're in a user function then it's a local variable or possibly a
    // global variable. If we're not in a user function it's a
    // module variable level. If we're in a user function then is the local variable
    // already declared? If its not, add the symbol to the user function symbol table
    // We also issue either a oLoad or oLoadLocal depending on the scope
    if (inUserFunctionScope()) {
        throw std::runtime_error("TByteCodeBuilder> Not yet implemented");
    } else {
        int index = 0;
        if (not module_->symboltable().find(identifier, index)) {

            std::stringstream ss;
            ss << "TByteCodeBuilder> undefined variable: " << identifier;
            throw std::runtime_error(ss.str());
            // index = module_->symboltable().addSymbol(identifier);
        }

        if (code() == TokenCode::tLeftBracket) {
            throw std::runtime_error("TByteCodeBuilder> Not yet implemented");
        } else {
            program.addByteCode(OpCode::Load, index);
        }
    }
}

// argument = identifier
void TByteCodeBuilder::argument(TProgram &program) {
    if (code() != TokenCode::tIdentifier) {
        throw std::runtime_error(
            "TByteCodeBuilder> expecting identifier in function argument definition");
    }

    // Add the argument symbol to the user function local symbol table
    int index = 0;
    if (!currentUserFunction->symboltable().find(token().tString(), index)) {
        currentUserFunction->symboltable().addSymbol(token().tString());
    }
    nextToken();
}

// argumentList = argument { ',' argument }
int TByteCodeBuilder::argumentList(TProgram &program) {
    int count = 0;
    if (code() == TokenCode::tIdentifier) {
        count = 1;
        argument(program);
    }

    while (code() == TokenCode::tComma) {
        nextToken();
        argument(program);
        ++count;
    }
    return count;
}

// function = function identifier '(' argumentList ')' statementList
void TByteCodeBuilder::functionDef(TProgram &program) {
    nextToken();
    bool newUserFunction = false;
    std::string functionName;
    if (code() != TokenCode::tIdentifier) {
        throw std::runtime_error("TByteCodeBuilder> expecting function name");
    } else {
        functionName = sc_.token().tString();
    }
    currentUserFunction = new TUserFunction(functionName);
    module_->symboltable().addSymbol(currentUserFunction);

    enterUserFunctionScope();
    nextToken();
    expect(TokenCode::tLeftParenthesis);
    currentUserFunction->setNumberOfArguments(argumentList(currentUserFunction->funcCode()));
    expect(TokenCode::tRightParenthesis);

    statementList(currentUserFunction->funcCode());
    exitUserFunctionScope();
    inUserFunctionParsing_ = false;
    expect(TokenCode::tEnd);
    // make sure we return
    currentUserFunction->funcCode().addByteCode(OpCode::PushNone);
    currentUserFunction->funcCode().addByteCode(OpCode::Return);
    exitUserFunctionScope();
}

// returnStatement = RETURN expression
void TByteCodeBuilder::returnStmt(TProgram &program) {
    if (!inUserFunctionParsing_) {
        throw std::runtime_error("TByteCodeBuilder> return statement outside function");
    }
    expect(TokenCode::tReturn);
    expression(program);
    program.addByteCode(OpCode::Return);
}
