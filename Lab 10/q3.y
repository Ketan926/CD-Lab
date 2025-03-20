%{
#include <stdio.h>
#include <stdlib.h>

int yylex();  
void yyerror(const char *s);  

%}

%token NUM
%left '+' '-'
%left '*' '/'
%left NEG // For unary minus
%left '^'

%%

input:
    | input line
    ;

line:
    '\n'                  { /* Empty line */ }
    | expr '\n'           { printf("Result: %d\n", $1); }
    ;

expr:
      NUM                   { $$ = $1; }        
    | expr '+' expr          { $$ = $1 + $3; }  
    | expr '-' expr          { $$ = $1 - $3; }  
    | expr '*' expr          { $$ = $1 * $3; }  
    | expr '/' expr          { if ($3 == 0) { yyerror("Division by zero!"); } else { $$ = $1 / $3; } }
    | '(' expr ')'           { $$ = $2; }       
    | '-' expr %prec NEG     { $$ = -$2; }       
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main() {
    printf("Enter an arithmetic expression: ");
    yyparse();  
    return 0;
}
