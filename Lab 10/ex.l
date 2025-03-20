%{
#include "ex.tab.h" // Include the Bison header
%}

%%
[0-9]+      { return NUMBER; }
\n          { return NL; }
[a-zA-Z][a-zA-Z0-9_]* { return ID; }
[ \t]+      { /* Ignore whitespace */ }
.           { return yytext[0]; }
%%

int yywrap() {
    return 1;
}
