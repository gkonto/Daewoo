#ifndef ASTNODE_HPP_INCLUDED
#define ASTNODE_HPP_INCLUDED

#include <vector>
#include <memory>
#include "ASTNodeTypes.hpp"

class ASTNode
{
public:
    ASTNode(ASTNodeType type, int lineNumber)
        : nodeType_(type), lineNumber_(lineNumber) {}
    virtual ~ASTNode() = default;
    ASTNodeType type() const { return nodeType_; }
    int lineNumber() const { return lineNumber_; }

protected:
    ASTNodeType nodeType_;
    int lineNumber_ = 0;
};

class ASTNodeList : public ASTNode
{
public:
    ASTNodeList(ASTNodeType type)
        : ASTNode(type, 0) {}
    void add(std::unique_ptr<ASTNode> n) { list_.emplace_back(std::move(n)); }
    size_t size() const { return list_.size(); }
    auto begin() { return list_.begin(); }
    auto end() { return list_.end(); }
    auto cbegin() const { return list_.cbegin(); }
    auto cend() const { return list_.cend(); }
    const ASTNode *at(size_t index) const { return list_.at(index).get(); }

private:
    std::vector<std::unique_ptr<ASTNode>> list_;
};

class ASTProgram : public ASTNode
{
public:
    ASTProgram(std::unique_ptr<ASTNodeList> statements)
        : ASTNode(ASTNodeType::ntProgram, 0),
          statements_(std::move(statements)) {}

    size_t size() const { return statements_->size(); }
    auto begin() { return statements_->begin(); }
    auto end() { return statements_->end(); }
    auto begin() const { return statements_->cbegin(); }
    auto end() const { return statements_->cend(); }
    auto at(size_t index) const { return statements_->at(index); }

private:
    std::unique_ptr<ASTNodeList> statements_;
};

class ASTErrorNode : public ASTNode
{
public:
    ASTErrorNode(const std::string &msg, int lineNumber, int columnNumber)
        : ASTNode(ASTNodeType::ntError, lineNumber), errorMsg_(msg), columnNumber_(columnNumber) {}

private:
    std::string errorMsg_;
    int columnNumber_ = 0;
};

class ASTPrimary : public ASTNode
{
public:
    ASTPrimary(std::unique_ptr<ASTNode> factor, std::unique_ptr<ASTNode> primaryPlus, int linenumber)
        : ASTNode(ASTNodeType::ntPrimary, linenumber), factor_(std::move(factor)), primaryPlus_(std::move(primaryPlus)) {}

private:
    std::unique_ptr<ASTNode> factor_;
    std::unique_ptr<ASTNode> primaryPlus_;
};

class ASTExpression : public ASTNode
{
public:
    ASTExpression(std::unique_ptr<ASTNode> expr, int lineNumber)
        : ASTNode(ASTNodeType::ntExpression, lineNumber), expression_(std::move(expr)) {}

private:
    std::unique_ptr<ASTNode> expression_;
};

class ASTIdentifier : public ASTNode
{
public:
    ASTIdentifier(const std::string &symbol, int linenumber)
        : ASTNode(ASTNodeType::ntIdentifier, linenumber), symbolName_(symbol) {}

    const std::string &value() const { return symbolName_; }

private:
    std::string symbolName_;
};

class ASTLetStatement : public ASTNode
{
public:
    ASTLetStatement(std::unique_ptr<ASTIdentifier> lhs,
                    std::unique_ptr<ASTNode> rhs,
                    int lineNumber)
        : ASTNode(ASTNodeType::ntAssignment, lineNumber),
          leftSide_(std::move(lhs)),
          rightSide_(std::move(rhs)) {}

    const std::string &identifierValue() const { return leftSide_->value(); }
    const ASTNode *expression() const { return rightSide_.get(); }

private:
    std::unique_ptr<ASTIdentifier> leftSide_;
    std::unique_ptr<ASTNode> rightSide_;
};

class ASTAssignment : public ASTNode
{
public:
    ASTAssignment(std::unique_ptr<ASTPrimary> lhs, std::unique_ptr<ASTNode> rhs, int lineNumber)
        : ASTNode(ASTNodeType::ntAssignment, lineNumber), leftSide_(std::move(lhs)), rightSide_(std::move(rhs)) {}

private:
    std::unique_ptr<ASTPrimary> leftSide_;
    std::unique_ptr<ASTNode> rightSide_;
};

class ASTExpressionStatement : public ASTNode
{
public:
    ASTExpressionStatement(std::unique_ptr<ASTNode> expr, int lineNumber)
        : ASTNode(ASTNodeType::ntExpressionStatement, lineNumber), expression_(std::move(expr)) {}

private:
    std::unique_ptr<ASTNode> expression_;
};

