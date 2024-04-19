#pragma once

#include "../expression.h"
#include <vector>

class ASTExpressionSexpr : public ASTExpression
{

    std::vector<std::unique_ptr<ASTExpression>> terms;

public:
    ASTExpressionSexpr(std::vector<std::unique_ptr<ASTExpression>> terms) : terms(std::move(terms)) {}

    static auto Create(std::vector<std::unique_ptr<ASTExpression>> terms)
    {
        return std::make_unique<ASTExpressionSexpr>(std::move(terms));
    }

    std::unique_ptr<VarType> ReturnType(ASTFunction& func) override;
    bool IsLValue(ASTFunction& func) override;
    llvm::Value* Compile(llvm::IRBuilder<>& builder, ASTFunction& func) override;
    std::string ToString(const std::string& prefix) override;
};
