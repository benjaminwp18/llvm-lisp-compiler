%code requires {
  #include <stdio.h>
  #include <stdlib.h>
  #include <vector>
  #include <string>
  #include <cstring>
  #include <variant>
  #include <iostream>
  // All of these includes are done as relative paths starting from the build/ directory, since that's where CMake places parser.tab.cc
  #include "../src/ast.h"
  #include "../src/expressions/call.h"
  #include "../src/expressions/int.h"
  #include "../src/expressions/float.h"
  #include "../src/expressions/bool.h"
  #include "../src/expressions/string.h"
  #include "../src/expressions/variable.h"
  #include "../src/expressions/negative.h"
  #include "../src/expressions/addition.h"
  #include "../src/expressions/subtraction.h"
  #include "../src/expressions/multiplication.h"
  #include "../src/expressions/division.h"
  #include "../src/expressions/assignment.h"
  #include "../src/expressions/comparison.h"
  #include "../src/expressions/and.h"
  #include "../src/expressions/or.h"
  #include "../src/statements/expressionSet.h"
  #include "../src/statements/ifExpr.h"
  #include "../src/types/simple.h"
  extern FILE *yyin;
}

%{
  #include "parser.tab.hh"

  extern int yylex(void);
  void yyerror(const char *s);
  void save_to_dot(FILE *);
  int trav_and_write(FILE *, node *);

  AST ast("LispModule");
%}

%start program

%define parse.error verbose

 /* You'll notice that the union has many more types than previously. Read over it to make sure you know what everything does.
  * In particular, note that we do not store objects (or structs) in the union. Instead, it is better practice to store pointers. */
%union {
  bool boolval;
  int intval;
  double fltval;
  char *strval;
  struct node *nodeval;
  ASTFunctionParameter *var;
  std::vector<ASTFunctionParameter *> *vars;
  ASTStatement *stmt;
  std::vector<ASTStatement *> *stmtVec;
  ASTExpression *exp;
  std::vector<ASTExpression *> *exprVec;
  VarType *type;
  ASTExpressionComparisonType rel;
}

%token ID BOOL_TYPE INT_TYPE FLOAT_TYPE STRING_TYPE VOID_TYPE SEMICOLON LPAREN RPAREN COMMA LBRACE RBRACE IF ELSE EXPR_SET FUNCTION EQUALS_SIGN LOGICAL_OR LOGICAL_AND LOGICAL_NOT RELOP_GT RELOP_LT RELOP_GE RELOP_LE RELOP_EQ RELOP_NE ARITH_PLUS ARITH_MINUS ARITH_MULT ARITH_DIV ARITH_MOD VARIADIC BOOL_LITERAL INT_LITERAL FLOAT_LITERAL STRING_LITERAL EOL

%type <boolval> BOOL_LITERAL
%type <strval> ID STRING_LITERAL
%type <intval> int_lit INT_LITERAL
%type <fltval> flt_lit FLOAT_LITERAL
/* %type <var> varDec */
%type <vars> params paramList
/* %type <stmt> stmt selStmt */
/* %type <stmtVec> stmts */
%type <exp> expr relExpr primary call constant variable wrappedExpr ifExpr funDef
%type <exprVec> exprs
%type <type> type
%type <rel> relop

//%expect 1  // Shift/reduce conflict when resolving the if/else production; okay

%%
// AST does not support global variables, so the only declarations are functions
program: LPAREN funDef RPAREN;
// decList: decList dec | dec ;
// dec: funDef;

type:
  BOOL_TYPE {
    $$ = new VarTypeSimple(VarTypeSimple::BoolType);
  } |
  INT_TYPE {
    $$ = new VarTypeSimple(VarTypeSimple::IntType);
  } |
  FLOAT_TYPE {
    $$ = new VarTypeSimple(VarTypeSimple::FloatType);
  } |
  STRING_TYPE {
    $$ = new VarTypeSimple(VarTypeSimple::StringType);
  } |
  VOID_TYPE {
    $$ = new VarTypeSimple(VarTypeSimple::VoidType);
  };
