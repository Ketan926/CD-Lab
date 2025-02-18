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

struct symbol {
    int sno;
    char lexeme[100];
    char datatype[20];
    int size;
};

struct token table[100];
struct symbol symTable[100];
int symIndex = 0;

const char *keywords[] = {
    "int", "float", "double", "char", "if", "else", "for", "return",
    "while", "void", "switch", "case", "break", "continue",
    "default", "struct", "union", "enum", "long", "short", "const",
    "sizeof", "printf", "scanf"
};

#define NUM_KEYWORDS 24

int isKeyword(char *word) {
    for (int i = 0; i < NUM_KEYWORDS; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void addToSymbolTable(char *lexeme, char *datatype, int size) {
    for (int i = 0; i < symIndex; i++) {
        if (strcmp(symTable[i].lexeme, lexeme) == 0) {
            return; // Avoid duplicates
        }
    }
    symTable[symIndex].sno = symIndex + 1;
    strcpy(symTable[symIndex].lexeme, lexeme);
    strcpy(symTable[symIndex].datatype, datatype);
    symTable[symIndex].size = size;
    symIndex++;
}

void generate_token(FILE *f1, FILE *f2) {
    char c, next;
    int row = 1, col = 1, i = 0;
    int cur_row, cur_col;
    char word[50];
    char datatype[20] = "";
    int ind = 0;
    int isFunction = 0;
    int arraySize = 1;

    if (!f1 || !f2) {
        printf("Error opening files!\n");
        return;
    }

    while ((c = getc(f1)) != EOF) {
        if (isspace(c)) {
            if (c == '\n') {
                row++;
                col = 1;
            } else {
                col++;
            }
            continue;
        }

        cur_row = row;
        cur_col = col;

        if (isalpha(c) || c == '_') {
            i = 0;
            word[i++] = c;
            while (isalnum(c = getc(f1)) || c == '_') {
                word[i++] = c;
            }
            word[i] = '\0';
            ungetc(c, f1);
            
            next = getc(f1);
            if (next == '(') {
                isFunction = 1;
            }
            ungetc(next, f1);

            table[ind].row = cur_row;
            table[ind].col = cur_col;
            strcpy(table[ind].token, word);
            table[ind].index = ind + 1;
            
            if (isKeyword(word)) {
                strcpy(table[ind].type, "Keyword");
                strcpy(datatype, word);
            } else if (isFunction) {
                strcpy(table[ind].type, "Function");
                addToSymbolTable(word, "Function", -1); // Insert function in symbol table
                isFunction = 0;
            } else {
                strcpy(table[ind].type, "Identifier");
                arraySize = 1;
                
                c = getc(f1);
                if (c == '[') {
                    fscanf(f1, "%d]", &arraySize);
                } else {
                    ungetc(c, f1);
                }

                if (datatype[0] != '\0') {
                    addToSymbolTable(word, datatype, arraySize);
                }
            }
            
            fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col += i;
        }

        if (c == '"') {
            do {
                c = getc(f1);
            } while (c != '"' && c != EOF);
        }
    }
}

int main() {
    FILE *f1 = fopen("s4.txt", "r");
    FILE *f2 = fopen("s6.txt", "w");
    generate_token(f1, f2);

    printf("Symbol Table:\n");
    printf("S.No\tLexeme\tDataType\tSize\n");
    for (int i = 0; i < symIndex; i++) {
        printf("%d\t%s\t%s\t%d\n", symTable[i].sno, symTable[i].lexeme, symTable[i].datatype, symTable[i].size);
    }

    fclose(f1);
    fclose(f2);
    return 0;
}
