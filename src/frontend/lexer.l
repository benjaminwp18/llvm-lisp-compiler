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
"("       { debugPrint("Found", yytext); return LPAREN; }
")"       { debugPrint("Found", yytext); return RPAREN; }
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

[_a-zA-Z0-9]+ { debugPrint("Found ID", yytext); yylval.strval = strdup(yytext); return ID; }
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