/*
varDec:
  type ID {
    // ASTFunctionParameter is just a tuple of a unique pointer to a type and a string (see definition in function.h)
    $$ = new ASTFunctionParameter(std::unique_ptr<VarType>($1), $2);
    printf("Declaring var %s\n", $2);
  };
varDecs:
  varDecs varDec SEMICOLON {
    $$ = $1;  // We know that varDecs is always a pointer to vector of variables, so we can just copy it and push the next variable
    $$->push_back($2);
  } |
  {
    $$ = new std::vector<ASTFunctionParameter *>();
  }; */

/* funDec:
  type ID LPAREN params RPAREN SEMICOLON {
    // create the parameters
    auto parameters = ASTFunctionParameters();
    bool variadic = false;
    for (auto p : *$4) {
      // The AST uses unique pointers for memory purposes, but bison doesn't work well with those, so the parser uses plain C-style pointers.
      // To account for this, make sure to dereference the pointers before using.
      if (p) {
        parameters.push_back(std::move(*p));
      }
      else {
        variadic = true;
      }
    }
    // then make the function
    auto f = ast.AddFunction($2, std::unique_ptr<VarType>($1), std::move(parameters), variadic);
  }; */

funDef:
  FUNCTION type ID LPAREN params RPAREN expr {
    // stmts is a "new std::vector<ASTStatement *>()"
    // int i = 0; i < $8->size(); i++
    // $8->at(i)

    // std::cout << "Compiling " + std::string($2) + "." << std::endl;
    printf("FUNDEF %s\n", $3);

    // std::unique_ptr<ASTStatementBlock> stmtBlock(new ASTStatementBlock());

    printf("MAKING STATEMENTS\n");

    // for (ASTStatement *stmt : *$8) {
    // printf("PUSHING STMT\n");
    // stmtBlock->statements.push_back(std::unique_ptr<ASTStatement>(std::move($8)));
    // }
    printf("ADDING FUNCTION EXPR\n");
    // std::unique_ptr<ASTStatement> statement = std::unique_ptr<ASTStatement>(std::move($8));
    ASTStatementReturn *retStmt = new ASTStatementReturn();
    retStmt->returnExpression = std::unique_ptr<ASTExpression>($7);

    printf("MADE STATEMENTS\n");

    std::vector<ASTFunctionParameter> parameters = ASTFunctionParameters();
    // bool variadic = false;
    printf("MAKING PARAMS\n");
    for (auto p : *$5) {
      // if (p) {
      printf("PUSHING PARAM\n");
      parameters.push_back(std::move(*p));
      // }
      // else {
      //   printf("PUSHING VARIADIC\n");
      //   variadic = true;
      // }
    }
    printf("MADE PARAMS\n");

    ASTFunction *func = ast.AddFunction($3, std::unique_ptr<VarType>($2), std::move(parameters), false);

    // printf("MAKING STACK VARS\n");
    // for (auto p : *$7) {
    //   printf("ADDING STACK VAR\n");
    //   func->AddStackVar(std::move(std::move(*p)));
    // }
    // printf("MADE STACK VARS\n");

    func->Define(std::unique_ptr<ASTStatement>(retStmt));

    printf("DEFINED FUNCTION\n");

    $$ = new ASTExpressionInt(0);
  };
params: paramList | { $$ = new std::vector<ASTFunctionParameter *>(); };
paramList:
  paramList LPAREN type ID RPAREN {  // This works similarly to varDecs
    $$ = $1;
    $$->push_back(new ASTFunctionParameter(std::unique_ptr<VarType>($3), $4));
  } |
  LPAREN type ID RPAREN {
    $$ = new std::vector<ASTFunctionParameter *>();
    $$->push_back(new ASTFunctionParameter(std::unique_ptr<VarType>($2), $3));
  };

expr:
  LPAREN wrappedExpr RPAREN { $$ = $2; } |
  primary { $$ = $1; };
  
