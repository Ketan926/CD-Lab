#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

struct token
{
    char token[100];
    int row;
    int col;
    char type[50];
    int index;
};
struct token table[100];
const char *keywords[]= {"int", "float", "double", "char", "if", "else", "for", "return",
    "while", "return", "void", "switch", "case", "break", "continue", "default",
    "struct", "union", "enum", "long", "short", "const", "sizeof","printf","scanf"};
#define NUM_KEYWORDS 25
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
void generate_token(FILE *f1, FILE *f2)
{
    char c,next;
    int row=1,col=1,i=0;
    int cur_row, cur_col;
    char word[50];
    char number[50];
    char string[50];
    c=getc(f1);
    int ind=0;
    while(c!=EOF)
    {
        if(isspace(c))
        {
            while(isspace(c))
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
                c=getc(f1);
            }
        }
        else if(c=='+' || c=='-' || c=='*' || c=='/' || c=='%')
        {
            i=0;
            table[ind].token[0]=c;
            table[ind].token[1]='\0';
            table[ind].row=row;
            table[ind].col=col;
            table[ind].index=ind+1;
            strcpy(table[ind].type,"Arithmetic Operator");
            fprintf(f2, "<%d, '%s', %d, %d, '%s', >\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            col++;
        }
        else if(c=='=' || c=='!' || c=='>' || c=='<')
        {
            next=getc(f1);
            cur_row=row;
            cur_col=col;
            col++;
            if(next=='=')
            {
                i=0;
                table[ind].token[0]=c;
                table[ind].token[1]=next;
                table[ind].token[2]='\0';
                table[ind].row=cur_row;
                table[ind].col=cur_col;
                table[ind].index=ind+1;
                strcpy(table[ind].type,"Relational Operator");
                fprintf(f2, "<%d, '%s', %d, %d, '%s', >\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                ind++;
                col++;
            }
            else
            {
                ungetc(next,f1);
                col--;
                if(c=='=')
                {
                    i=0;
                    table[ind].token[0]=c;
                    table[ind].token[1]='\0';
                    table[ind].row=row;
                    table[ind].col=col;
                    table[ind].index=ind+1;
                    strcpy(table[ind].type,"Assignment Operator");
                    fprintf(f2, "<%d, '%s', %d, %d, '%s', >\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                    ind++; 
                    col++;  
                }
                else
                {
                    i=0;
                    table[ind].token[0]=c;
                    table[ind].token[1]='\0';
                    table[ind].row=row;
                    table[ind].col=col;
                    table[ind].index=ind+1;
                    strcpy(table[ind].type,"Relational Operator");
                    fprintf(f2, "<%d, '%s', %d, %d, '%s', >\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                    ind++; 
                    col++;  
                }
            }
        }
        else if(c=='&' || c=='|')
        {
            next=getc(f1);
            cur_row=row;
            cur_col=col;
            col++;
            if(next=='&')
            {
                i=0;
                table[ind].token[0]=c;
                table[ind].token[1]=next;
                table[ind].token[2]='\0';
                table[ind].row=cur_row;
                table[ind].col=cur_col;
                table[ind].index=ind+1;
                strcpy(table[ind].type,"Logical Operator");
                fprintf(f2, "<%d, '%s', %d, %d, '%s', >\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                ind++;
                col++;
            }   
            else if(next=='|')   
            {
                i=0;
                table[ind].token[0]=c;
                table[ind].token[1]=next;
                table[ind].token[2]='\0';
                table[ind].row=cur_row;
                table[ind].col=cur_col;
                table[ind].index=ind+1;
                strcpy(table[ind].type,"Logical Operator");
                fprintf(f2, "<%d, '%s', %d, %d, '%s', >\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
                ind++; 
                col++;               
            }      
            else
            {
                ungetc(next,f1);
                col--;
            }
        }
        else if(isdigit(c))
        {
            i=0;
            cur_row=row;
            cur_col=col;
            number[i]=c;
            c=getc(f1);
            i++;
            col++;
            while (isdigit(c) || c=='.')
            {
                number[i]=c;
                i++;
                c=getc(f1);
                col++;

            }
            number[i]='\0';
            strcpy(table[ind].token,number);
            table[ind].row=cur_row;
            table[ind].col=cur_col;
            table[ind].index=ind+1;
            strcpy(table[ind].type,"Numeric");
            fprintf(f2, "<%d, '%s', %d, %d, '%s', >\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
        }
        else if(isalpha(c) || c=='_')
        {
            i=0;
            cur_row=row;
            cur_col=col;
            word[i]=c;
            c=getc(f1);
            i++;
            col++;
            while (isalpha(c) || c=='_' || isdigit(c))
            {
                word[i]=c;
                i++;
                getc(f1);
                col++;

            }
            word[i]='\0';
            strcpy(table[ind].token,word);
            table[ind].row=cur_row;
            table[ind].col=cur_col;
            table[ind].index=ind+1;
            if(iskeyword(word))
            {
                strcpy(table[ind].type,"Keyword");
            }
            else
            {
                strcpy(table[ind].type,"Identifier");
            }
            fprintf(f2, "<%d, '%s', %d, %d, '%s', >\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
            ungetc(c,f1);
        }
        else if(c=='"')
        {
            char quote=c;
            cur_row=row;
            cur_col=col;
            i=0;
            string[i]=c;
            i++;
            c=getc(f1);
            col++;
            while(c!=quote && c!=EOF)
            {
                string[i]=c;
                i++;
                col++;
                c=getc(f1);
            }
            string[i]=quote;
            i++;
            string[i]='\0';
            strcpy(table[ind].token,string);
            table[ind].row=cur_row;
            table[ind].col=cur_col;
            table[ind].index=ind+1;
            strcpy(table[ind].type,"String Literal");
            fprintf(f2, "<%d, '%s', %d, %d, '%s', >\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;
        }
        else
        {
            i=0;
            table[ind].token[0]=c;
            table[ind].token[1]='\0';
            table[ind].row=row;
            table[ind].col=col;
            table[ind].index=ind+1;
            strcpy(table[ind].type,"Special Symbol");
            fprintf(f2, "<%d, '%s', %d, %d, '%s', >\n", table[ind].index, table[ind].token, table[ind].row, table[ind].col, table[ind].type);
            ind++;   
            col++;
        }
        c=getc(f1);
    }
}
int main()
{
    FILE *f1, *f2;
    f1=fopen("s4.txt","r");
    f2=fopen("s5.txt","w+");
    generate_token(f1,f2);
    fclose(f1);
    fclose(f2);
}