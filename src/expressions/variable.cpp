#include "variable.h"

#include "../function.h"

std::unique_ptr<VarType> ASTExpressionVariable::ReturnType(ASTFunction& func)
{
    printf("COMPILING: CHECKING VARIABLE %s RETURN TYPE\n", var.c_str());
    fflush(stdout);
    return func.GetVariableType(var)->Copy(); // We just need to resolve the variable and copy its type.
}

bool ASTExpressionVariable::IsLValue(ASTFunction& func)
{
    auto retType = ReturnType(func);
    return !dynamic_cast<VarTypeFunction*>(retType.get());
    // If the variable is a function type, then we shouldn't load from it, it's just a raw function address.
    // Otherwise, we know that the variable is really just a pointer to some memory allocated somewhere and is thus an L-Value.
}

llvm::Value* ASTExpressionVariable::Compile(llvm::IRBuilder<>& builder, ASTFunction& func)
{
    printf("COMPILING: COMPILING VARIABLE %s FOR VALUE\n", var.c_str());
    fflush(stdout);
    return func.GetVariableValue(var); // Simply just return the value from the scope table.
}

std::string ASTExpressionVariable::ToString(const std::string& prefix)
{
    return var + "\n";
}

std::string ASTExpressionVariable::getVariableString()
{
    return var;
}