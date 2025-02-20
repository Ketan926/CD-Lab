#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
#define MAX_SYMBOLS   50

// -------------------------------------------------
// Token structure definition
// -------------------------------------------------
typedef struct {
    char tokenName[MAX_TOKEN_LEN]; // The lexeme text
    char type[20];                 // Token type: KEYWORD, IDENTIFIER, NUMERIC, STRING_LITERAL, SPECIAL, or EOF
    unsigned int row, col;         // Starting row and column in the source file
} Token;

// -------------------------------------------------
// Symbol table entry for function names
// -------------------------------------------------
typedef struct {
    char funcName[50];       // Function name (identifier)
    char returnType[20];     // Dummy return type (here always "unknown")
} Symbol;

// Global symbol table array
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
// Reads the next character from the source file and updates row and column counters.
// -------------------------------------------------
void getNextChar(void) {
    currentChar = fgetc(source_fp);
    if(currentChar == '\n'){
        row++;
        col = 0;
    } else {
        col++;
    }
}

// -------------------------------------------------
// skipWhitespaceAndComments()
// Skips whitespace, preprocessor directives (lines beginning with '#'),
// and both single-line (// ...) and multi-line (/* ... */) comments.
// -------------------------------------------------
void skipWhitespaceAndComments(void) {
    while(currentChar != EOF) {
        // Skip whitespace characters.
        while(currentChar != EOF && isspace(currentChar))
            getNextChar();
        
        // If at beginning of line and currentChar is '#' then skip the entire line (preprocessor directive)
        if(col == 1 && currentChar == '#') {
            while(currentChar != EOF && currentChar != '\n')
                getNextChar();
            continue;
        }
        
        // Check for comments starting with '/'
        if(currentChar == '/') {
            int next = fgetc(source_fp);
            if(next == '/') {  // Single-line comment
                while(currentChar != EOF && currentChar != '\n')
                    getNextChar();
                continue;
            } else if(next == '*') {  // Multi-line comment
                getNextChar();  // Move past the '*' character
                while(currentChar != EOF) {
                    if(currentChar == '*'){
                        int temp = fgetc(source_fp);
                        if(temp == '/') {
                            getNextChar();  // Skip '/'
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

// -------------------------------------------------
// isCPPKeyword()
// Returns 1 if s is one of our predefined C++ keywords; else returns 0.
// (This small list can be expanded as needed.)
//
// For our purposes, we also use this to distinguish "type keywords" (for function definitions)
// from other keywords. Here we consider these as type keywords:
// "int", "void", "char", "bool", "float", "double".
//
// Other keywords (like "if", "else", etc.) are also handled.
 // -------------------------------------------------
int isCPPKeyword(const char *s) {
    const char *keywords[] = {
        "int", "void", "char", "bool", "float", "double",
        "if", "else", "for", "while", "return", "class", "using", "namespace"
    };
    int numKeywords = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < numKeywords; i++) {
        if (strcmp(s, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// -------------------------------------------------
// isTypeKeyword()
// Returns 1 if s is one of the type keywords we use for function definition expectations.
 // -------------------------------------------------
int isTypeKeyword(const char *s) {
    const char *typeKeywords[] = {
        "int", "void", "char", "bool", "float", "double"
    };
    int numTypes = sizeof(typeKeywords) / sizeof(typeKeywords[0]);
    for (int i = 0; i < numTypes; i++) {
        if (strcmp(s, typeKeywords[i]) == 0)
            return 1;
    }
    return 0;
}

// -------------------------------------------------
// getNextToken()
// A simplified lexer that recognizes tokens in C++ code:
// • String literals (delimited by double quotes)
// • Numeric constants (sequence of digits)
// • Identifiers and keywords (starting with a letter or underscore)
// • Any other single character becomes a SPECIAL token.
// -------------------------------------------------
Token getNextToken(void) {
    Token tok;
    int idx = 0;
    tok.tokenName[0] = '\0';
    tok.type[0] = '\0';
    tok.row = row;
    tok.col = col;
    
    skipWhitespaceAndComments();
    
    if(currentChar == EOF){
        strcpy(tok.tokenName, "EOF");
        strcpy(tok.type, "EOF");
        return tok;
    }
    
    // Record starting position of token.
    tok.row = row;
    tok.col = col;
    
    // Check for string literal.
    if(currentChar == '"'){
        tok.tokenName[idx++] = currentChar;
        getNextChar();
        while(currentChar != '"' && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        if(currentChar == '"'){
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "STRING_LITERAL");
        return tok;
    }
    
    // Check for numeric constant.
    if(isdigit(currentChar)) {
        while(isdigit(currentChar) && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "NUMERIC");
        return tok;
    }
    
    // Check for identifier or keyword.
    // In C++ an identifier may start with a letter or underscore.
    if(isalpha(currentChar) || currentChar == '_'){
        while((isalnum(currentChar) || currentChar == '_') && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        if(isCPPKeyword(tok.tokenName))
            strcpy(tok.type, "KEYWORD");
        else
            strcpy(tok.type, "IDENTIFIER");
        return tok;
    }
    
    // For any other single character, produce a SPECIAL token.
    tok.tokenName[idx++] = currentChar;
    tok.tokenName[idx] = '\0';
    strcpy(tok.type, "SPECIAL");
    getNextChar();
    return tok;
}

// -------------------------------------------------
// addFunctionSymbol()
// Adds a function symbol (name) to the symbol table if there is space.
// The return type is set to "unknown" (as a dummy value).
// -------------------------------------------------
void addFunctionSymbol(const char *funcName, const char *retType) {
    if(symbolCount >= MAX_SYMBOLS)
        return;
    strcpy(symbolTable[symbolCount].funcName, funcName);
    strcpy(symbolTable[symbolCount].returnType, retType);
    symbolCount++;
}

// -------------------------------------------------
// Main function
// -------------------------------------------------
int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Usage: %s <cpp_source_file>\n", argv[0]);
        return 1;
    }
    
    source_fp = fopen(argv[1], "r");
    if(source_fp == NULL) {
        printf("Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    // Initialize currentChar.
    getNextChar();
    
    printf("Scanning C++ tokens and building symbol table for function names...\n\n");
    
    Token currentToken;
    
    /*  
       We'll use a simple state machine to catch a function definition pattern:
         state 0: waiting for a type keyword.
         state 1: after a type keyword, expecting an identifier (candidate function name).
         state 2: after candidate identifier, expecting a "(" token.
         If the pattern is matched, add the candidate to the symbol table.
    */
    int state = 0;
    char candidate[MAX_TOKEN_LEN] = "";
    
    while(1) {
        currentToken = getNextToken();
        if(strcmp(currentToken.type, "EOF") == 0)
            break;
        
        // Print the token.
        printf("<%s, %s, row: %d, col: %d>\n",
               currentToken.tokenName, currentToken.type,
               currentToken.row, currentToken.col);
        
        // State machine for function definition detection.
        if(state == 0) {
            // If we see a type keyword, we expect the next identifier to possibly be a function name.
            if(currentToken.type != NULL && strcmp(currentToken.type, "KEYWORD") == 0 &&
               isTypeKeyword(currentToken.tokenName)) {
                state = 1;
                continue;
            }
        }
        else if(state == 1) {
            // Expect an identifier (candidate function name).
            if(strcmp(currentToken.type, "IDENTIFIER") == 0) {
                strcpy(candidate, currentToken.tokenName);
                state = 2;
                continue;
            } else {
                state = 0; // Pattern broken.
            }
        }
        else if(state == 2) {
            // Expect a left parenthesis "(".
            if(strcmp(currentToken.type, "SPECIAL") == 0 &&
               strcmp(currentToken.tokenName, "(") == 0) {
                // Add candidate function name to symbol table.
                addFunctionSymbol(candidate, "unknown");
                state = 0;
                continue;
            } else {
                state = 0; // Pattern broken.
            }
        }
    }
    
    fclose(source_fp);
    
    // Display the symbol table.
    printf("\nSymbol Table for Function Names:\n");
    printf("SlNo\tFunction Name\tReturn Type\n");
    for (int i = 0; i < symbolCount; i++){
        printf("%d\t%s\t\t%s\n", i + 1,
               symbolTable[i].funcName,
               symbolTable[i].returnType);
    }
    
    return 0;
}
