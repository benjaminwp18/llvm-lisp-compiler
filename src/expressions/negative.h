#pragma once

#include "../expression.h"

class ASTExpressionNegation : public ASTExpression
{
    std::unique_ptr<ASTExpression> a1;
    VarTypeSimple* returnType = nullptr;

public:

    ASTExpressionNegation(
        std::unique_ptr<ASTExpression> a1
    ) : a1(std::move(a1)) {}

    static auto Create(std::unique_ptr<ASTExpression> a1)
    {
        return std::make_unique<ASTExpressionNegation>(std::move(a1));
    }

    std::unique_ptr<VarType> ReturnType(ASTFunction& func) override;
    bool IsLValue(ASTFunction& func) override;
    llvm::Value* Compile(llvm::IRBuilder<>& builder, ASTFunction& func) override;
    std::string ToString(const std::string& prefix) override;
};