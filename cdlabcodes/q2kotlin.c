#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
#define MAX_SYMBOLS   50

// ---------------------------------
// Token structure for Kotlin tokens
// ---------------------------------
typedef struct {
    char tokenName[MAX_TOKEN_LEN]; // Lexeme text
    char type[20];                 // Token type: KEYWORD, IDENTIFIER, STRING_LITERAL, NUMERIC, SPECIAL, or EOF
    unsigned int row, col;         // Starting row and column in the source file
} Token;

// ---------------------------------
// Symbol table entry for function names
// ---------------------------------
typedef struct {
    char funcName[50];       // Function name
    char returnType[20];     // Dummy return type (here "unknown")
} Symbol;

// Global symbol table (for function names)
Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

// ---------------------------------
// Global file pointer and location info
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
int isKotlinKeyword(const char *s);
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
// Skips whitespace, single-line comments (// ...), and block comments (/* ... */).
// ---------------------------------
void skipWhitespaceAndComments(void) {
    while(currentChar != EOF) {
        // Skip whitespace
        while(currentChar != EOF && isspace(currentChar))
            getNextChar();
        // Check for a single-line comment.
        if(currentChar == '/') {
            int next = fgetc(source_fp);
            if(next == '/') {  // Single-line comment
                while(currentChar != EOF && currentChar != '\n')
                    getNextChar();
                continue;
            } else if(next == '*') {  // Block comment
                getNextChar();  // Advance past '*'
                while(currentChar != EOF) {
                    if(currentChar == '*'){
                        int temp = fgetc(source_fp);
                        if(temp == '/') {
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
        break;
    }
}

// ---------------------------------
// isKotlinKeyword()
// Returns 1 if the given string is one of our predefined Kotlin keywords.
// ---------------------------------
int isKotlinKeyword(const char *s) {
    const char *keywords[] = {
        "fun", "val", "var", "if", "else", "for", "while", "when", "return", "class", "object"
    };
    int numKeywords = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < numKeywords; i++) {
        if (strcmp(s, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// ---------------------------------
// getNextToken()
// A simplified lexer for Kotlin code. It recognizes string literals (delimited by double quotes),
// numeric constants, identifiers (or keywords), and any other single character as a SPECIAL token.
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
    
    // Record token start position.
    tok.row = row;
    tok.col = col;
    
    // String literal: only double quotes for simplicity.
    if(currentChar == '"') {
        tok.tokenName[idx++] = currentChar;
        getNextChar();
        while(currentChar != '"' && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        if(currentChar == '"') {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "STRING_LITERAL");
        return tok;
    }
    
    // Numeric constant
    if(isdigit(currentChar)) {
        while(isdigit(currentChar) && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "NUMERIC");
        return tok;
    }
    
    // Identifier or keyword: identifiers can start with a letter or underscore.
    if(isalpha(currentChar) || currentChar == '_') {
        while((isalnum(currentChar) || currentChar == '_') && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        if(isKotlinKeyword(tok.tokenName))
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

// ---------------------------------
// addFunctionSymbol()
// When a function declaration is detected (the keyword "fun" is encountered),
// the next identifier token is assumed to be the function's name and is added 
// to the symbol table with a dummy return type ("unknown").
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
    if(argc < 2) {
        printf("Usage: %s <kotlin_source_file>\n", argv[0]);
        return 1;
    }
    
    source_fp = fopen(argv[1], "r");
    if(source_fp == NULL) {
        printf("Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    getNextChar(); // Initialize currentChar
    
    printf("Scanning Kotlin tokens and building symbol table for function names...\n\n");
    
    Token currentToken;
    int expectFuncName = 0;  // Flag: when "fun" keyword is seen, next identifier is function name.
    
    while(1) {
        currentToken = getNextToken();
        if(strcmp(currentToken.type, "EOF") == 0)
            break;
        printf("<%s, %s, row: %d, col: %d>\n", currentToken.tokenName,
               currentToken.type, currentToken.row, currentToken.col);
        
        // When the keyword "fun" is seen, set the flag.
        if(strcmp(currentToken.tokenName, "fun") == 0 &&
           strcmp(currentToken.type, "KEYWORD") == 0) {
            expectFuncName = 1;
            continue;
        }
        if(expectFuncName && strcmp(currentToken.type, "IDENTIFIER") == 0) {
            // Add the function name to the symbol table.
            addFunctionSymbol(currentToken.tokenName, "unknown");
            expectFuncName = 0;
        }
    }
    
    fclose(source_fp);
    
    // Display the function symbol table.
    printf("\nSymbol Table for Function Names:\n");
    printf("SlNo\tFunction Name\tReturn Type\n");
    for(int i = 0; i < symbolCount; i++){
        printf("%d\t%s\t\t%s\n", i+1, symbolTable[i].funcName, symbolTable[i].returnType);
    }
    
    return 0;
}