wrappedExpr:
  LPAREN wrappedExpr RPAREN { $$ = $2; } |
  EXPR_SET exprs {
    printf("EXPR SET\n");
    auto expressions = new ASTExpressionSet();
    for (auto expr : *$2) {
      expressions->expressions.push_back(std::unique_ptr<ASTExpression>(expr));
    }
    $$ = expressions;
  } |
  ARITH_PLUS expr expr {
    $$ = new ASTExpressionAddition(std::unique_ptr<ASTExpression>($2), std::unique_ptr<ASTExpression>($3));
  } |
  ARITH_MINUS expr expr {
    $$ = new ASTExpressionSubtraction(std::unique_ptr<ASTExpression>($2), std::unique_ptr<ASTExpression>($3));
  } |
  ARITH_MULT expr expr {
    $$ = new ASTExpressionMultiplication(std::unique_ptr<ASTExpression>($2), std::unique_ptr<ASTExpression>($3));
  } |
  ARITH_DIV expr expr {
    $$ = new ASTExpressionDivision(std::unique_ptr<ASTExpression>($2), std::unique_ptr<ASTExpression>($3));
  } |
  LOGICAL_OR expr expr {
    $$ = new ASTExpressionOr(std::unique_ptr<ASTExpression>($2), std::unique_ptr<ASTExpression>($3));
  } |
  LOGICAL_AND expr expr {
    $$ = new ASTExpressionAnd(std::unique_ptr<ASTExpression>($2), std::unique_ptr<ASTExpression>($3));
  } |
  /* LOGICAL_NOT expr {
    $$ = new ASTExpressionNegation(std::unique_ptr<ASTExpression>($2));
  } | */
  EQUALS_SIGN ID expr {
    printf("Assigning to var %s\n", $2);
    $$ = new ASTExpressionAssignment(
      ASTExpressionVariable::Create($2),
      std::unique_ptr<ASTExpression>($3)
    );
  } |
  relExpr { $$ = $1; } |
  ifExpr { $$ = $1; } |
  funDef { $$ = $1; } |
  call { $$ = $1; };

ifExpr:
  IF expr expr expr {
    $$ = new ASTExpressionIf(
      std::unique_ptr<ASTExpression>($2),
      std::unique_ptr<ASTExpression>($3),
      std::unique_ptr<ASTExpression>($4)
    );
  };

relExpr:
  relop expr expr {
    $$ = new ASTExpressionComparison(
      $1,
      std::unique_ptr<ASTExpression>($2),
      std::unique_ptr<ASTExpression>($3)
    );
  };
relop:
  RELOP_GT {
    $$ = ASTExpressionComparisonType::GreaterThan;
  } |
  RELOP_LT {
    $$ = ASTExpressionComparisonType::LessThan;
  } |
  RELOP_GE {
    $$ = ASTExpressionComparisonType::GreaterThanOrEqual;
  } |
  RELOP_LE {
    $$ = ASTExpressionComparisonType::LessThanOrEqual;
  } |
  RELOP_EQ {
    $$ = ASTExpressionComparisonType::Equal;
  } |
  RELOP_NE {
    $$ = ASTExpressionComparisonType::NotEqual;
  };
primary:
  LPAREN primary RPAREN { $$ = $2; } |
  variable |
  constant { $$ = $1; };
variable:
  ID { $$ = new ASTExpressionVariable($1); };
call:
  ID exprs {
    printf("CALLING FUNCTION %s\n", $1);
    fflush(stdout);
    // Convert exprs to a vector of unique ptrs:
    auto expVec = std::vector<std::unique_ptr<ASTExpression>>();
    for (auto a : *$2) {
      expVec.push_back(std::unique_ptr<ASTExpression>(a));
    }

    printf("CREATING EXPRESSSION CALL %s\n", $1);
    fflush(stdout);
    $$ = new ASTExpressionCall(ASTExpressionVariable::Create($1), std::move(expVec));
  } |
  ID {
    // If there are no exprs, then just give it an empty vector
    $$ = new ASTExpressionCall(ASTExpressionVariable::Create($1), std::vector<std::unique_ptr<ASTExpression>>());
  };
