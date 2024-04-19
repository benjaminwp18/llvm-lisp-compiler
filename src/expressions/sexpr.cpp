#include "sexpr.h"

std::unique_ptr<VarType> ASTExpressionSexpr::ReturnType(ASTFunction& func)
{
    // TODO: update
    return VarTypeSimple::BoolType.Copy();
}

bool ASTExpressionSexpr::IsLValue(ASTFunction& func)
{
    return false;
}

llvm::Value* ASTExpressionSexpr::Compile(llvm::IRBuilder<>& builder, ASTFunction& func)
{
    //printf("COMPILING FUNCTION CALL\n");
    //fflush(stdout);

    //// Get the function type to make sure we are returning a function.
    //auto calleeType = callee->ReturnType(func);
    //auto funcType = dynamic_cast<VarTypeFunction*>(calleeType.get());
    //if (!funcType)
    //{
    //    throw std::runtime_error("ERROR: Trying to call a value that isn't a function!");
    //}

    //// Type check parameters to make sure it satisfies the call.
    //int minParameters = (int) funcType->parameterTypes.size();
    //int maxParameters = funcType->varArgs ? INT_MAX : minParameters; // We can either pass in the minimum number of arguments or infinitely many.
    //if (arguments.size() < minParameters || arguments.size() > maxParameters)
    //    throw std::runtime_error("ERROR: Invalid number of arguments in function call! Expected " + std::to_string(minParameters) + " to " + std::to_string(maxParameters) + " arguments, but got " + std::to_string(arguments.size()) + "!");
    //for (int i = 0; i < minParameters; i++) // We only need to check the non-variadic parameters.
    //{
    //    auto argReturnType = arguments[i]->ReturnType(func);
    //    if (!funcType->parameterTypes[i]->Equals(argReturnType.get())) // The value types are not equal, try seeing if it's a float we need to cast an int to.
    //    {
    //        if (!(funcType->parameterTypes[i]->Equals(&VarTypeSimple::FloatType) && argReturnType->Equals(&VarTypeSimple::IntType))) // Uncastable.
    //            throw std::runtime_error("ERROR: Invalid type passed to function!");
    //        else
    //        {
    //            auto tmp = std::move(arguments[i]);
    //            arguments[i] = std::make_unique<ASTExpressionInt2Float>(std::move(tmp)); // Cast the int expression to float.
    //        }
    //    }
    //}

    //// Compile all the values and then perform a call. It's important for everything to be R-Values.
    //llvm::Value* calleeVal = callee->CompileRValue(builder, func);
    //std::vector<llvm::Value*> argumentVals;
    //for (auto& arg : arguments)
    //{
    //    argumentVals.push_back(arg->CompileRValue(builder, func));
    //}
    //return builder.CreateCall((llvm::Function*)calleeVal, argumentVals);
}

std::string ASTExpressionSexpr::ToString(const std::string& prefix)
{
    std::string output;
    //for (int i = 0; i < arguments.size() - 1; i++)
    //    output += prefix + "├──" + (arguments[i] == nullptr ? "nullptr\n" : arguments[i]->ToString(prefix + "│  "));
    //output += prefix + "└──" + (arguments.back() == nullptr ? "nullptr\n" : arguments.back()->ToString(prefix + "   "));
    return output;
}
