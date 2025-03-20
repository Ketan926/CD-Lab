%{
#include "y.tab.h"
%}

%%
"if"            { return IF; }
"else"          { return ELSE; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
";"             { return SEMI; }
">"             { return GT; }
"="             { return ASSIGN; }
[0-9]+          { return NUM; }
[a-zA-Z][a-zA-Z0-9]* { return ID; }
[ \t\n]+        { /* Skip whitespace */ }
.               { return yytext[0]; }
%%

int yywrap(void) { return 1; }
