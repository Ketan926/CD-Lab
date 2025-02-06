%{
#include <stdio.h>
#include <stdlib.h>
int decimal = 0;
%}

%%
[0-9]+ { 
    decimal = atoi(yytext); 
    printf("Decimal: %d, Hexadecimal: %X\n", decimal, decimal); 
}
[ \t\n]+ ; 
. {  
}
%%

int main() 
{
    printf("Enter decimal numbers:\n");
    yylex(); 
    return 0;
}

int yywrap() {
    return 1; 
}