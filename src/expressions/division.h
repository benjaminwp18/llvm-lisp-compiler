#pragma once

#include "../expression.h"

class ASTExpressionDivision : public ASTExpression
{
    std::unique_ptr<ASTExpression> a1;
    std::unique_ptr<ASTExpression> a2;
    VarTypeSimple* returnType = nullptr;

public:

    ASTExpressionDivision(
        std::unique_ptr<ASTExpression> a1,
        std::unique_ptr<ASTExpression> a2
    ) : a1(std::move(a1)), a2(std::move(a2)) {}

    static auto Create(std::unique_ptr<ASTExpression> a1, std::unique_ptr<ASTExpression> a2)
    {
        return std::make_unique<ASTExpressionDivision>(std::move(a1), std::move(a2));
    }

    std::unique_ptr<VarType> ReturnType(ASTFunction& func) override;
    bool IsLValue(ASTFunction& func) override;
    llvm::Value* Compile(llvm::IRBuilder<>& builder, ASTFunction& func) override;
    std::string ToString(const std::string& prefix) override;
};