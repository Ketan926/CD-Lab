%{
#include <stdio.h>
#include <stdlib.h>
%}

%token NUMBER
%token LPAREN RPAREN
%left '+' '-'
%left '*' '/'
%left NEG

%%
input:
      /* empty */
    | input line
    ;
line:
      expr '\n' { printf("Result = %d\n", $1); }
    ;
expr:
      expr '+' expr { $$ = $1 + $3; }
    | expr '-' expr { $$ = $1 - $3; }
    | expr '*' expr { $$ = $1 * $3; }
    | expr '/' expr { if($3 == 0) { printf("Division by zero error\n"); exit(1); } else $$ = $1 / $3; }
    | '-' expr %prec NEG { $$ = -$2; }
    | LPAREN expr RPAREN { $$ = $2; }
    | NUMBER { $$ = $1; }
    ;
%%

int main(void)
{
    printf("Enter arithmetic expression:\n");
    yyparse();
    return 0;
}

int yyerror(char *s)
{
    fprintf(stderr, "Error: %s\n", s);
    exit(1);
}
