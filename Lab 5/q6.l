%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>
char word;  
%}

%%
"//".* {
    fprintf(yyout, "%s", yytext);
}

"/*"([^*]|\*+[^*/])*\*+"/" {
    fprintf(yyout, "%s",  yytext);
}
[A-Z] {
    printf("%s",yytext);
    word=tolower(yytext[0]);
    fputc(word,yyout);
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
    printf("\n");
    return 0;
}

int yywrap() {
    return 1;
}