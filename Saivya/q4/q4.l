%{
#include "y.tab.h"
#include <stdlib.h>
%}

%%
[0-9]+          { yylval = atoi(yytext); return NUMBER; }
\n              { return '\n'; }
"n"             { return 'n'; }
"+"             { return '+'; }
"-"             { return '-'; }
"*"             { return '*'; }
"/"             { return '/'; }
"^"             { return '^'; }
[ \t]+         { /* skip whitespace */ }
.              { return yytext[0]; }
%%

int yywrap(void) { return 1; }
