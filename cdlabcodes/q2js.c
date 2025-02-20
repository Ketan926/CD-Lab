#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
#define MAX_SYMBOLS   50

// ---------------------------------
// Token structure for JavaScript tokens
// ---------------------------------
typedef struct {
    char tokenName[MAX_TOKEN_LEN]; // Lexeme text
    char type[20];                 // Token type: KEYWORD, IDENTIFIER, STRING_LITERAL, NUMERIC, SPECIAL, or EOF
    unsigned int row, col;         // Location info (starting row and col)
} Token;

// ---------------------------------
// Symbol table entry for function names
// ---------------------------------
typedef struct {
    char funcName[50];        
    char returnType[20];      // For demo, we set it to "unknown"
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
int isJSKeyword(const char *s);
void addFunctionSymbol(const char *funcName, const char *retType);

// ---------------------------------
// getNextChar()
// Reads the next character from the source file and updates row/col counters.
// ---------------------------------
void getNextChar(void) {
    currentChar = fgetc(source_fp);
    if (currentChar == '\n') {
        row++;
        col = 0;
    } else {
        col++;
    }
}

// ---------------------------------
// skipWhitespaceAndComments()
// Skips whitespace, a shebang line (if present), and comments in JavaScript.
// ---------------------------------
void skipWhitespaceAndComments(void) {
    while (currentChar != EOF) {
        // Skip any whitespace.
        while (currentChar != EOF && isspace(currentChar))
            getNextChar();
        
        // Skip a shebang line (e.g., #!/usr/bin/env node) if at start of file.
        if (col == 1 && currentChar == '#') {
            // Consume until newline.
            while (currentChar != EOF && currentChar != '\n')
                getNextChar();
            continue;
        }
        
        // If we see a '/', possibly a comment.
        if (currentChar == '/') {
            int next = fgetc(source_fp);
            if(next == '/') {  // Single-line comment
                while (currentChar != EOF && currentChar != '\n')
                    getNextChar();
                continue;
            } else if(next == '*') {  // Multi-line comment
                getNextChar();  // Skip character after '*'
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
                // Not a comment; push back the character and break.
                ungetc(next, source_fp);
                break;
            }
        }
        break;
    }
}

// ---------------------------------
// isJSKeyword()
// Checks if a given string is one of our small set of JavaScript keywords.
// ---------------------------------
int isJSKeyword(const char *s) {
    const char *keywords[] = {
        "function", "var", "let", "const", "if", "else", "for", "while", "return"
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
// A simplified lexer for JavaScript code. It recognizes string literals
// (using single or double quotes), numeric constants, identifiers (and keywords),
// and any other single-character special tokens.
// ---------------------------------
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
    
    // Record starting position.
    tok.row = row;
    tok.col = col;
    
    // String literal handling (supports both single and double quotes)
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
    
    // Numeric constant
    if (isdigit(currentChar)) {
        while (isdigit(currentChar) && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "NUMERIC");
        return tok;
    }
    
    // Identifier (or keyword). In JavaScript identifiers can start with a letter, underscore or dollar sign.
    if (isalpha(currentChar) || currentChar == '_' || currentChar == '$') {
        while ((isalnum(currentChar) || currentChar == '_' || currentChar == '$') && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        if (isJSKeyword(tok.tokenName))
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
// When a function declaration is detected (the token "function" is encountered),
// the next identifier token is assumed to be the function name and is added to the symbol table.
// ---------------------------------
void addFunctionSymbol(const char *funcName, const char *retType) {
    if (symbolCount >= MAX_SYMBOLS)
        return;
    strcpy(symbolTable[symbolCount].funcName, funcName);
    strcpy(symbolTable[symbolCount].returnType, retType);
    symbolCount++;
}

// ---------------------------------
// Main function
// ---------------------------------
int main(int argc, char *argv[]) {
    source_fp = fopen("q2js.txt", "r");
    if (source_fp == NULL) {
        printf("Cannot open file %s\n", argv[1]);
        return 1;
    }
    getNextChar();  // initialize currentChar
    
    printf("Scanning JavaScript tokens and building symbol table for function names...\n\n");
    
    Token currentToken;
    int expectFuncName = 0;  // flag: when "function" keyword is seen, the next identifier is the function name.
    
    while (1) {
        currentToken = getNextToken();
        if (strcmp(currentToken.type, "EOF") == 0)
            break;
        printf("<%s, %s, row: %d, col: %d>\n",
               currentToken.tokenName, currentToken.type,
               currentToken.row, currentToken.col);
        
        // When we encounter the keyword "function", set the flag.
        if (strcmp(currentToken.tokenName, "function") == 0 &&
            strcmp(currentToken.type, "KEYWORD") == 0) {
            expectFuncName = 1;
            continue;
        }
        // If flag is set and we receive an identifier token, add it as a function name.
        if (expectFuncName && strcmp(currentToken.type, "IDENTIFIER") == 0) {
            addFunctionSymbol(currentToken.tokenName, "unknown");
            expectFuncName = 0;
        }
    }
    
    fclose(source_fp);
    
    // Display the symbol table for function names.
    printf("\nSymbol Table for Function Names:\n");
    printf("SlNo\tFunction Name\tReturn Type\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%d\t%s\t\t%s\n",
               i + 1, symbolTable[i].funcName, symbolTable[i].returnType);
    }
    
    return 0;
}
