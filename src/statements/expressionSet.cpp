#include "expressionSet.h"

#include "../types/simple.h"

std::unique_ptr<VarType> ASTExpressionSet::ReturnType(ASTFunction& func)
{
    // The type of the return value of the last expression in the set
    auto ret = expressions.back()->ReturnType(func);
    return std::move(ret);
}

bool ASTExpressionSet::IsLValue(ASTFunction& func)
{
    return false;
}

llvm::Value* ASTExpressionSet::Compile(llvm::IRBuilder<>& builder, ASTFunction& func)
{
    printf("COMPILING EXPR SET\n");
    fflush(stdout);
    for (int i = 0; i < expressions.size() - 1; i++)
    {
        printf("COMPILING EXPR IN EXPR SET\n");
        fflush(stdout);
        expressions.at(i)->Compile(builder, func);
        // if (statement->ReturnType(func)) return;
    }

    return expressions.back()->CompileRValue(builder, func);
}

std::string ASTExpressionSet::ToString(const std::string& prefix)
{
    std::string output = "exprSet\n";
    if (expressions.size() > 0) {
      for (int i = 0; i < expressions.size() - 1; i++)
        output += prefix + "├──" + (expressions.at(i) == nullptr ? "nullptr\n" : expressions.at(i)->ToString(prefix + "│  "));
      output += prefix + "└──" + (expressions.back() == nullptr ? "nullptr\n" : expressions.back()->ToString(prefix + "   "));
    }
    return output;
}
