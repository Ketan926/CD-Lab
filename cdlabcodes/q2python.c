#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
#define MAX_SYMBOLS   50

// ---------------------------------
// Token structure for Python tokens
// ---------------------------------
typedef struct {
    char tokenName[MAX_TOKEN_LEN]; // Lexeme text
    char type[20];                 // Token type: KEYWORD, IDENTIFIER, STRING_LITERAL, NUMERIC, SPECIAL, or EOF
    unsigned int row, col;         // Location info (starting row and col)
} Token;

// ---------------------------------
// Symbol table entry for Python functions
// ---------------------------------
typedef struct {
    char funcName[50];        // Function name
    char returnType[20];      // Dummy return type (here: "unknown")
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
// getNextChar()
// Reads the next character from the source file and updates row and col counters.
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
// Skips whitespace and comments. Comments in Python begin with '#' (or the shebang "#!" on the first line).
// ---------------------------------
void skipWhitespaceAndComments(void) {
    while (currentChar != EOF) {
        // Skip any whitespace.
        while (currentChar != EOF && isspace(currentChar))
            getNextChar();
        // If at beginning of a line and we see a '#' (possibly a shebang or comment), skip the entire line.
        if (currentChar == '#') {
            while (currentChar != EOF && currentChar != '\n')
                getNextChar();
            continue;  // After newline, loop again.
        }
        break;
    }
}

// ---------------------------------
// isPythonKeyword()
// Returns 1 if the string is one of the Python keywords we care about; otherwise returns 0.
// ---------------------------------
int isPythonKeyword(const char *s) {
    const char *keywords[] = {
        "def", "if", "else", "elif", "while", "for", "return",
        "import", "from", "as", "print", "class"
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
// A simplified lexer for Python code; it returns tokens for string literals,
// identifiers/keywords, numeric constants, and any single-character special tokens.
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
    
    // Record starting position for this token.
    tok.row = row;
    tok.col = col;
    
    // String literal (support both double and single quotes)
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
    
    // Identifier or a keyword returns when starting with a letter or underscore.
    if (isalpha(currentChar) || currentChar == '_') {
        while ((isalnum(currentChar) || currentChar == '_') && currentChar != EOF) {
            tok.tokenName[idx++] = currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        if (isPythonKeyword(tok.tokenName))
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
// When a function declaration is detected (via keyword "def"), the next identifier is assumed to be the function name.
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
int main() {
    source_fp = fopen("q2python.cpp", "r");
    if (source_fp == NULL) {
        printf("Cannot open file");
        return 1;
    }
    getNextChar();  // initialize currentChar
    
    printf("Scanning Python tokens and building symbol table for function names...\n\n");
    
    Token currentToken;
    int expectFuncName = 0;  // Flag: when "def" is encountered, expect a function name next.
    
    while (1) {
        currentToken = getNextToken();
        if (strcmp(currentToken.type, "EOF") == 0)
            break;
        printf("<%s, %s, row: %d, col: %d>\n",
               currentToken.tokenName, currentToken.type,
               currentToken.row, currentToken.col);
        
        // When the token "def" is encountered, set our flag.
        if (strcmp(currentToken.tokenName, "def") == 0 &&
            strcmp(currentToken.type, "KEYWORD") == 0) {
            expectFuncName = 1;
            continue;
        }
        // If flag is set and we get an identifier, assume it is a function name.
        if (expectFuncName && strcmp(currentToken.type, "IDENTIFIER") == 0) {
            addFunctionSymbol(currentToken.tokenName, "unknown");
            expectFuncName = 0;
        }
    }
    
    fclose(source_fp);
    
    // Display the constructed function symbol table.
    printf("\nSymbol Table for Python Function Names:\n");
    printf("SlNo\tFunction Name\tReturn Type\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%d\t%s\t\t%s\n",
               i + 1, symbolTable[i].funcName, symbolTable[i].returnType);
    }
    
    return 0;
}
