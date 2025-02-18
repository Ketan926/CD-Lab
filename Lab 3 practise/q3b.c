#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct token {
    char token[100];
    int row;
    int col;
    char type[20];
    int index;
};
struct token table[100];

const char *keywords[] = {
    "int", "float", "double", "char", "if", "else", "for", "return",
    "while", "return", "void", "switch", "case", "break", "continue",
    "default", "struct", "union", "enum", "long", "short", "const",
    "sizeof", "printf", "scanf"
};
#define NUM_KEYWORDS 25

int iskeyword(char *word) 
{
    for (int i = 0; i < NUM_KEYWORDS; i++) 
    {
        if (strcmp(word, keywords[i]) == 0) 
        {
            return 1;
        }
    }
    return 0;
}

void generate_token(FILE *f1, FILE *f2) 
{
    char c, next;
    int row = 1, col = 1, i = 0;
    int cur_row, cur_col;
    char word[50];
    char number[20];
    char string[100];
    int ind = 0;

    if (!f1 || !f2) 
    {
        printf("Error opening files!\n");
        return;
    }

    while ((c = getc(f1)) != EOF) 
    {
        if (isspace(c)) 
        {
            if (c == '\n') 
            {
                row++;
                col = 1;
            } 
            else 
            {
                col++;
            }
            continue;
        }

        cur_row = row;
        cur_col = col;

        if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') 
        {
            table[ind].token[0] = c;
            table[ind].token[1] = '\0';
            table[ind].row = cur_row;
            table[ind].col = cur_col;
            strcpy(table[ind].type, "Arithmetic Operator");
            table[ind].index = ind + 1;
            fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col++;
        }

        else if (c == '=' || c == '!' || c == '>' || c == '<') 
        {
            next = getc(f1);
            if (next == '=') 
            {
                table[ind].token[0] = c;
                table[ind].token[1] = next;
                table[ind].token[2] = '\0';
                col++;
            } 
            else 
            {
                table[ind].token[0] = c;
                table[ind].token[1] = '\0';
                ungetc(next, f1);
            }
            table[ind].row = cur_row;
            table[ind].col = cur_col;
            if (c == '=') 
                strcpy(table[ind].type, "Assignment Operator");
            else 
                strcpy(table[ind].type, "Relational Operator");
            table[ind].index = ind + 1;
            fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col++;
        }

        else if (c == '&' || c == '|') 
        {
            next = getc(f1);
            if ((c == '&' && next == '&') || (c == '|' && next == '|')) 
            {
                table[ind].token[0] = c;
                table[ind].token[1] = next;
                table[ind].token[2] = '\0';
                col++;
            } 
            else 
            {
                table[ind].token[0] = c;
                table[ind].token[1] = '\0';
                ungetc(next, f1);
            }
            table[ind].row = cur_row;
            table[ind].col = cur_col;
            strcpy(table[ind].type, "Logical Operator");
            table[ind].index = ind + 1;
            fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col++;
        }

        else if (isdigit(c)) 
        {
            i = 0;
            number[i++] = c;
            while (isdigit(c = getc(f1)) || c == '.') 
            {
                number[i++] = c;
            }
            number[i] = '\0';
            ungetc(c, f1);
            table[ind].row = cur_row;
            table[ind].col = cur_col;
            strcpy(table[ind].token, number);
            strcpy(table[ind].type, "Number");
            table[ind].index = ind + 1;
            fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col += i;
        }

        else if (isalpha(c) || c == '_') 
        {
            i = 0;
            word[i++] = c;
            while (isalnum(c = getc(f1)) || c == '_') 
            {
                word[i++] = c;
            }
            word[i] = '\0';
            ungetc(c, f1);
            table[ind].row = cur_row;
            table[ind].col = cur_col;
            strcpy(table[ind].token, word);
            if (iskeyword(word))
                strcpy(table[ind].type, "Keyword");
            else
                strcpy(table[ind].type, "Identifier");
            table[ind].index = ind + 1;
            fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col += i;
        }

        else if (c == '"') 
        {
            i = 0;
            while ((c = getc(f1)) != '"' && c != EOF) 
            {
                string[i++] = c;
            }
            string[i] = '\0';
            table[ind].row = cur_row;
            table[ind].col = cur_col;
            strcpy(table[ind].token, string);
            strcpy(table[ind].type, "String Literal");
            table[ind].index = ind + 1;
            fprintf(f2, "<%d, '\"%s\"', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col += i;
        }

        else 
        {
            table[ind].token[0] = c;
            table[ind].token[1] = '\0';
            table[ind].row = cur_row;
            table[ind].col = cur_col;
            strcpy(table[ind].type, "Special Character");
            table[ind].index = ind + 1;
            fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col++;
        }
    }
}

int main() 
{
    FILE *f1 = fopen("s4.txt", "r");
    FILE *f2 = fopen("s5.txt", "w");
    generate_token(f1, f2);

    fclose(f1);
    fclose(f2);
    return 0;
}
