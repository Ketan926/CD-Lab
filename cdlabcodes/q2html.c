#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LEN 200
#define MAX_SYMBOLS   50

// ---------------------------------
// Token structure for HTML tokens
// ---------------------------------
typedef struct {
    char tokenName[MAX_TOKEN_LEN]; // Lexeme (for tag or text)
    char type[20];                 // Token Type: "TAG", "TEXT", or "EOF"
    unsigned int row, col;         // Location in source file
} Token;

// ---------------------------------
// Symbol (tag) table entry for HTML tags
// ---------------------------------
typedef struct {
    char tagName[50];     // The HTML tag (e.g., "html", "body", "p")
    char category[20];    // We use a fixed category "tag"
} Symbol;

// Global symbol table (an array of Symbols)
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
// Reads the next character from the file and updates row/col counters.
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
// skipWhitespace()
// Skips spaces and newline characters.
// ---------------------------------
void skipWhitespace(void) {
    while (currentChar != EOF && isspace(currentChar))
        getNextChar();
}

// ---------------------------------
// skipHTMLCommentsAndHeaders()
// If the current token begins with "<!" then check if it is a comment (<!-- ... -->)
// or a header such as <!DOCTYPE ...> and skip it completely.
// ---------------------------------
void skipHTMLCommentsAndHeaders(void) {
    while (currentChar != EOF) {
        skipWhitespace();
        if (currentChar == '<') {
            // Save current file position for peeking.
            long pos = ftell(source_fp);
            char buffer[10] = {0};
            int i = 0;
            // We want to peek the characters immediately after '<'
            int temp = fgetc(source_fp);
            if (temp != EOF) {
                buffer[i++] = (char)temp;
            } else {
                break;
            }
            // If the next character is '!', we might have a comment or header.
            if (buffer[0] == '!') {
                // Read a few more characters to decide.
                for (; i < 4 && currentChar != EOF; i++) {
                    temp = fgetc(source_fp);
                    if (temp == EOF) break;
                    buffer[i] = (char)temp;
                }
                buffer[i] = '\0';
                // If buffer begins with "!--", it is an HTML comment.
                if (strncmp(buffer, "!--", 3) == 0) {
                    // Skip until we see the closing sequence "-->"
                    while (currentChar != EOF) {
                        if (currentChar == '-') {
                            int a = fgetc(source_fp);
                            int b = fgetc(source_fp);
                            if (a == '-' && b == '>') {
                                getNextChar();  // Advance after ">"
                                break;
                            } else {
                                if (b != EOF) ungetc(b, source_fp);
                                if (a != EOF) ungetc(a, source_fp);
                            }
                        }
                        getNextChar();
                    }
                    continue; // Check again after skipping comment.
                }
                // Otherwise, if it’s a header like <!DOCTYPE ...> or any declaration starting with <! ...
                else {
                    // Skip until you find '>'
                    while (currentChar != EOF && currentChar != '>')
                        getNextChar();
                    if (currentChar == '>')
                        getNextChar();
                    continue;
                }
            } else {
                // Not a header/comment; revert to saved position
                fseek(source_fp, pos - 1, SEEK_SET);
                currentChar = fgetc(source_fp);
                break;
            }
        } else {
            break;
        }
    }
}

// ---------------------------------
// addTagSymbol()
// Adds a unique tag to the symbol table
// ---------------------------------
void addTagSymbol(const char *tagName) {
    // Check for duplicates
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].tagName, tagName) == 0)
            return;
    }
    if (symbolCount >= MAX_SYMBOLS)
        return;
    strcpy(symbolTable[symbolCount].tagName, tagName);
    strcpy(symbolTable[symbolCount].category, "tag");
    symbolCount++;
}

// ---------------------------------
// getNextToken()
// Returns the next token from the HTML source. Two kinds of tokens are generated:
// 1. When a '<' is encountered, a TAG token is produced (the tag name is read).
// 2. Otherwise, a TEXT token is produced (all characters until the next '<').
// Comments and header declarations are skipped.
// ---------------------------------
Token getNextToken(void) {
    Token tok;
    int idx = 0;
    tok.tokenName[0] = '\0';
    tok.type[0] = '\0';
    tok.row = row;
    tok.col = col;
    
    skipHTMLCommentsAndHeaders();
    skipWhitespace();
    
    if (currentChar == EOF) {
        strcpy(tok.tokenName, "EOF");
        strcpy(tok.type, "EOF");
        return tok;
    }
    
    // If we are at a tag (starting with '<')
    if (currentChar == '<') {
        getNextChar(); // Skip the '<'
        // If it is a closing tag, skip the '/'
        if (currentChar == '/')
            getNextChar();
        // Now read the tag name.
        while (currentChar != EOF && (isalnum(currentChar) || currentChar == '-' || currentChar == '_')) {
            tok.tokenName[idx++] = (char)currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "TAG");
        tok.row = row;
        tok.col = col;
        if (strlen(tok.tokenName) > 0) {
            addTagSymbol(tok.tokenName);
        }
        // Skip any attributes and remaining characters until the closing '>'.
        while (currentChar != EOF && currentChar != '>') {
            getNextChar();
        }
        if (currentChar == '>')
            getNextChar();  // Skip the '>'
        return tok;
    } else {
        // Otherwise, read text until the next '<'
        while (currentChar != EOF && currentChar != '<') {
            tok.tokenName[idx++] = (char)currentChar;
            getNextChar();
        }
        tok.tokenName[idx] = '\0';
        strcpy(tok.type, "TEXT");
        tok.row = row;
        tok.col = col;
        return tok;
    }
}

// ---------------------------------
// Main function
// ---------------------------------
int main(int argc, char *argv[]) {
    source_fp = fopen("q2html.txt", "r");
    if (source_fp == NULL) {
        printf("Cannot open file ");
        return 1;
    }
    getNextChar();
    
    printf("Scanning HTML tokens...\n");
    while (1) {
        Token t = getNextToken();
        if (strcmp(t.type, "EOF") == 0)
            break;
        // For TEXT tokens, skip if they are only whitespace.
        if (strcmp(t.type, "TEXT") == 0) {
            int allSpace = 1;
            for (int i = 0; i < strlen(t.tokenName); i++) {
                if (!isspace(t.tokenName[i])) {
                    allSpace = 0;
                    break;
                }
            }
            if (allSpace)
                continue;
        }
        printf("<%s, %s, row: %d, col: %d>\n", t.tokenName, t.type, t.row, t.col);
    }
    fclose(source_fp);
    
    // Display the symbol table (HTML tag names)
    printf("\nSymbol Table for HTML Tags:\n");
    printf("SlNo\tTag Name\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%d\t%s\n", i + 1, symbolTable[i].tagName);
    }
    return 0;
}
