%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
int yyerror(char *msg);
%}

%token NUMBER

%%
input:
      /* empty */
    | input line
    ;
line:
      '\n'
    | exp '\n'
    ;
exp:
      NUMBER
    | exp exp '+' 
    | exp exp '-' 
    | exp exp '*' 
    | exp exp '/' 
    | exp exp '^' 
    | exp 'n'
    ;
%%

int yyerror(char *msg)
{
    printf("Invalid postfix expression\n");
    exit(1);
}

int main(void)
{
    printf("Enter postfix expression:\n");
    yyparse();
    printf("Valid postfix expression\n");
    return 0;
}
