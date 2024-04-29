#include "ifExpr.h"

#include "../function.h"

std::unique_ptr<VarType> ASTExpressionIf::ReturnType(ASTFunction& func)
{
    auto thenRet = thenExpression->ReturnType(func);
    auto elseRet = elseExpression->ReturnType(func);

    // Check for matching return types.
    if (thenRet->Equals(elseRet.get())) {
        return std::move(thenRet);  // Return if equal.
    }
    else {
        throw std::runtime_error("ERROR: If/Else expressions both return a value but their return types don't match!");
    }
}

bool ASTExpressionIf::IsLValue(ASTFunction& func)
{
    return false;
}

llvm::Value* ASTExpressionIf::Compile(llvm::IRBuilder<>& builder, ASTFunction& func)
{
    if (!condition->ReturnType(func)->Equals(&VarTypeSimple::BoolType)) {
        throw std::runtime_error("ERROR: Expected condition that returns a boolean value but got another type instead!");
    }

    llvm::Value* cond = condition->Compile(builder, func);
    llvm::Value* thenVal = thenExpression->Compile(builder, func);
    llvm::Value* elseVal = elseExpression->Compile(builder, func);

    return builder.CreateSelect(cond, thenVal, elseVal);
}

std::string ASTExpressionIf::ToString(const std::string& prefix)
{
    std::string output = "if " + condition->ToString(prefix) + "\n";
    output += prefix + (thenExpression ? "├──" : "└──") + thenExpression->ToString(prefix + "   ");
    output += prefix + (elseExpression ? "├──" : "└──") + elseExpression->ToString(prefix + "   ");
    return output;
}