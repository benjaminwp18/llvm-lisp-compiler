#include "and.h"

#include "../function.h"

std::unique_ptr<VarType> ASTExpressionAnd::ReturnType(ASTFunction& func)
{
    return VarTypeSimple::BoolType.Copy(); // a && b is always a boolean.
}

bool ASTExpressionAnd::IsLValue(ASTFunction& func)
{
    return false; // && operator works on two R-Values to produce an R-Value.
}

llvm::Value* ASTExpressionAnd::Compile(llvm::IRBuilder<>& builder, ASTFunction& func) // Hm, this isn't the most efficient approach. I can think of a much easier way...
{
    // Make sure to cast both sides as booleans first.
    ASTExpression::ImplicitCast(func, a1, &VarTypeSimple::BoolType);
    ASTExpression::ImplicitCast(func, a2, &VarTypeSimple::BoolType);

    // Create blocks. Check right if left is true. Continue block happens if true.
    auto* funcVal = (llvm::Function*) func.GetVariableValue(func.name);
    llvm::BasicBlock* checkRight = llvm::BasicBlock::Create(builder.getContext(), "checkRight", funcVal);
    llvm::BasicBlock* cont = llvm::BasicBlock::Create(builder.getContext(), "cont", funcVal);

    // If left is false, branch to continue block where result will be false
    llvm::Value* leftVal = a1->CompileRValue(builder, func);
    llvm::BasicBlock* lastBlockLeft = builder.GetInsertBlock();
    builder.CreateCondBr(leftVal, checkRight, cont);

    // Compile the right expression if needed.
    builder.SetInsertPoint(checkRight);
    llvm::Value* rightVal = a2->CompileRValue(builder, func);
    llvm::BasicBlock* lastBlockRight = builder.GetInsertBlock(); // In case the block has changed, fix it.

    // Tell LLVM that it should either select the left value or the right one depending on where we came from.
    builder.SetInsertPoint(cont);
    llvm::PHINode* res = builder.CreatePHI(VarTypeSimple::BoolType.GetLLVMType(builder.getContext()), 2);
    res->addIncoming(leftVal, lastBlockLeft);
    res->addIncoming(rightVal, lastBlockRight);
    return res;
}

std::string ASTExpressionAnd::ToString(const std::string& prefix)
{
    std::string ret = "(&&)\n";
    ret += prefix + "├──" + a1->ToString(prefix + "│  ");
    ret += prefix + "└──" + a2->ToString(prefix + "   ");
    return ret;
}