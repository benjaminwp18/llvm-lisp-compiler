#pragma once

#include "../expression.h"

// Sometimes we need more than a single statement, like a code block. This is the purpose of this.
class ASTExpressionSet : public ASTExpression
{
public:

    std::vector<std::unique_ptr<ASTExpression>> expressions;

    std::unique_ptr<VarType> ReturnType(ASTFunction& func) override;
    bool IsLValue(ASTFunction& func) override;
    llvm::Value* Compile(llvm::IRBuilder<>& builder, ASTFunction& func) override;
    std::string ToString(const std::string& prefix) override;

};