class ASTIf : public ASTNode
{
public:
    ASTIf(std::unique_ptr<ASTNode> condition, std::unique_ptr<ASTNode> thenStmt, std::unique_ptr<ASTNode> elseStmt, int lineNumber)
        : ASTNode(ASTNodeType::ntIf, lineNumber),
          condition_(std::move(condition)),
          thenStatementList_(std::move(thenStmt)),
          elseStatementList_(std::move(elseStmt))
    {
    }

private:
    std::unique_ptr<ASTNode> condition_;
    std::unique_ptr<ASTNode> thenStatementList_;
    std::unique_ptr<ASTNode> elseStatementList_;
};

class ASTReturn : public ASTNode
{
public:
    ASTReturn(std::unique_ptr<ASTExpression> expr, int linenumber)
        : ASTNode(ASTNodeType::ntReturn, linenumber), expression_(std::move(expr)) {}

private:
    std::unique_ptr<ASTExpression> expression_;
};

class ASTBinOp : public ASTNode
{
public:
    ASTBinOp(std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs, ASTNodeType nodeType, int linenumber)
        : ASTNode(nodeType, linenumber), left_(std::move(lhs)), right_(std::move(rhs)) {}

private:
    std::unique_ptr<ASTNode> left_;
    std::unique_ptr<ASTNode> right_;
};

class ASTPowerOp : public ASTNode
{
public:
    ASTPowerOp(std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs, int linenumber)
        : ASTNode(ASTNodeType::ntPower, linenumber), left_(std::move(lhs)), right_(std::move(rhs)) {}

private:
    std::unique_ptr<ASTNode> left_;
    std::unique_ptr<ASTNode> right_;
};

class ASTUniOp : public ASTNode
{
public:
    ASTUniOp(std::unique_ptr<ASTNode> left, ASTNodeType type, int linenumber)
        : ASTNode(type, linenumber), left_(std::move(left)) {}

private:
    std::unique_ptr<ASTNode> left_;
};

class ASTPrimaryOp : public ASTNode
{
public:
private:
};

class ASTInteger : public ASTNode
{
public:
    ASTInteger(int value, int lineNumber)
        : ASTNode(ASTNodeType::ntInteger, lineNumber), value_(value) {}
    int value() const { return value_; }

private:
    int value_ = 0;
};

class ASTFloat : public ASTNode
{
public:
    ASTFloat(double value, int lineNumber)
        : ASTNode(ASTNodeType::ntFloat, lineNumber), value_(value) {}
    double value() const { return value_; }

private:
    double value_ = 0.;
};

class ASTString : public ASTNode
{
public:
    ASTString(const std::string &value, int lineNumber)
        : ASTNode(ASTNodeType::ntString, lineNumber), value_(value) {}
    const std::string &value() const { return value_; }

private:
    std::string value_;
};

class ASTNotOp : public ASTNode
{
public:
    ASTNotOp(std::unique_ptr<ASTNode> node, int linenumber)
        : ASTNode(ASTNodeType::ntNOT, linenumber), expression_(std::move(node)) {}

private:
    std::unique_ptr<ASTNode> expression_;
};

class ASTBoolean : public ASTNode
{
public:
    ASTBoolean(bool value, int lineNumber)
        : ASTNode(ASTNodeType::ntBoolean, lineNumber), value_(value) {}
    bool value() const { return value_; }

private:
    bool value_ = true;
};

class ASTCreateList : public ASTNodeList
{
public:
private:
};

class ASTPrimaryFunction : public ASTNode
{
public:
    ASTPrimaryFunction(std::unique_ptr<ASTNodeList> argList,
                       std::unique_ptr<ASTNode> primaryPlus,
                       int linenumber)
        : ASTNode(ASTNodeType::ntPrimaryFunction, linenumber),
          argumentList_(std::move(argList)),
          primaryPlus_(std::move(primaryPlus)) {}

private:
    std::unique_ptr<ASTNodeList> argumentList_;
    std::unique_ptr<ASTNode> primaryPlus_;
};

class ASTNull : public ASTNode
{
public:
    ASTNull()
        : ASTNode(ASTNodeType::ntNull, 0) {}

private:
};

class ASTUserFunction : public ASTNode
{
public:
    ASTUserFunction(const std::string &moduleName, const std::string &functionName, std::unique_ptr<ASTNodeList> argList, std::unique_ptr<ASTNodeList> stmtList, int linenumber)
        : ASTNode(ASTNodeType::ntFunction, linenumber),
          moduleName_(moduleName), functionName_(functionName),
          argumentList_(std::move(argList)), statementList_(std::move(stmtList)) {}

private:
    std::string moduleName_;
    std::string functionName_;
    std::unique_ptr<ASTNodeList> argumentList_;
    std::unique_ptr<ASTNodeList> statementList_;
};

class ASTFunctionCall : public ASTNode
{
public:
    ASTFunctionCall(std::unique_ptr<ASTNodeList> argList, int linenumber)
        : ASTNode(ASTNodeType::ntFunctionCall, linenumber), argumentList_(std::move(argList)) {}

private:
    std::unique_ptr<ASTNodeList> argumentList_;
};

#endif