exprs:
  exprs expr {
    $$ = $1;
    $$->push_back($2);
  } |
  expr {
    $$ = new std::vector<ASTExpression *>();
    $$->push_back($1);
  };
constant:
  int_lit { $$ = new ASTExpressionInt($1); } |
  flt_lit { $$ = new ASTExpressionFloat($1); } |
  BOOL_LITERAL { $$ = new ASTExpressionBool($1); } |
  STRING_LITERAL { $$ = new ASTExpressionString(std::string($1)); };
int_lit: INT_LITERAL; // | ARITH_MINUS INT_LITERAL { $$ = -1 * $2; };
flt_lit: FLOAT_LITERAL; // | ARITH_MINUS FLOAT_LITERAL { $$ = -1 * $2; };

%%
int main(int argc, char **argv) {

  // Arg flags:
  bool showHelp = false; // Show the help and exit.
  std::string openFile = ""; // File to open. Nothing for standard in.
  std::string outFile = ""; // File to write to. Nothing for standard out.
  int outputFormat = 3; // 0 - LLVM Assembly. 1 - LLVM Bitcode. 2 - Object (TODO). 3 - AST tree.
  bool printAST = true; // If to print the AST to console.

  // Read the arguments. Don't count the first which is the executable name.
  for (int i = 1; i < argc; i++)
  {
    bool hasNextArg = i + 1 < argc;
    std::string arg(argv[i]);
    if (arg == "-i" && hasNextArg)
    {
      i++;
      openFile = argv[i];
    }
    else if (arg == "-o" && hasNextArg)
    {
      i++;
      outFile = argv[i];
    }
    else if (arg == "-nPrint")
    {
      printAST = false;
    }
    else if (arg == "-fAsm")
    {
      outputFormat = 0;
    }
    else if (arg == "-fBc")
    {
      outputFormat = 1;
    }
    else if (arg == "-fObj")
    {
      outputFormat = 2;
    }
    else if (arg == "-fAst")
    {
      outputFormat = 3;
    }
    else
    {
      showHelp = true;
    }
  }
  printAST &= outputFormat != 3 && outFile != ""; // Always print AST by default in addition to whatever is being output.

  // Show help if needed.
  if (showHelp)
  {
    printf("Usage: LLVM-Lab [options]\n");
    printf("\nOptions:\n\n");
    printf("-h              Show this help screen.\n");
    printf("-i [input]      Read from an input file (reads from console by default).\n");
    printf("-o [output]     Write to an output file (writes to console by default).\n");
    printf("-nPrint         If to not print the AST to the console.\n");
    printf("-fAsm           Output format is in LLVM assembly.\n");
    printf("-fAst           Output format is an abstract syntax tree.\n");
    printf("-fBc            Output format is in LLVM bitcode.\n");
    printf("-fObj           Output format is an object file.\n");
    return 1;
  }

  // Fetch input.
  if (openFile != "")
  {
    yyin = fopen(openFile.c_str(), "r");
  }

  if (yyparse() == 1)
  {
    printf("Irrecoverable error state, aborting\n");
    return 1;
  }

  // Close input if needed.
  if (openFile != "")
  {
    fclose(yyin);
  }

  // Do the compilation.
  ast.Compile();

  // Print AST if needed.
  if (printAST) std::cout << ast.ToString() << std::endl;

  // Export data.
  if (outputFormat == 0)
  {
    ast.WriteLLVMAssemblyToFile(outFile);
  }
  else if (outputFormat == 1)
  {
    ast.WriteLLVMBitcodeToFile(outFile);
  }
  else if (outputFormat == 2)
  {
    std::cout << "OBJ exporting not supported yet." << std::endl;
  }
  else
  {
    std::cout << ast.ToString() << std::endl;
  }
  return 0;
}

void yyerror(const char *s)
{
  fprintf(stderr, "error: %s\n", s);
}
