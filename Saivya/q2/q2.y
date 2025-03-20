%{
#include <stdio.h>
#include <stdlib.h>
int yylex();
int yyerror(char *msg);
%}

%token IF ELSE LPAREN RPAREN LBRACE RBRACE ID NUM GT ASSIGN SEMI
%left GT

%%
program:
      stmt { printf("Valid decision statement\n"); exit(0); }
    ;
stmt:
      matched_stmt
    | unmatched_stmt
    ;
matched_stmt:
      IF LPAREN expr RPAREN matched_stmt ELSE matched_stmt
    | other_stmt
    ;
unmatched_stmt:
      IF LPAREN expr RPAREN stmt
    ;
other_stmt:
      block
    | assign_stmt
    ;
block:
      LBRACE stmt_list RBRACE
    ;
stmt_list:
      
    | stmt_list stmt
    ;
assign_stmt:
      ID ASSIGN expr SEMI
    ;
expr:
      expr GT expr
    | ID
    | NUM
    ;
%%

int yyerror(char *msg)
{
    printf("Invalid decision statement\n");
    exit(0);
}

int main(void)
{
    printf("Enter a decision making statement:\n");
    yyparse();
    return 0;
}
