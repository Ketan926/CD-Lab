%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
int yyerror(char *msg);
%}

/* Token definitions */
%token INT FLOAT CHAR ID NUM SEMI COMMA ASSIGN

%%
decl_stmt: type_specifier decl_list SEMI 
            { printf("Valid Declaration\n"); exit(0); }
         ;

type_specifier: INT 
              | FLOAT 
              | CHAR
              ;

decl_list: decl 
         | decl_list COMMA decl
         ;

decl: ID 
    | ID ASSIGN NUM
    ;
%%

int yyerror(char *msg)
{
    printf("Invalid Declaration\n");
    exit(0);
}

int main(void)
{
    printf("Enter a declaration statement:\n");
    yyparse();
    return 0;
}
