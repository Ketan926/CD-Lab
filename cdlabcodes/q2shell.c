#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
#define MAX_SYMBOLS   50

// -------------------------------------------------
// Token structure for shell script tokens
// -------------------------------------------------
typedef struct {
    char tokenName[MAX_TOKEN_LEN]; // The lexeme text
    char type[20];                 // Token type: KEYWORD, IDENTIFIER, STRING_LITERAL, NUMERIC, SPECIAL, or EOF
    unsigned int row, col;         // Location info (starting row and column)
} Token;

// -------------------------------------------------
// Symbol table entry for function names
// -------------------------------------------------
typedef struct {
    char funcName[50];       // Function name (identifier)
    char returnType[20];     // Dummy return type (here: "unknown")
} Symbol;

// Global symbol table (for function names)
Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

// -------------------------------------------------
// Global file pointer and location info
// -------------------------------------------------
FILE *source_fp;
int currentChar;
unsigned int row = 1, col = 0;

// -------------------------------------------------
// getNextChar()
// Reads the next character from the source file and updates row/col counters.
// -------------------------------------------------
void getNextChar(void) {
    currentChar = fgetc(source_fp);
    if (currentChar == '\n') {
        row++;
        col = 0;
    } else {
        col++;
    }
}

// -------------------------------------------------
// skipWhitespaceAndComments()
// Skips whitespace characters. In shell scripts, a comment starts with '#' (the shebang
// line as well as other comment lines). We assume that if '#' is encountered (at the beginning
// of a line or after whitespace), the entire line is a comment and it is skipped.
// -------------------------------------------------
void skipWhitespaceAndComments(void) {
    while (currentChar != EOF) {
        // Skip whitespace
        while (currentChar != EOF && isspace(currentChar))
            getNextChar();
        // If at beginning of a line (col == 1) and currentChar is '#' then skip the whole line.
        if (col == 1 && currentChar == '#') {
            while (currentChar != EOF && currentChar != '\n')
                getNextChar();
            continue; // After skipping the line, check again.
        }
        break;
    }
}

// -------------------------------------------------
// getNextToken()
// A simplified lexer for shell script source. It recognizes:
// • String literals (delimited by single or double quotes),
// • Numeric constants,
// • Identifiers (starting with a letter or underscore),
// • Otherwise a single character is produced as a SPECIAL token.
// -------------------------------------------------
Token getNextToken(void) {
    Token tok;
    int idx = 0;
    tok.tokenName[0] = '\0';
    tok.type[0] = '\0';
    tok.row = row;
    tok.col = col;

    skipWhitespaceAndComments();

    if (currentChar == EOF) {
        strcpy(tok.tokenName, "EOF");
        strcpy(tok.type, "EOF");
        return tok;
    }

    // Record starting position of this token.
    tok.row = row;
    tok.col = col;

    // String literal: support both single and double quotes.
    if (currentChar == '"' || currentChar == '\'') {
        char quote = currentChar;
        tok.tokenName[idx++] = currentChar;
        getNextChar();
        while (currentChar != quote && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        if (currentChar == quote) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "STRING_LITERAL");
        return tok;
    }

    // Numeric constant.
    if (isdigit(currentChar)) {
        while (isdigit(currentChar) && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "NUMERIC");
        return tok;
    }

    // Identifier or keyword: in shell scripts identifiers typically start with a letter or underscore.
    if (isalpha(currentChar) || currentChar == '_') {
        while ((isalnum(currentChar) || currentChar == '_') && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        // Our small keyword list: we include "function" as the only keyword for function definitions.
        if (strcmp(tok.tokenName, "function") == 0)
            strcpy(tok.type, "KEYWORD");
        else
            strcpy(tok.type, "IDENTIFIER");
        return tok;
    }

    // Otherwise, treat a single character as a SPECIAL token.
    tok.tokenName[idx++] = currentChar;
    tok.tokenName[idx] = '\0';
    strcpy(tok.type, "SPECIAL");
    getNextChar();
    return tok;
}

// -------------------------------------------------
// addFunctionSymbol()
// Adds a function name to the symbol table with a dummy return type ("unknown").
// -------------------------------------------------
void addFunctionSymbol(const char *funcName, const char *retType) {
    if (symbolCount >= MAX_SYMBOLS)
        return;
    strcpy(symbolTable[symbolCount].funcName, funcName);
    strcpy(symbolTable[symbolCount].returnType, retType);
    symbolCount++;
}

// -------------------------------------------------
// Main function
// -------------------------------------------------
int main(int argc, char *argv[]) {

    source_fp = fopen("q2shell.txt", "r");
    if (source_fp == NULL) {
        printf("Cannot open file %s\n", argv[1]);
        return 1;
    }

    getNextChar(); // Initialize currentChar

    printf("Scanning shell script tokens and building symbol table for function names...\n\n");

    Token currentToken;
    int expectFuncName = 0;  // Set to 1 when we see the "function" keyword.

    while (1) {
        currentToken = getNextToken();
        if (strcmp(currentToken.type, "EOF") == 0)
            break;

        printf("<%s, %s, row: %d, col: %d>\n",
               currentToken.tokenName, currentToken.type,
               currentToken.row, currentToken.col);

        // If we see the keyword "function", then the next identifier token is taken as a function name.
        if (strcmp(currentToken.tokenName, "function") == 0 &&
            strcmp(currentToken.type, "KEYWORD") == 0) {
            expectFuncName = 1;
            continue;
        }
        if (expectFuncName && strcmp(currentToken.type, "IDENTIFIER") == 0) {
            addFunctionSymbol(currentToken.tokenName, "unknown");
            expectFuncName = 0;
            continue;
        }
    }

    fclose(source_fp);

    // Display the symbol table.
    printf("\nSymbol Table for Function Names:\n");
    printf("SlNo\tFunction Name\tReturn Type\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%d\t%s\t\t%s\n", i + 1,
               symbolTable[i].funcName,
               symbolTable[i].returnType);
    }

    return 0;
}
