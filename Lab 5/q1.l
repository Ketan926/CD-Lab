%{
#include <stdio.h>
#include <string.h>

int chars = 0;    
int vowels = 0;     
int consonants = 0;     
%}

%%
[aAeEiIoOuU]+ { 
    vowels += strlen(yytext); 
    chars += strlen(yytext); 
}
\n { 
    chars++;  
}
[a-zA-Z] { 
    consonants++; 
    chars++; 
}
. { 
    chars++; 
}
%%

int main(int argc, char **argv) 
{
    yylex();
    printf("\nCharacters: %d, Vowels: %d, Consonants: %d\n", chars, vowels, consonants);
    return 0;
}

int yywrap() {
    return 1;
}