#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
#define MAX_SYMBOLS   50

// -----------------------------
// Token structure definition
// -----------------------------
typedef struct {
    char tokenName[MAX_TOKEN_LEN]; // The lexeme string.
    char type[20];                 // Type string: KEYWORD, IDENTIFIER, STRING_LITERAL, SPECIAL, EOF.
    unsigned int row, col;         // Location info.
} Token;

// -----------------------------
// Symbol table entry for functions (methods)
// -----------------------------
typedef struct {
    char lexeme[50];  // Function name.
    char returnType[20];   // For simplicity, we store the return type if possible; else "unknown".
} Symbol;

// Global symbol table (for function names)
Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

// -----------------------------
// Global file pointer and location info
// -----------------------------
FILE *source_fp;
int currentChar;
unsigned int row = 1, col = 0;

// -----------------------------
// Function prototypes
// -----------------------------
void getNextChar(void);
void skipWhitespaceCommentsAndHeaders(void);
Token getNextToken(void);
int isJavaKeyword(const char *s);
void addFunctionSymbol(const char *funcName, const char *retType);

// -----------------------------
// getNextChar()
// Reads the next character from the source file and updates the row/col counters.
// -----------------------------
void getNextChar(void) {
    currentChar = fgetc(source_fp);
    if(currentChar == '\n'){
        row++;
        col = 0;
    } else {
        col++;
    }
}

