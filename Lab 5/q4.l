%{
#include <stdio.h>
#include <string.h>

int printf_count = 0;
int scanf_count = 0;    
%}

%%
"printf" {
    printf_count++;
    fprintf(yyout, "WRITE");
}
"scanf" {
    scanf_count++;
    fprintf(yyout, "READ");
}
\n { 
    fprintf(yyout, "\n");
}
. { 
    fputc(yytext[0], yyout);
}
%%

int main(int argc, char **argv) 
{
    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");
    yyin = input_file;
    yyout = output_file; 
    yylex();
    fclose(input_file);
    fclose(output_file);
    printf("\nPrintf count %d, Scanf count %d\n", printf_count, scanf_count);
    return 0;
}

int yywrap() {
    return 1;
}