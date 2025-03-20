%{
#include "y.tab.h"
%}

%%
[0-9]+          { yylval = atoi(yytext); return NUMBER; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
\n              { return '\n'; }
[ \t]+          ;
.               { return yytext[0]; }
%%
int yywrap(void) { return 1; }
