#pragma once

#include "../expression.h"

class ASTExpressionIf : public ASTExpression
{
public:

    std::unique_ptr<ASTExpression> condition;
    std::unique_ptr<ASTExpression> thenExpression;
    std::unique_ptr<ASTExpression> elseExpression;

    ASTExpressionIf(
        std::unique_ptr<ASTExpression> condition,
        std::unique_ptr<ASTExpression> thenExpression,
        std::unique_ptr<ASTExpression> elseExpression
    ) : condition(std::move(condition)), thenExpression(std::move(thenExpression)), elseExpression(std::move(elseExpression)) {}

    std::unique_ptr<VarType> ReturnType(ASTFunction& func) override;
    bool IsLValue(ASTFunction& func) override;
    llvm::Value* Compile(llvm::IRBuilder<>& builder, ASTFunction& func) override;
    std::string ToString(const std::string& prefix) override;

};