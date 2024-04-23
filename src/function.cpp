#include "function.h"
#include <iostream>

#include "ast.h"
#include "types/simple.h"
#include <llvm/IR/Verifier.h>

ASTFunction::ASTFunction(AST& ast, const std::string& name, std::unique_ptr<VarType> returnType, ASTFunctionParameters parameters, bool variadic) : ast(ast), name(name)
{

    // Create the function type.
    auto params = std::move(parameters);
    std::vector<std::unique_ptr<VarType>> paramTypes;
    for (auto& param : params) // We must copy each type from the parameters.
    {
        paramTypes.push_back(std::get<0>(param)->Copy()); // This copies the first item of the tuple, which is a var type pointer.
    }
    funcType = std::make_unique<VarTypeFunction>(std::move(returnType), std::move(paramTypes), variadic);

    // Add to scope table, we need to error if it already exists.
    if (!ast.scopeTable.AddVariable(name, funcType->Copy()))
    {
        throw std::runtime_error("ERROR: Function or global variable with name " + name + " already exists.");
    }

    // Add parameters as stack variables. It's ok for us to do it since we are the ones setting up the parameters for stack variables.
    for (auto& param : params)
    {
        this->parameters.push_back(std::get<1>(param));
        AddStackVar(std::move(param));
    }

}

void ASTFunction::AddStackVar(ASTFunctionParameter var, bool allowRedefns)
{
    if (scopeTable.AddVariable(std::get<1>(var), std::move(std::get<0>(var)))) {
        stackVariables.push_back(std::get<1>(var));
    } else {
        std::cout << "=====redef " + std::get<1>(var) + "=====\n";
        if (!allowRedefns)
        throw std::runtime_error("ERROR: Variable " + std::get<1>(var) + " is already defined in function " + name + "!");
    }
}

VarType* ASTFunction::GetVariableType(const std::string& name)
{
    VarType* ret;
    if (ret = scopeTable.GetVariableType(name), !ret)
    {
        InferVarDecl(name);
    if (ret = scopeTable.GetVariableType(name), !ret) {
            throw std::runtime_error("ERROR123: In function " + this->name + ", cannot resolve variable or function " + name + " to get its type");
    }
        //if (ret = ast.scopeTable.GetVariableType(name), !ret) // Continue only if AST scope table doesn't have value.
        //{
        //    // TODO: properly infer the type. For now just assume int
        //    throw std::runtime_error("ERROR: In function " + this->name + ", cannot resolve variable or function " + name + " to get its type");
        //}
        //else return ret;
    }
    return ret;
}

llvm::Value* ASTFunction::GetVariableValue(const std::string& name)
{
    llvm::Value* ret;
    if (ret = scopeTable.GetVariableValue(name), !ret) // Continue only if function scope table doesn't have value.
    {
        InferVarDecl(name);
    ret = scopeTable.GetVariableValue(name);
        if (!ret) {
        }
        //if (ret = ast.scopeTable.GetVariableValue(name), !ret) // Continue only if AST scope table doesn't have value.
        //{
        //    throw std::runtime_error("ERROR: In function " + this->name + ", cannot resolve variable or function " + name + " to get its value");
        //}
        //else return ret;
    }
    //else return ret;
    return ret;
}

void ASTFunction::SetVariableValue(const std::string& name, llvm::Value* value)
{
    if (!scopeTable.SetVariableValue(name, value)) // Continue only if function scope table doesn't have value.
    {
    InferVarDecl(name);
        //if (!ast.scopeTable.SetVariableValue(name, value)) // Continue only if AST scope table doesn't have value.
        //{
        //    throw std::runtime_error("ERROR: In function " + this->name + ", cannot resolve variable or function " + name + " to set it");
        //}
    }
}

void ASTFunction::Define(std::unique_ptr<ASTStatement> definition)
{
    if (!this->definition) // Define only if not already defined.
    {
        this->definition = std::move(definition);
    }
    else throw std::runtime_error("ERROR: Function " + name + " already has a definition!");
}

