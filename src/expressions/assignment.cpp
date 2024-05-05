#include "assignment.h"
#include "../function.h"
#include "variable.h"

std::unique_ptr<VarType> ASTExpressionAssignment::ReturnType(ASTFunction& func)
{
    printf("COMPILING ASSIGNMENT CHECKING RETURN TYPE\n");
    fflush(stdout);
    try {
        return left->ReturnType(func);
    }
    catch (std::runtime_error& e)
    {
        printf("COMPILING ASSIGNMENT CHECKING RETURN TYPE: FAILED TO GET LEFT RETURN TYPE, ADOPTING RIGHT\n");
        fflush(stdout);
    }

    return right->ReturnType(func); // "x = 5" simply just returns an L-Value of x so we can do "x = y = 5".
}

bool ASTExpressionAssignment::IsLValue(ASTFunction& func)
{
    return true;
}

llvm::Value* ASTExpressionAssignment::Compile(llvm::IRBuilder<>& builder, ASTFunction& func)
{
    printf("COMPILING ASSIGNMENT\n");
    fflush(stdout);

    try {
        left->ReturnType(func);
    }
    catch (std::runtime_error& e)
    {
        printf("COMPILING ASSIGNMENT: Failed to get return type, adding variable to stack\n");
        fflush(stdout);

        // ASTFunctionParameter std::tuple<std::unique_ptr<VarType>, std::string>

        std::string variableString;
        try {
            variableString = left->getVariableString();
        }
        catch (std::runtime_error& e)
        {
            throw std::runtime_error("Failed to add variable to stack because LVal is not a variable");
        }

        printf("COMPILING ASSIGNMENT: getting variable string does not error\n");
        fflush(stdout);

        auto stackVarTuple = std::tuple<std::unique_ptr<VarType>, std::string>(
            right->ReturnType(func),
            variableString
        );

        printf("COMPILING ASSIGNMENT: created stack var tuple\n");
        fflush(stdout);

        func.AddStackVar(std::move(stackVarTuple));
        
        printf("COMPILING ASSIGNMENT: added stack var\n");
        fflush(stdout);

        func.SetVariableValue(
            variableString,
            builder.CreateAlloca(
                func.GetVariableType(variableString)->GetLLVMType(builder.getContext()),
                nullptr,
                variableString
            )
        );

        printf("COMPILING ASSIGNMENT: set stack var value\n");
        fflush(stdout);
    }

    // First make sure that the right side is compatible with the left one by casting as needed.
    ASTExpression::ImplicitCast(func, right, left->ReturnType(func).get());

    printf("COMPILING CHECKING ASSIGNMENT LVALUE\n");
    fflush(stdout);

    // Make sure the left side is an L-Value and get its reference/pointer value.
    if (!left->IsLValue(func)) throw std::runtime_error("ERROR: Left side of assignment expression is not an L-Value!");
    llvm::Value* ptr = left->Compile(builder, func);

    printf("COMPILING CREATING ASSIGNMENT STORE\n");
    fflush(stdout);
    // Store the right value into the position pointed to by the left and return the left pointer.
    builder.CreateStore(right->CompileRValue(builder, func), ptr);
    return ptr;
}

std::string ASTExpressionAssignment::ToString(const std::string& prefix)
{
    std::string ret = "(=)\n";
    ret += prefix + "├──" + left->ToString(prefix + "│  ");
    ret += prefix + "└──" + right->ToString(prefix + "   ");
    return ret;
}