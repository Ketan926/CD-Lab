#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
#define MAX_SYMBOLS   50

// ---------------------------------
// Token structure for PHP tokens
// ---------------------------------
typedef struct {
    char tokenName[MAX_TOKEN_LEN]; // Lexeme (identifier, keyword, string literal, etc.)
    char type[20];                 // Token type: KEYWORD, IDENTIFIER, STRING_LITERAL, SPECIAL, or EOF
    unsigned int row, col;         // Location info in the source file
} Token;

// ---------------------------------
// Symbol table entry for function names
// ---------------------------------
typedef struct {
    char funcName[50];    // Function name
    char returnType[20];  // For demo purposes we set "unknown"
} Symbol;

// Global symbol table for function names
Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

// ---------------------------------
// Global file pointer and lexer location info
// ---------------------------------
FILE *source_fp;
int currentChar;
unsigned int row = 1, col = 0;

// ---------------------------------
// Function prototypes
// ---------------------------------
void getNextChar(void);
void skipWhitespaceAndComments(void);
Token getNextToken(void);
int isPHPKeyword(const char *s);
void addFunctionSymbol(const char *funcName, const char *retType);

// ---------------------------------
// getNextChar()
// Reads the next character from the source file and updates row/col counters.
// ---------------------------------
void getNextChar(void) {
    currentChar = fgetc(source_fp);
    if(currentChar == '\n'){
        row++;
        col = 0;
    } else {
        col++;
    }
}

// ---------------------------------
// skipWhitespaceAndComments()
// Skips whitespace and comments, and also skips PHP header/footer tags.
// In PHP, we skip the starting tag "<?php" (and similarly "?>" at the end).
// ---------------------------------
void skipWhitespaceAndComments(void) {
    while(currentChar != EOF) {
        // Skip whitespace.
        while(currentChar != EOF && isspace(currentChar))
            getNextChar();
        
        // Skip PHP starting tag <?php or echo short tag <?= if present.
        if(currentChar == '<') {
            long pos = ftell(source_fp);
            char tagBuffer[10] = {0};
            int i = 0;
            tagBuffer[i++] = (char)currentChar;
            getNextChar();
            if(currentChar == '?') {
                tagBuffer[i++] = (char)currentChar;
                getNextChar();
                // Peek letters for "php"
                while(i < 6 && currentChar != EOF && !isspace(currentChar)) {
                    tagBuffer[i++] = (char)currentChar;
                    getNextChar();
                }
                tagBuffer[i] = '\0';
                if(strncmp(tagBuffer, "<?php", 5) == 0 || strncmp(tagBuffer, "<?= ", 3) == 0 ||
                   strncmp(tagBuffer, "<?", 2) == 0) {
                    // Skip until end of line.
                    while(currentChar != EOF && currentChar != '\n')
                        getNextChar();
                    continue;
                } else {
                    // Not a header tag—rewind.
                    fseek(source_fp, pos, SEEK_SET);
                    currentChar = fgetc(source_fp);
                }
            } else {
                // Not a PHP header tag; rewind.
                fseek(source_fp, pos, SEEK_SET);
                currentChar = fgetc(source_fp);
            }
        }
        
        // Skip single-line comments starting with //
        if(currentChar == '/') {
            int next = fgetc(source_fp);
            if(next == '/') {
                while(currentChar != EOF && currentChar != '\n')
                    getNextChar();
                continue;
            } else if(next == '*') { // Skip multi-line comments: /* ... */
                getNextChar(); // skip the '*'
                while(currentChar != EOF) {
                    if(currentChar == '*') {
                        int temp = fgetc(source_fp);
                        if(temp == '/'){
                            getNextChar();
                            break;
                        } else {
                            ungetc(temp, source_fp);
                        }
                    }
                    getNextChar();
                }
                continue;
            } else {
                ungetc(next, source_fp);
            }
        }
        // Skip single-line comments starting with #
        if(currentChar == '#') {
            while(currentChar != EOF && currentChar != '\n')
                getNextChar();
            continue;
        }
        break; // Done skipping.
    }
}

