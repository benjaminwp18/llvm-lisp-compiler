%{
#include "parser.tab.hh"
#include <sstream>
#include <string>

static std::stringstream ss;

const bool LEXER_DEBUG = true;

void debugPrint(std::string message, std::string text = "") {
  if (LEXER_DEBUG) {
    printf("LEXER: %s%s\n", message.c_str(), (!text.empty() ? " '" + text + "'" : "").c_str());
  }
}
%}

%option noyywrap

%x strlit

%%
"int"     { debugPrint("Found", yytext); return INT_TYPE; }
"float"   { debugPrint("Found", yytext); return FLOAT_TYPE; }
"string"  { debugPrint("Found", yytext); return STRING_TYPE; }
"void"    { debugPrint("Found", yytext); return VOID_TYPE; }
"bool"    { debugPrint("Found", yytext); return BOOL_TYPE; }
"("       { debugPrint("Found", yytext); return LPAREN; }
")"       { debugPrint("Found", yytext); return RPAREN; }
"if"      { debugPrint("Found", yytext); return IF; }
"or"      { debugPrint("Found", yytext); return LOGICAL_OR; }
"and"     { debugPrint("Found", yytext); return LOGICAL_AND; }
"not"     { debugPrint("Found", yytext); return LOGICAL_NOT; }
">"       { debugPrint("Found", yytext); return RELOP_GT; }
"<"       { debugPrint("Found", yytext); return RELOP_LT; }
">="      { debugPrint("Found", yytext); return RELOP_GE; }
"<="      { debugPrint("Found", yytext); return RELOP_LE; }
"=="      { debugPrint("Found", yytext); return RELOP_EQ; }
"!="      { debugPrint("Found", yytext); return RELOP_NE; }
"+"       { debugPrint("Found", yytext); return ARITH_PLUS; }
"-"       { debugPrint("Found", yytext); return ARITH_MINUS; }
"*"       { debugPrint("Found", yytext); return ARITH_MULT; }
"/"       { debugPrint("Found", yytext); return ARITH_DIV; }
false|true          { debugPrint("Found bool lit", yytext); yylval.boolval = strcmp(yytext, "true") == 0; return BOOL_LITERAL; }
[0-9]+              { debugPrint("Found int lit", yytext); yylval.intval = atoi(yytext); return INT_LITERAL; }
([0-9]+[.])?[0-9]+  { debugPrint("Found float lit", yytext); yylval.fltval = atof(yytext); return FLOAT_LITERAL; }

\"                  { debugPrint("Found str lit start", yytext); BEGIN strlit; ss.str(std::string()); }
<strlit>[^\\"\n]*   { debugPrint("Found str lit regular chars", yytext); ss << yytext;}
<strlit>\\n         { debugPrint("Found str lit (escaped) line break", yytext); ss << '\n';}
<strlit>\\t         { debugPrint("Found str lit (escaped) tab", yytext); ss << '\t';}
<strlit>\\[\\"]     { debugPrint("Found str lit escaped quote/backslash", yytext); ss << yytext[1]; /*escaped quote or backslash*/ }
<strlit>\"          { debugPrint("Found str lit end", yytext); yylval.strval = strdup((char *) ss.str().c_str()); BEGIN 0; return STRING_LITERAL; }
<strlit>\\.         { debugPrint("Found invalid escaped char", yytext); }
<strlit>\n          { debugPrint("Found str lit (unescaped) newline", yytext); }

[_a-zA-Z][_a-zA-Z0-9]* { debugPrint("Found ID", yytext); yylval.strval = strdup(yytext); return ID; }
[\r\n\t ]+ { debugPrint("Found whitespace"); }
. { debugPrint("Unrecognized character", yytext); }
%%

  /*int main(int argc, char **argv) {
  int tok;

  while((tok = yylex())) {
    printf("%d", tok);
    if(tok == INT_LITERAL) printf(" = %d\n", yyintval);
    else if (tok == FLOAT_LITERAL) printf(" = %0.3f\n", yyfltval);
    else if (tok == STRING_LITERAL) printf(" = %s\n", yytext);
    else if (tok == ID) printf(" called %s\n", yytext);
    else printf("\n");
  }
}
  */
