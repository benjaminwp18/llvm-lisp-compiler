#include "multiplication.h"

std::unique_ptr<VarType> ASTExpressionMultiplication::ReturnType(ASTFunction& func)
{
    if (!returnType && !ASTExpression::CoerceMathTypes(func, a1, a2, returnType))
    {
        throw std::runtime_error("ERROR: Can not coerce types in multiplication expression! Are they both either ints or floats?");
    }
    return std::make_unique<VarTypeSimple>(*returnType);
}

bool ASTExpressionMultiplication::IsLValue(ASTFunction& func)
{
    return false;
}

llvm::Value* ASTExpressionMultiplication::Compile(llvm::IRBuilder<>& builder, ASTFunction& func)
{
    auto retType = ReturnType(func);
    if (retType->Equals(&VarTypeSimple::IntType)) {
        return builder.CreateMul(
            a1->CompileRValue(builder, func),
            a2->CompileRValue(builder, func)
        );
    }
    else if (retType->Equals(&VarTypeSimple::FloatType)) {
        return builder.CreateFMul(
            a1->CompileRValue(builder, func),
            a2->CompileRValue(builder, func)
        );
    }
    else {
        throw std::runtime_error("ERROR: Can not perform multiplication! Are both inputs either ints or floats?");
    }
}

std::string ASTExpressionMultiplication::ToString(const std::string& prefix)
{
    std::string ret = "(*)\n";
    ret += prefix + "├──" + a1->ToString(prefix + "│  ");
    ret += prefix + "└──" + a2->ToString(prefix + "   ");
    return ret;
}