// -----------------------------
// skipWhitespaceCommentsAndHeaders()
// Skip normal whitespace, comments and header lines.
// For header lines, we skip lines starting with "import" or "package".
// -----------------------------
void skipWhitespaceCommentsAndHeaders(void) {
    while(currentChar != EOF){
        // Skip whitespace characters.
        while(currentChar != EOF && isspace(currentChar)) {
            getNextChar();
        }
        // For header lines: if at start of a line (col==1) and the word "import" or "package" occurs.
        if(col == 1){
            char headerBuffer[100];
            int i = 0;
            // Peek ahead without consuming character, using fpos_t.
            long pos = ftell(source_fp);
            // Read the line up to newline
            while(currentChar != EOF && currentChar != '\n' && i < 99){
                headerBuffer[i++] = currentChar;
                getNextChar();
            }
            headerBuffer[i] = '\0';
            // Check if the line starts with "import" or "package"
            if(strncmp(headerBuffer, "import", 6) == 0 || strncmp(headerBuffer, "package", 7) == 0){
                // Skip this line; continue to next.
                getNextChar();  // consume newline
                continue;
            } else {
                // Not a header line. Reset file pointer back to the start of that line.
                fseek(source_fp, pos - 1, SEEK_SET);
                currentChar = fgetc(source_fp);
                // Update the column: we assume pos was at beginning of line.
                col = 1;
            }
        }
        // Check for comments: if currentChar is '/'.
        if(currentChar == '/'){
            int nextChar = fgetc(source_fp);
            if(nextChar == '/'){
                // Single-line comment: skip until newline.
                while(currentChar != EOF && currentChar != '\n'){
                    getNextChar();
                }
                continue;
            } else if(nextChar == '*'){
                // Multi-line comment: skip until closing "*/"
                getNextChar(); // Advance from '*'
                while(currentChar != EOF) {
                    if(currentChar == '*'){
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
                // Not a comment; push back the character.
                ungetc(nextChar, source_fp);
            }
        }
        // If none of the conditions met, break out.
        break;
    }
}

// -----------------------------
// isJavaKeyword()
// A basic function to check Java keywords. You can add more keywords as needed.
// -----------------------------
int isJavaKeyword(const char *s) {
    const char *keywords[] = {"public", "private", "protected", "static", "void", "int", "boolean",
                              "if", "else", "for", "while", "class", "new", "import", "package", "return"};
    int numKeywords = sizeof(keywords)/sizeof(keywords[0]);
    for(int i = 0; i < numKeywords; i++){
        if(strcmp(s, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// -----------------------------
// getNextToken()
// A simplified lexer for Java code. It returns tokens of types: KEYWORD, IDENTIFIER,
// STRING_LITERAL, SPECIAL, or EOF. Comments and header lines are skipped beforehand.
// -----------------------------
Token getNextToken(void) {
    Token tok;
    int idx = 0;
    tok.tokenName[0] = '\0';
    tok.type[0] = '\0';
    tok.row = row;
    tok.col = col;
    
    skipWhitespaceCommentsAndHeaders();
    
    if(currentChar == EOF) {
        strcpy(tok.tokenName, "EOF");
        strcpy(tok.type, "EOF");
        return tok;
    }
    
    // Record start position.
    tok.row = row;
    tok.col = col;
    
    // If the token is a string literal.
    if(currentChar == '"') {
        tok.tokenName[idx++] = (char)currentChar;
        getNextChar();
        while(currentChar != '"' && currentChar != EOF) {
            tok.tokenName[idx++] = (char)currentChar;
            getNextChar();
        }
        if(currentChar == '"') {
            tok.tokenName[idx++] = (char)currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "STRING_LITERAL");
        return tok;
    }
    
    // If token starts with a letter or underscore, then it is an identifier (or keyword).
    if(isalpha(currentChar) || currentChar == '_') {
        while((isalnum(currentChar) || currentChar == '_') && currentChar != EOF) {
            tok.tokenName[idx++] = (char)currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        if(isJavaKeyword(tok.tokenName))
            strcpy(tok.type, "KEYWORD");
        else
            strcpy(tok.type, "IDENTIFIER");
        return tok;
    }
    
    // If token starts with a digit, treat as numeric constant (here we simply treat it as identifier type).
    if(isdigit(currentChar)){
        while(isdigit(currentChar) && currentChar != EOF) {
            tok.tokenName[idx++] = (char)currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "NUMERIC");
        return tok;
    }
    
    // If none of above, then take as a single-character special token.
    tok.tokenName[idx++] = (char)currentChar;
    tok.tokenName[idx] = '\0';
    strcpy(tok.type, "SPECIAL");
    getNextChar();
    return tok;
}

// -----------------------------
// addFunctionSymbol()
// Insert a function (method) symbol into the symbol table. In a more advanced analyzer,
// one would extract type information and parameters.
// -----------------------------
void addFunctionSymbol(const char *funcName, const char *retType) {
    if(symbolCount >= MAX_SYMBOLS)
        return;
    // In our simple demo, we add the function name.
    strcpy(symbolTable[symbolCount].lexeme, funcName);
    strcpy(symbolTable[symbolCount].returnType, retType);
    symbolCount++;
}

// -----------------------------
// Main function
// -----------------------------
int main(int argc, char *argv[]) {
    source_fp = fopen("q2java.txt","r");
    if(source_fp == NULL){
        printf("Cannot open file %s\n", argv[1]);
        return 1;
    }
    // Initialize first character.
    getNextChar();
    
    printf("Scanning tokens (for Java) and building symbol table for function names...\n\n");
    
    Token currentToken, nextToken;
    char prevToken[MAX_TOKEN_LEN] = "";
    int prevWasIdentifier = 0;
    
    // We scan until we get an EOF token.
    while(1){
        currentToken = getNextToken();
        if(strcmp(currentToken.type, "EOF") == 0)
            break;
        printf("<%s, %s, row: %d, col: %d>\n", currentToken.tokenName,
               currentToken.type, currentToken.row, currentToken.col);
        
        // For function detection:
        // A simple approach is: if we see an identifier token followed immediately by a "(" SPECIAL token,
        // then treat that identifier as a function name.
        if(strcmp(currentToken.type, "IDENTIFIER") == 0){
            // Save the current identifier in case the next token is "(".
            strcpy(prevToken, currentToken.tokenName);
            prevWasIdentifier = 1;
        } else {
            if(prevWasIdentifier && strcmp(currentToken.tokenName, "(") == 0) {
                // We found: identifier immediately followed by '('.
                // In a real Java parser you would check modifiers and return types.
                // Here we assume that this is a function (method) call/definition.
                // For demonstration, we set the return type as "unknown".
                addFunctionSymbol(prevToken, "unknown");
            }
            prevWasIdentifier = 0;
        }
    }
    
    fclose(source_fp);
    
    // Display the symbol table for function names.
    printf("\nSymbol Table for Function Names:\n");
    printf("SlNo\tFunction Name\tReturn Type\n");
    for (int i = 0; i < symbolCount; i++){
        printf("%d\t%s\t\t%s\n", i+1, symbolTable[i].lexeme, symbolTable[i].returnType);
    }
    
    return 0;
}