// ---------------------------------
// isPHPKeyword()
// Checks if a string is a PHP keyword. You may add more keywords as needed.
// ---------------------------------
int isPHPKeyword(const char *s) {
    const char *keywords[] = {"function", "echo", "if", "else", "while", "for", "return", "class", "public", "private", "protected", "static", "var"};
    int numKeywords = sizeof(keywords)/sizeof(keywords[0]);
    for(int i = 0; i < numKeywords; i++){
        if(strcmp(s, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// ---------------------------------
// getNextToken()
// A simplified lexer for PHP code. It generates tokens for string literals,
// identifiers (or keywords), and for any single-character special tokens.
// ---------------------------------
Token getNextToken(void) {
    Token tok;
    int idx = 0;
    tok.tokenName[0] = '\0';
    tok.type[0] = '\0';
    tok.row = row;
    tok.col = col;
    
    skipWhitespaceAndComments();
    
    if(currentChar == EOF) {
        strcpy(tok.tokenName, "EOF");
        strcpy(tok.type, "EOF");
        return tok;
    }
    
    tok.row = row;
    tok.col = col;
    
    // String literal token (delimited by double quotes).
    if(currentChar == '"') {
        tok.tokenName[idx++] = (char)currentChar;
        getNextChar();
        while(currentChar != '"' && currentChar != EOF) {
            tok.tokenName[idx++] = (char)currentChar;
            getNextChar();
        }
        if(currentChar == '"'){
            tok.tokenName[idx++] = (char)currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "STRING_LITERAL");
        return tok;
    }
    
    // If starts with a letter or underscore then it is an identifier (or keyword).
    if(isalpha(currentChar) || currentChar == '_') {
        while((isalnum(currentChar) || currentChar == '_') && currentChar != EOF) {
            tok.tokenName[idx++] = (char)currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        if(isPHPKeyword(tok.tokenName))
            strcpy(tok.type, "KEYWORD");
        else
            strcpy(tok.type, "IDENTIFIER");
        return tok;
    }
    
    // For digits, we can treat them as numeric tokens (here we use IDENTIFIER type for simplicity).
    if(isdigit(currentChar)) {
        while(isdigit(currentChar) && currentChar != EOF) {
            tok.tokenName[idx++] = (char)currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "NUMERIC");
        return tok;
    }
    
    // Otherwise, treat a single character as a special token.
    tok.tokenName[idx++] = (char)currentChar;
    tok.tokenName[idx] = '\0';
    strcpy(tok.type, "SPECIAL");
    getNextChar();
    return tok;
}

// ---------------------------------
// addFunctionSymbol()
// When a function declaration is detected, add the function name to the symbol table.
// For PHP, a function declaration begins with the keyword "function"; the token
// immediately following is taken as the function name.
// ---------------------------------
void addFunctionSymbol(const char *funcName, const char *retType) {
    if(symbolCount >= MAX_SYMBOLS)
        return;
    strcpy(symbolTable[symbolCount].funcName, funcName);
    strcpy(symbolTable[symbolCount].returnType, retType);
    symbolCount++;
}

// ---------------------------------
// Main function
// ---------------------------------
int main(int argc, char *argv[]) {
    source_fp = fopen("q2php.txt", "r");
    if(source_fp == NULL) {
        printf("Cannot open file ");
        return 1;
    }
    // Initialize the first character.
    getNextChar();
    
    printf("Scanning PHP tokens and building symbol table for function names...\n\n");
    
    Token currentToken;
    int expectFunctionName = 0;  // flag: set to 1 when the last token was "function"
    while(1) {
        currentToken = getNextToken();
        if(strcmp(currentToken.type, "EOF") == 0)
            break;
        
        // Print the token.
        printf("<%s, %s, row: %d, col: %d>\n",
               currentToken.tokenName, currentToken.type,
               currentToken.row, currentToken.col);
        
        // If we encounter the keyword "function", then the next identifier is a function name.
        if(strcmp(currentToken.tokenName, "function") == 0 && strcmp(currentToken.type, "KEYWORD") == 0) {
            expectFunctionName = 1;
            continue;
        }
        if(expectFunctionName) {
            // The next non-special token (expected identifier) becomes the function name.
            if(strcmp(currentToken.type, "IDENTIFIER") == 0) {
                // For demonstration, we set return type as "unknown".
                addFunctionSymbol(currentToken.tokenName, "unknown");
            }
            expectFunctionName = 0;
        }
    }
    
    fclose(source_fp);
    
    // Display the symbol table for function names.
    printf("\nSymbol Table for Function Names:\n");
    printf("SlNo\tFunction Name\tReturn Type\n");
    for(int i = 0; i < symbolCount; i++){
        printf("%d\t%s\t\t%s\n",
               i+1, symbolTable[i].funcName, symbolTable[i].returnType);
    }
    
    return 0;
}
