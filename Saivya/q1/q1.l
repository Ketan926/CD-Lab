%{
#include "y.tab.h"
%}

%%
"int"           { return INT; }
"float"         { return FLOAT; }
"char"          { return CHAR; }
[0-9]+          { return NUM; }
[a-zA-Z_][a-zA-Z0-9_]* { return ID; }
";"             { return SEMI; }
","             { return COMMA; }
"="             { return ASSIGN; }
[ \t\n]+       { /* Skip whitespace */ }
.               { return yytext[0]; }
%%

int yywrap() { return 1; }
