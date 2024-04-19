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
  #include "../src/expressions/int.h"
  #include "../src/expressions/float.h"
  #include "../src/expressions/bool.h"
  #include "../src/expressions/string.h"
  #include "../src/expressions/variable.h"
  #include "../src/expressions/sexpr.h"
  #include "../src/types/simple.h"
  extern FILE *yyin;
}

%{
  #include "parser.tab.hh"

  extern int yylex(void);
  void yyerror(const char *s);
  void save_to_dot(FILE *);
  int trav_and_write(FILE *, node *);

  AST ast("TestMod");
%}

%start termList

%define parse.error verbose

 /* You'll notice that the union has many more types than previously. Read over it to make sure you know what everything does.
  * In particular, note that we do not store objects (or structs) in the union. Instead, it is better practice to store pointers. */
%union {
  bool boolval;
  int intval;
  double fltval;
  char *strval;
  struct node *nodeval;
  ASTExpression *exp;
  std::vector<ASTExpressionSexpr *> *expVec;
}

%token ID LPAREN RPAREN BOOL_LITERAL INT_LITERAL FLOAT_LITERAL STRING_LITERAL EOL

%type <boolval> BOOL_LITERAL
%type <strval> ID STRING_LITERAL
%type <intval> INT_LITERAL
%type <fltval> FLOAT_LITERAL
%type <exp> sexpr lispTerm constant
%type <expVec> termList

%%
sexpr:
	LPAREN termList RPAREN { $$ = new ASTExpressionSexpr(std::move($2)); }
	| LPAREN RPAREN { $$ = new ASTExpressionSexpr(std::vector<std::unique_ptr<ASTExpression>>()); }
	;

termList:
	termList lispTerm {
		$$ = $1;
		$$->push_back($2);
	} |
	lispTerm {
		$$ = new std::vector<ASTExpression *>();
		$$->push_back($1);
	};

lispTerm:
	constant { $$ = $1; } |
	sexpr { $$ = $1; } |
	ID { $$ = new ASTExpressionVariable($1); }
	;

constant:
  INT_LITERAL { $$ = new ASTExpressionInt($1); } |
  FLOAT_LITERAL { $$ = new ASTExpressionFloat($1); } |
  BOOL_LITERAL { $$ = new ASTExpressionBool($1); } |
  STRING_LITERAL { $$ = new ASTExpressionString(std::string($1)); };

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
