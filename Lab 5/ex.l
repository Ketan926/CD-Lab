%{
#include <stdio.h>
%}

%%
[a-zA-Z_][a-zA-Z0-9_]* { 
    printf("Identifier: %s\n", yytext); 
}
.|\n { 
    // Ignore other characters and newlines
}
%%

int yywrap() { 
    return 1; 
}

int main() {
    printf("Enter the valid C statement: ");
    yylex(); // Start scanning input
    return 0;
}