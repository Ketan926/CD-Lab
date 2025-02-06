%{
#include <stdio.h>
#include <string.h>

int chars = 0;    
int words = 0;
int lines = 0;      
%}

%%
[a-zA-Z]+ {  
    words ++;
    chars += strlen(yytext); 
}
\n { 
    chars++; 
    lines++; 
}
. { 
    chars++; 
}
%%

int main(int argc, char **argv) 
{
    printf("Enter text:\n");
    yylex();
    printf("\nCharacters: %d, Words: %d, Lines: %d\n", chars, words, lines);
    return 0;
}

int yywrap() {
    return 1;
}