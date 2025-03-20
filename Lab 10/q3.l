%{
#include "q3.tab.h"
%}

%%

[0-9]+       { yylval = atoi(yytext); return NUM; }  
[ \t]         { /* Skip whitespace */ }
[\n]          { return '\n'; }
"+"           { return '+'; }
"-"           { return '-'; }
"*"           { return '*'; }
"/"           { return '/'; }
"("           { return '('; }
")"           { return ')'; }

.             { return yytext[0]; }  

%%

int yywrap() {
    return 1;
}
