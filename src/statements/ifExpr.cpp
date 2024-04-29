#include "ifExpr.h"

#include "../function.h"

std::unique_ptr<VarType> ASTExpressionIf::ReturnType(ASTFunction& func)
{
    auto thenRet = thenExpression->ReturnType(func);
    auto elseRet = elseExpression->ReturnType(func);

    // Check for matching return types.
    if (thenRet->Equals(elseRet.get())) return std::move(thenRet); // Return if equal.
    else throw std::runtime_error("ERROR: If/Else expressions both return a value but their return types don't match!");
}

bool ASTExpressionIf::IsLValue(ASTFunction& func)
{
    return false;
}

llvm::Value* ASTExpressionIf::Compile(llvm::IRBuilder<>& builder, ASTFunction& func)
{
    // Compile the condition. TODO: TO BOOLEAN CAST CONVERSION?
    if (!condition->ReturnType(func)->Equals(&VarTypeSimple::BoolType))
        throw std::runtime_error("ERROR: Expected condition that returns a boolean value but got another type instead!");

    llvm::Value* cond = condition->Compile(builder, func);
    llvm::Value* thenVal = thenExpression->Compile(builder, func);
    llvm::Value* elseVal = elseExpression->Compile(builder, func);

    return builder.CreateSelect(cond, thenVal, elseVal);

    // Create blocks.
    // auto* funcVal = (llvm::Function*) func.GetVariableValue(func.name);
    // llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(builder.getContext(), "thenBlock", funcVal);
    // llvm::BasicBlock* elseBlock = nullptr;
    // if (elseStatement) elseBlock = llvm::BasicBlock::Create(builder.getContext(), "elseBlock", funcVal);
    // llvm::BasicBlock* contBlock = llvm::BasicBlock::Create(builder.getContext(), "contBlock", funcVal);

    // Make jumps to blocks.
    // builder.CreateCondBr(cond, thenBlock, elseBlock ? elseBlock : contBlock); // Use else as false if exists, otherwise go to continuation.

    // Compile the then block and then jump to continuation block.
    // builder.SetInsertPoint(thenBlock);
    // thenExpression->Compile(mod, builder, func);
    // builder.CreateBr(contBlock); // Only create branch if no return encountered.

    // Compile the else block if applicable.
    // if (elseBlock)
    // {
    //     builder.SetInsertPoint(elseBlock);
    //     elseStatement->Compile(mod, builder, func);
    //     if (!elseStatement->StatementReturnType(func)) builder.CreateBr(contBlock); // Only create branch if no return encountered.
    // }

    // Resume compilation at continuation block.
    // builder.SetInsertPoint(contBlock);
}

std::string ASTExpressionIf::ToString(const std::string& prefix)
{
    std::string output = "if " + condition->ToString(prefix) + "\n";
    output += prefix + (thenExpression ? "├──" : "└──") + thenExpression->ToString(prefix + "   ");
    output += prefix + (elseExpression ? "├──" : "└──") + elseExpression->ToString(prefix + "   ");
    return output;
}