#include "negative.h"

std::unique_ptr<VarType> ASTExpressionNegation::ReturnType(ASTFunction& func)
{
    if (!returnType && !ASTExpression::CoerceMathTypes(func, a1, a1, returnType))
    {
        throw std::runtime_error("ERROR: Can not coerce type in negation expression! Is the argument either an int or a float?");
    }
    return std::make_unique<VarTypeSimple>(*returnType);
}

bool ASTExpressionNegation::IsLValue(ASTFunction& func)
{
    return false;
}

llvm::Value* ASTExpressionNegation::Compile(llvm::IRBuilder<>& builder, ASTFunction& func)
{
    auto retType = ReturnType(func);
    if (retType->Equals(&VarTypeSimple::IntType)) {
        return builder.CreateNeg(
            a1->CompileRValue(builder, func)
        );
    }
    else if (retType->Equals(&VarTypeSimple::FloatType)) {
        return builder.CreateFNeg(
            a1->CompileRValue(builder, func)
        );
    }
    else {
        throw std::runtime_error("ERROR: Can not perform negation! Is the argument either an int or a float?");
    }
}

std::string ASTExpressionNegation::ToString(const std::string& prefix)
{
    std::string ret = "(-)\n";
    ret += prefix + "└──" + a1->ToString(prefix + "   ");
    return ret;
}