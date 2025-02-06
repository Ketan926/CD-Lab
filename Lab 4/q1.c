#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TableLength 30

enum tokenType {EOFILE=-1, KEYWORD, IDENTIFIER, NUMERIC, SPECIAL};

void remove_headers(FILE *fa, FILE *fb) {
    char line[1024];
    while (fgets(line, sizeof(line), fa)) {
        if (line[0] != '#') {
            fputs(line, fb);
        }
    }
    fclose(fa);
    fclose(fb);
}

struct token {
    char *lexeme;
    int index;
    unsigned int rowno, colno; // row number, column number.
    enum tokenType type;
    int size; // Changed to a single integer
};

struct ListElement {
    struct token tok;
    struct ListElement *next;
};

struct ListElement *TABLE[TableLength];

void Initialize() {
    for (int i = 0; i < TableLength; i++) {
        TABLE[i] = NULL;
    }
}

int HASH(char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % TableLength;
}

int SEARCH(char *str) {
    int val = HASH(str);
    struct ListElement *ele = TABLE[val];
    while (ele != NULL) {
        if (strcmp(ele->tok.lexeme, str) == 0) {
            return 1; // Found
        }
        ele = ele->next;
    }
    return 0; // Not found
}

void INSERT(struct token tk) {
    if (SEARCH(tk.lexeme) == 1) {
        free(tk.lexeme); // Free the lexeme if already exists
        return; // Already exists
    }
    int val = HASH(tk.lexeme);
    struct ListElement *cur = (struct ListElement *)malloc(sizeof(struct ListElement));
    cur->tok = tk;
    cur->next = NULL;
    if (TABLE[val] == NULL) {
        TABLE[val] = cur; // No collision
    } else {
        struct ListElement *ele = TABLE[val];
        while (ele->next != NULL) {
            ele = ele->next; // Add the element at the end in case of a collision
        }
        ele->next = cur;
    }
}

void Display(FILE *output_file) {
    fprintf(output_file, "Name\t\tType\t\tSize\n");
    for (int i = 0; i < TableLength; i++) {
        struct ListElement *ele = TABLE[i];
        while (ele != NULL) {
            fprintf(output_file, "%s\t\t%s\t%d\n", ele->tok.lexeme, 
                    (ele->tok.type == KEYWORD) ? "KEYWORD\t\t" : 
                    (ele->tok.type == IDENTIFIER) ? "IDENTIFIER\t" : 
                    (ele->tok.type == NUMERIC) ? "NUMERIC\t\t" : 
                    "SPECIAL\t\t", 
                    ele->tok.size);
            ele = ele->next;
        }
    }
}

int iskeyword(const char *str) {
    const char *keywords[] = {"int", "float", "double", "char", "if", "else", "for", "return",
                              "while", "void", "switch", "case", "break", "continue", "default",
                              "struct", "union", "enum", "long", "short", "const", "sizeof"};
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1; 
        }
    }
    return 0; 
}

struct token getNextToken(FILE *fa, int *row, int *col) {
    int ca;
    struct token current;
    current.lexeme = (char *)malloc(50 * sizeof(char)); 
    ca = fgetc(fa);
    while (ca != EOF && (isspace(ca) || ca == '\n')) {
        if (ca == '\n') {
            (*row)++;
            *col = 1;
        } else {
            (*col)++;
        }
        ca = fgetc(fa);
    }
    if (ca == EOF) {
        current.lexeme[0] = '\0';
        current.rowno = *row;
        current.colno = *col;
        current.index = -1;
        current.type = EOFILE;
        return current;
    }

    int i = 0;
    current.rowno = *row;
    current.colno = *col;

    if (isalpha(ca) || ca == '_') {
        while (isalnum(ca) || ca == '_') {
            current.lexeme[i++] = ca;
            ca = fgetc(fa);
            (*col)++;
        }
        current.lexeme[i] = '\0';
        if (iskeyword(current.lexeme)) {
            current.type = KEYWORD;
            current.size = -1; // Size not applicable for keywords
        } else {
            current.type = IDENTIFIER;
            current.size = -1; // Size not applicable for identifiers
        }
        INSERT(current);
        return current;
    }

    if (isdigit(ca)) {
        while (isdigit(ca) || ca == '.') {
            current.lexeme[i++] = ca;
            ca = fgetc(fa);
            (*col)++;
        }
        current.lexeme[i] = '\0';
        current.type = NUMERIC;
        current.size = -1; // Size not applicable for numeric
        INSERT(current);
        return current;
    }

    current.lexeme[0] = ca;
    current.lexeme[1] = '\0';
    current.type = SPECIAL;
    current.size = -1; // Size not applicable for special characters
    INSERT(current);
    (*col)++;

    return current;
}

void freeTable() {
    for (int i = 0; i < TableLength; i++) {
        struct ListElement *ele = TABLE[i];
        while (ele != NULL) {
            struct ListElement *temp = ele;
            ele = ele->next;
            free(temp->tok.lexeme); // Free lexeme
            free(temp); // Free list element
        }
    }
}

int main() {
    FILE *fa, *fb, *fc, *ff;
    fa = fopen("digit.c", "r");
    if (fa == NULL) {
        printf("Error opening input file digit.c\n");
        return 1;
    }
    fb = fopen("digit1.c", "w");
    remove_headers(fa, fb);
    fc = fopen("digit1.c", "r");
    if (fc == NULL) {
        printf("Error opening intermediate file digit1.c\n");
        return 1;
    }
    ff = fopen("q1output.txt", "w");
    if (ff == NULL) {
        printf("Error opening output file q1output.txt\n");
        return 1;
    }

    Initialize(); 
    int row = 1, col = 1;
    struct token t;

    while (1) {
        t = getNextToken(fc, &row, &col);
        if (t.index == -1) {
            break;
        }
    }
    fprintf(ff, "Symbol Table:\n");
    Display(ff);

    fclose(fc);
    fclose(ff);
    freeTable(); // Free memory before exiting
    return 0;
}