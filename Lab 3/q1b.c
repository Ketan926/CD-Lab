#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct token
{
    char token_name[50];
    int row;
    int col;
    char type[20];
    int index;
};
const char *keywords[] = {"int", "float", "double", "char", "if", "else", "for", "return",
                          "while", "return", "void", "switch", "case", "break", "continue", "default",
                          "struct", "union", "enum", "long", "short", "const", "sizeof"};
#define NUM_KEYWORDS 23

int iskeyword(const char *str)
{
    for (int i = 0; i < NUM_KEYWORDS; i++)
    {
        if (strcmp(str, keywords[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}
void remove_white_spaces(FILE *fa, FILE *fb)
{
    int ca;
    ca = getc(fa);
    while (ca != EOF)
    {
        if (ca == ' ')
        {
            while (ca == ' ')
            {
                ca = getc(fa);
            }
            putc(' ', fb);
        }
        else if (ca == '\t')
        {
            while (ca == '\t')
            {
                ca = getc(fa);
            }
            putc(' ', fb);
        }
        else
        {
            putc(ca, fb);
            ca = getc(fa);
        }
    }
    fclose(fa);
    fclose(fb);
}
void remove_headers(FILE *fa, FILE *fb)
{
    char line[1024];
    while (fgets(line, sizeof(line), fa))
    {
        if (line[0] != '#')
            fputs(line, fb);
    }
    fclose(fa);
    fclose(fb);
}
void identify_operators(FILE *fa, FILE *fb)
{
    int ca, next;
    int row = 1, col = 1;
    struct token current;
    int index = 0;
    ca = fgetc(fa);

    while (ca != EOF)
    {
        if (isspace(ca))
        {
            if (ca == '\n')
            {
                row++;
                col = 1;
            }
            else
            {
                col++;
            }
        }
        else if (ca == '&' || ca == '|' || ca == '!')
        {
            next = fgetc(fa);
            col++;
            if (ca == '&' && next == '&')
            {
                current.row = row;
                current.col = col;
                current.index = index;
                current.token_name[0] = '&';
                current.token_name[1] = '&';
                current.token_name[2] = '\0';
                strcpy(current.type, "Logical");
                fprintf(fb, "<'%s', %d, %d, '%s'>\n", current.token_name, current.row, current.col, current.type);
                col++;
                index++;
            }
            else if (ca == '|' && next == '|')
            {
                current.row = row;
                current.col = col;
                current.index = index;
                current.token_name[0] = '|';
                current.token_name[1] = '|';
                current.token_name[2] = '\0';
                strcpy(current.type, "Logical");
                fprintf(fb, "<'%s', %d, %d, '%s'>\n", current.token_name, current.row, current.col, current.type);
                col++;
                index++;
            }
            else
            {
                ungetc(next, fa);
                col--;
            }
        }
        else if (isdigit(ca))
        {
            int i = 0;
            while (isdigit(ca) || ca == '.')
            {
                current.token_name[i++] = ca;
                ca = fgetc(fa);
                col++;
            }
            current.token_name[i] = '\0';
            strcpy(current.type, "Numeric");
            fprintf(fb, "<'%s', %d, %d, '%s'>\n", current.token_name, current.row, current.col, current.type);
            index++;
        }
        else if (isalpha(ca) || ca == '_')
        {
            int i = 0;
            current.row = row; // Assign row before processing
            current.col = col; // Assign column before processing
            while (isalnum(ca) || ca == '_')
            {
                current.token_name[i++] = ca;
                ca = fgetc(fa);
                col++;
            }
            current.token_name[i] = '\0';
            if (iskeyword(current.token_name))
            {
                strcpy(current.type, "Keyword");
            }
            else
            {
                strcpy(current.type, "Identifier");
            }
            fprintf(fb, "<'%s', %d, %d, '%s'>\n", current.token_name, current.row, current.col, current.type);
            index++;
            ungetc(ca, fa); // Put back the last read character
        }

        else if (ca == '"')
        {
            char quote = ca;
            int i = 0;
            current.token_name[i++] = quote;
            ca = fgetc(fa);
            col++;
            while (ca != quote && ca != EOF)
            {
                current.token_name[i++] = ca;
                ca = fgetc(fa);
                col++;
            }
            if (ca == quote)
            {
                current.token_name[i++] = quote;
                current.token_name[i] = '\0';
                strcpy(current.type, "String Literal");
                fprintf(fb, "<'%s', %d, %d, '%s'>\n", current.token_name, current.row, current.col, current.type);
                index++;
                col++;
            }
        }
        else
        {
            current.row = row;
            current.col = col;
            current.index = index;
            current.token_name[0] = ca;
            current.token_name[1] = '\0';
            strcpy(current.type, "Special Symbol");
            fprintf(fb, "<'%s', %d, %d, '%s'>\n", current.token_name, current.row, current.col, current.type);
            index++;
            col++;
        }
        ca = fgetc(fa);
    }
}

int main()
{
    FILE *fa, *fb, *fc, *fd, *fe, *ff;
    fa = fopen("digit.c", "r");
    fb = fopen("digit1.c", "w");
    remove_headers(fa, fb);
    fc = fopen("digit1.c", "r");
    fd = fopen("digit2.c", "w");
    remove_white_spaces(fc, fd);
    fe = fopen("digit2.c", "r");
    ff = fopen("digitout2.txt", "w");
    identify_operators(fe, ff);
    fclose(fe);
    fclose(ff);
}
