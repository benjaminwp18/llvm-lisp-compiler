#include "for.h"

#include "../function.h"

std::unique_ptr<VarType> ASTStatementFor::StatementReturnType(ASTFunction& func)
{

    // It is completely possible for a while's condition to never be true, so even if does return something it's not confirmed.
    return nullptr;

}

void ASTStatementFor::Compile(llvm::Module& mod, llvm::IRBuilder<>& builder, ASTFunction& func)
{
    // Create the basic blocks.
    auto* funcVal = (llvm::Function*) func.GetVariableValue(func.name);
    auto forLoop = llvm::BasicBlock::Create(builder.getContext(), "forLoop", funcVal);
    auto forLoopBody = llvm::BasicBlock::Create(builder.getContext(), "forLoopBody", funcVal);
    auto forLoopEnd = llvm::BasicBlock::Create(builder.getContext(), "forLoopEnd", funcVal);

    printf(initStatement ? "INIT! " : "NO INIT ");
    printf(condition ? "COND! " : "NO COND ");
    printf(incrementStatement ? "INCR! " : "NO INCR ");
    printf("\n");
    fflush(stdout);

    if (initStatement) {
        initStatement->Compile(mod, builder, func);
    }

    builder.CreateBr(forLoop);
    builder.SetInsertPoint(forLoop);
    if (condition) {
        auto conditionVal = condition->CompileRValue(builder, func);
        builder.CreateCondBr(conditionVal, forLoopBody, forLoopEnd);
    }
    else {
        builder.CreateBr(forLoopBody);
    }

    builder.SetInsertPoint(forLoopBody);
    bodyStatement->Compile(mod, builder, func);
    if (!bodyStatement->StatementReturnType(func)) {
        if (incrementStatement) {
            incrementStatement->Compile(mod, builder, func);
        }
        builder.CreateBr(forLoop);
    }
    
    builder.SetInsertPoint(forLoopEnd);
}

std::string ASTStatementFor::ToString(const std::string& prefix)
{
    std::string output = "for\n";
    output += prefix + "├──" + 
                (initStatement ? initStatement->ToString(prefix + "│  ") : "no loop init\n");
    output += prefix + "├──" +
                (condition ? condition->ToString(prefix + "│  ") : "no loop condition\n");
    output += prefix + "├──" +
                (incrementStatement ? incrementStatement->ToString(prefix + "│  ") : "no loop increment\n");
    output += prefix + "└──" + bodyStatement->ToString(prefix + "   ");
    return output;
}