#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
#define MAX_SYMBOLS   50

// ---------------------------------
// Token structure for JavaScript/jQuery tokens
// ---------------------------------
typedef struct {
    char tokenName[MAX_TOKEN_LEN]; // The lexeme text
    char type[20];                 // Token type: KEYWORD, IDENTIFIER, STRING_LITERAL, NUMERIC, SPECIAL, or EOF
    unsigned int row, col;         // Starting row and column in the source file
} Token;

// ---------------------------------
// Symbol table entry for function names
// ---------------------------------
typedef struct {
    char funcName[50];   // Function name
    char returnType[20]; // Dummy return type (for our demo, "unknown" or "jQuery")
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
    if(currentChar == '\n'){
        row++;
        col = 0;
    } else {
        col++;
    }
}

// ---------------------------------
// skipWhitespaceAndComments()
// Skips whitespace, a shebang line (if present) and both single-line (//)
// and multi-line (/* ... */) comments.
// ---------------------------------
void skipWhitespaceAndComments(void) {
    while (currentChar != EOF) {
        // Skip whitespace
        while (currentChar != EOF && isspace(currentChar))
            getNextChar();
        // Skip shebang line if at the start of file
        if (col == 1 && currentChar == '#') {
            while (currentChar != EOF && currentChar != '\n')
                getNextChar();
            continue;
        }
        // Handle comments starting with '/'
        if (currentChar == '/') {
            int next = fgetc(source_fp);
            if (next == '/') { // Single-line comment
                while (currentChar != EOF && currentChar != '\n')
                    getNextChar();
                continue;
            } else if (next == '*') { // Multi-line comment
                getNextChar();  // skip character after '*'
                while (currentChar != EOF) {
                    if (currentChar == '*') {
                        int temp = fgetc(source_fp);
                        if (temp == '/') {
                            getNextChar(); // move past '/'
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
        break; // No more whitespace/comments to skip
    }
}

// ---------------------------------
// isJSKeyword()
// Returns 1 if s is a JavaScript keyword from a small list; otherwise, returns 0.
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
// A simplified lexer for JavaScript/jQuery code.
// It recognizes string literals (both single and double quoted),
// numeric constants, identifiers (including ones beginning with '$'),
// and single-character special tokens.
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
    
    // Record token start position.
    tok.row = row;
    tok.col = col;
    
    // String literal: supports both double and single quotes.
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
    
    // Identifier (or keyword). For JavaScript (and jQuery), identifiers may start with
    // a letter, '_' or '$'.
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
// When a function declaration is detected (either by the keyword "function" or by a jQuery call),
// the next identifier token is assumed to be the function name and is added to the symbol table.
// For jQuery calls, we mark the return type as "jQuery".
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
    source_fp = fopen("q2jq.txt", "r");
    if (source_fp == NULL) {
        printf("Cannot open file %s\n", argv[1]);
        return 1;
    }
    getNextChar();  // Initialize currentChar
    
    printf("Scanning JavaScript/jQuery tokens and building symbol table for function names...\n\n");
    
    Token currentToken;
    int expectFuncName = 0;    // flag: when the token "function" is encountered, next identifier is a function name.
    int expectJQueryCall = 0;  // flag: when an identifier "$" or "jQuery" is seen, next token "(" indicates a call.
    char tempJQuery[MAX_TOKEN_LEN] = "";
    
    while (1) {
        currentToken = getNextToken();
        if (strcmp(currentToken.type, "EOF") == 0)
            break;
        printf("<%s, %s, row: %d, col: %d>\n",
               currentToken.tokenName, currentToken.type,
               currentToken.row, currentToken.col);
        
        // Check for standard function declaration via "function" keyword.
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
        
        // Check for jQuery function call.
        // If token is "$" or "jQuery", set flag.
        if ((strcmp(currentToken.tokenName, "$") == 0 ||
             strcmp(currentToken.tokenName, "jQuery") == 0) &&
            strcmp(currentToken.type, "IDENTIFIER") == 0) {
            strcpy(tempJQuery, currentToken.tokenName);
            expectJQueryCall = 1;
            continue;
        }
        if (expectJQueryCall) {
            // If the next token is a left parenthesis, then we treat it as a jQuery call.
            if (strcmp(currentToken.tokenName, "(") == 0 &&
                strcmp(currentToken.type, "SPECIAL") == 0) {
                addFunctionSymbol(tempJQuery, "jQuery");
            }
            expectJQueryCall = 0;
        }
    }
    
    fclose(source_fp);
    
    // Display the symbol table for function names
    printf("\nSymbol Table for Function Names:\n");
    printf("SlNo\tFunction Name\tReturn Type\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%d\t%s\t\t%s\n",
               i + 1, symbolTable[i].funcName, symbolTable[i].returnType);
    }
    
    return 0;
}
