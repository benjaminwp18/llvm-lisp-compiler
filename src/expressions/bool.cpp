#include "bool.h"

std::unique_ptr<VarType> ASTExpressionBool::ReturnType(ASTFunction& func)
{
    return VarTypeSimple::BoolType.Copy();
}

bool ASTExpressionBool::IsLValue(ASTFunction& func)
{
    return false;
}

llvm::Value* ASTExpressionBool::Compile(llvm::IRBuilder<>& builder, ASTFunction& func)
{
    return llvm::ConstantInt::get(
        VarTypeSimple::BoolType.GetLLVMType(builder.getContext()),
        value
    );
}

std::string ASTExpressionBool::ToString(const std::string& prefix)
{
    return std::to_string(value) + "\n";
}