void ASTFunction::Compile(llvm::Module& mod, llvm::IRBuilder<>& builder)
{
    std::cout << "Compiling function " + name + "." << std::endl;
    printf("COMPILING FUNCTION %s\n", name.c_str());
    fflush(stdout);

    // First, add a new function declaration to our scope.
    auto func = llvm::Function::Create((llvm::FunctionType*)funcType->GetLLVMType(builder.getContext()), llvm::GlobalValue::LinkageTypes::ExternalLinkage, name, mod);
    ast.scopeTable.SetVariableValue(name, func);

    printf("COMPILING: SET PARAM NAMES FUNCTION %s\n", name.c_str());
    fflush(stdout);

    // Set parameter names.
    unsigned idx = 0;
    for (auto& arg : func->args()) arg.setName(parameters[idx++]);

    printf("COMPILING: CHECK IF DEFINED FUNCTION %s\n", name.c_str());
    fflush(stdout);

    // Only continue if the function has a definition.
    if (!definition) return;

    printf("COMPILING: CREATE BASIC BLOCK FUNCTION %s\n", name.c_str());
    fflush(stdout);

    // Create a new basic block to start insertion into.
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(builder.getContext(), "entry", func);
    builder.SetInsertPoint(bb);

    /*
        So there's a lot going on here and it needs a bit of explanation.
        In LLVM, registers can only be assigned once which is not what we want for mutable variables.
        In order to combat this, we allocate memory on the stack to an LLVM register (llvm::Value*).
        This works, as while the memory location itself is constant, we can load and store to that stack location as much as we want.
        We can allocate stack memory with an "alloca" instruction.
        This must be done in the entry block for stack allocations to be automatically optimized to registers on the target machine where appropriate.
        Once we allocate memory on the stack for each stack variable, we must then store it to the scope table so we know where each variable "lives".
        Now that we did that, we can use our GetVariableValue and SetVariableValue functions to get the pointer to the variable, which we can load from or store to!
        Note that how the stored stack variables are pointers to a value is what makes us classify it as an L-Value!
    */
    printf("COMPILING STACK VARS\n");
    fflush(stdout);
    for (auto& stackVar : stackVariables)
    {
        scopeTable.SetVariableValue(
            stackVar,
            builder.CreateAlloca(scopeTable.GetVariableType(stackVar)->GetLLVMType(builder.getContext()), nullptr, stackVar)
        );
    }

    printf("COMPILING STACK VAR INIT VALS\n");
    fflush(stdout);
    // Now we need to store the initial values of the function arguments into their stack equivalents.
    for (auto& arg : func->args())
    {
        builder.CreateStore(&arg, scopeTable.GetVariableValue(arg.getName().data())); // We are storing the argument into the pointer to the stack variable gotten by fetching it from the scope table.
    }

    printf("COMPILING CHECKING FUNC RETURN TYPE\n");
    fflush(stdout);
    // Check the function body to make sure it returns what we expect it to.
    std::unique_ptr<VarType> retType = definition->StatementReturnType(*this);
    bool satisfiesType = !retType && funcType->returnType->Equals(&VarTypeSimple::VoidType); // If we return nothing and expect void, it works.
    if (!satisfiesType && retType) satisfiesType = retType->Equals(funcType->returnType.get()); // If we return something, make sure we return what is expected.
    if (!satisfiesType)
    {
        throw std::runtime_error("ERROR: Function " + name + " does not return what it should!");
    }

    // Generate the function.
    definition->Compile(mod, builder, *this);

    // Add an implicit return void if necessary.
    if (!retType)
    {
        builder.CreateRetVoid();
    }

    // Verify and optimize the function.
    llvm::verifyFunction(*func, &llvm::errs());
    ast.fpm.run(*func);

}

std::string ASTFunction::ToString(const std::string& prefix)
{
    std::string output = name + "\n";
    output += prefix + "└──" + (definition == nullptr ? "nullptr\n" : definition->ToString(prefix + "   "));
    return output;
}

void ASTFunction::InferVarDecl(const std::string& name)
{
    // Right now we just always infer the Int type.
    // TODO: be smarter
    AddStackVar(std::make_tuple(VarTypeSimple::IntType.Copy(), std::string(name)), true);
    scopeTable.SetVariableValue(
        name,
	llvm::ConstantInt::get(llvm::Type::getInt32Ty(ast.context), 0)
    );
}
