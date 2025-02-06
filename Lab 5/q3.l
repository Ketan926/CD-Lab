%{
#include <stdio.h>
#include <string.h>

int pos_int = 0;
int neg_int = 0;
int pos_float = 0;
int neg_float = 0;    
%}

%%
[0-9]+ {  
    pos_int++;
}
[-][0-9]+ {
    neg_int++;
}
[0-9]+[.][0-9]+ {  
    pos_float++;
}
[-][0-9]+[.][0-9]+ {  
    neg_float++;
}
\n { 
}
. { 
}
%%

int main(int argc, char **argv) 
{
    printf("Enter text:\n");
    yylex();
    printf("\nPositive Integers: %d, Negative Integers: %d, Positive Floats: %d, Negative Floats: %d\n", pos_int, neg_int, pos_float, neg_float);
    return 0;
}

int yywrap() {
    return 1;
}