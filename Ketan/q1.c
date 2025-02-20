#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

struct token
{
    char token[50];
    int row;
    int col;
    int index;
    char type[20];
};
struct token table[200];
struct symbol
{
    int sno;
    char lexeme[50];
    char datatype[20];
    int size;
};
struct symbol symtable[20];
int symind=0;

const char *keywords[] = {
    "int", "float", "double", "char", "if", "else", "for", "return",
    "while", "void", "switch", "case", "break", "continue",
    "default", "struct", "union", "enum", "long", "short", "const",
    "sizeof", "printf", "scanf"
};

#define NUM_KEYWORDS 24

int iskeyword(char *word)
{
    for(int i=0;i<NUM_KEYWORDS;i++)
    {
        if(strcmp(word,keywords[i])==0)
        {
            return 1;
        }
    }
    return 0;
}

int tokenexists(char *word, int ind)
{
    for(int i=0;i<ind;i++)
    {
        if(strcmp(word, table[i].token)==0)
        {
            return table[i].index;
        }
    }
    return -1;
}

int getdatatypesize(char *word)
{
    if(strcmp(word,"int")==0) return 4;
    else if(strcmp(word,"float")==0) return 4;
    else if(strcmp(word,"double")==0) return 8;
    else if(strcmp(word,"char")==0) return 1;
    else return 0;
}

void addtotable(char *lexeme, char *datatype, int arraysize, int isfunc)
{
    int size = getdatatypesize(datatype) * arraysize;
    for(int i=0; i<symind; i++)
    {
        if(strcmp(lexeme, symtable[i].lexeme) == 0)
        {
            return; // Prevent duplicate entry
        }
    }
    symtable[symind].sno = symind + 1;
    strcpy(symtable[symind].lexeme, lexeme);
    if (isfunc)
    {
        strcpy(symtable[symind].datatype, "Function");
        size = 1; // Functions are usually not assigned a size
    }
    else
    {
        strcpy(symtable[symind].datatype, datatype);
    }
    symtable[symind].size = size;
    symind++;
}


void get_token(FILE *f1, FILE *f2)
{
    char word[20];
    char datatype[20];
    char number[20];
    char string[30];
    char c,next;
    int i=0;
    int row=1,col=1;
    int cur_row, cur_col;
    int isfunc=0;
    int arraysize=1;
    int ind=0;

    while((c=getc(f1))!=EOF)
    {
        if(isspace(c))
        {
            if(c=='\n')
            {
                row++;
                col=1;
            }
            else
            {
                col++;
            }
            continue;
        }
        cur_col=col;
        cur_row=row;
        if(c=='+' || c=='-' || c=='*' || c=='/' || c=='%')
        {
            table[ind].index=ind+1;
            table[ind].row=cur_row;
            table[ind].col=cur_col;
            table[ind].token[0]=c;
            table[ind].token[1]='\0';
            strcpy(table[ind].type,"Arithmetic Op");
            fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col++;
        }
        if(c=='=' || c=='>' || c=='<' || c=='!')
        {
            next=getc(f1);
            if(next=='=')
            {
                col++;
                table[ind].index=ind+1;
                table[ind].row=cur_row;
                table[ind].col=cur_col;
                table[ind].token[0]=c;
                table[ind].token[1]=next;
                table[ind].token[2]='\0';
                strcpy(table[ind].type,"Relational Op");
                fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                ind++;
                col++;
            }
            else
            {
                ungetc(next,f1);
                if(c=='=')
                {
                    table[ind].index=ind+1;
                    table[ind].row=cur_row;
                    table[ind].col=cur_col;
                    table[ind].token[0]=c;
                    table[ind].token[1]='\0';
                    strcpy(table[ind].type,"Assignment Op");
                    fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                    ind++;
                    col++;
                }
                else
                {
                    table[ind].index=ind+1;
                    table[ind].row=cur_row;
                    table[ind].col=cur_col;
                    table[ind].token[0]=c;
                    table[ind].token[1]='\0';
                    strcpy(table[ind].type,"Relational Op");
                    fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                    ind++;
                    col++;
                }
            }
        }
        if(isdigit(c))
        {
            i=0;
            number[i]=c;
            i++;
            col++;
            c=getc(f1);
            while (isdigit(c) || c=='.')
            {
                number[i]=c;
                i++;
                c=getc(f1);
            }
            number[i]='\0';
            strcpy(table[ind].token,number);
            strcpy(table[ind].type,"Numeric");
            table[ind].index=ind+1;
            table[ind].row=cur_row;
            table[ind].col=cur_col;
            fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col+=i;
        }
        if(isalpha(c) || c=='_')
        {
            i=0;
            word[i]=c;
            i++;
            c=getc(f1);
            col++;
            while(isalnum(c) || c=='_')
            {
                word[i]=c;
                i++;
                c=getc(f1);
            }
            word[i]='\0';
            ungetc(c,f1);
            next=getc(f1);
            if(next=='(')
            {
                isfunc=1;
            }
            ungetc(next,f1);
            if(iskeyword(word))
            {   
                strcpy(table[ind].type,"Keyword");
                strcpy(table[ind].token,word);
                table[ind].index=ind+1;
                table[ind].row=cur_row;
                table[ind].col=cur_col;
                strcpy(datatype,word);
                fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                ind++;
            }
            else if(isfunc)
            {
                //strcpy(table[ind].type,"Function");
                isfunc=0;
            }
            else
            {
                strcpy(table[ind].type,"Identifier");
                strcpy(table[ind].token,word);
                table[ind].index=ind+1;
                table[ind].row=cur_row;
                table[ind].col=cur_col; 
                arraysize=1;
                c=getc(f1);
                if(c=='[')
                {
                    fscanf(f1,"%d]",&arraysize);
                }
                else
                {
                    ungetc(c,f1);
                }
                if(datatype[0]!='\0')
                {
                    addtotable(word,datatype,arraysize,isfunc);
                }
                int val=tokenexists(word, ind);
                if(val!=-1)
                {
                    char sid[50];
                    sprintf(sid,"%d",val);
                    strcpy(table[ind].token,sid);
                    fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);

                }
                else
                {
                    fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                }
                //fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                ind++;
            }
            col+=i;
        }
        if(c=='"')
        {
            i=0;
            string[i]=c;
            i++;
            col++;
            c=getc(f1);
            while (c!='"')
            {
                string[i]=c;
                i++;
                c=getc(f1);
            }
            string[i]='"';
            i++;
            string[i]='\0';
            strcpy(table[ind].token,string);
            strcpy(table[ind].type,"String Literal");
            table[ind].index=ind+1;
            table[ind].row=cur_row;
            table[ind].col=cur_col;
            fprintf(f2, "<%d, '%s', %d, %d, '%s'>\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col+=i;;
        }
        else
        {

        }
    }
}

int main() 
{
    FILE *f1 = fopen("s4.txt", "r");
    FILE *f2 = fopen("s6.txt", "w");
    get_token(f1, f2);

    printf("Symbol Table:\n");
    printf("S.No\tLexeme\tDataType\tSize\n");
    for (int i = 0; i < symind; i++) 
    {
        printf("%d\t%s\t%s\t\t%d\n", symtable[i].sno, symtable[i].lexeme, symtable[i].datatype, symtable[i].size);
    }

    fclose(f1);
    fclose(f2);
    return 0;
}