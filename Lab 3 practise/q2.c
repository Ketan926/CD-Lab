#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
const char *keywords[] = {"int", "float", "double", "char", "if", "else", "for", "return",
    "while", "return", "void", "switch", "case", "break", "continue", "default",
    "struct", "union", "enum", "long", "short", "const", "sizeof","printf","scanf"};
#define NUM_KEYWORDS 25
int iskeyword(char *str)
{
    for(int i=0;i<NUM_KEYWORDS;i++)
    {
        if(strcmp(str,keywords[i])==0)
            return 1;
    }
    return 0;
}
void identify_operators(FILE *f1)
{
    char c,next;
    char number[10],word[10],string[20];
    int i=0;
    int cur_row=0, cur_col=0;
    c=getc(f1);
    int row=1, col=1;
    while(c!=EOF)
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
        }
        else if(isdigit(c))
        {
           cur_row=row;
           cur_col=col;
           i=0;
           while(isdigit(c)||c=='.')
           {
                number[i]=c;
                c=getc(f1);
                i++;
                col++;
           }
           number[i]='\0';
           printf("%s Numeric row-%d col-%d\n",number,cur_row,cur_col);
           col++;
        }
        else if(isalpha(c))
        {
            cur_row=row;
            cur_col=col;
            i=0;
            while(isalpha(c)||c=='.')
            {
                word[i]=c;
                c=getc(f1);
                i++;
                col++;
            }
            word[i]='\0';
            if(iskeyword(word))
            {
                printf("%s Keyord row-%d col-%d\n",word,cur_row,cur_col);
            }
            else
            {
                printf("%s Identifier row-%d col-%d\n",word,cur_row,cur_col);
            }
            ungetc(c,f1);
        }
        else if(c=='"')
        {
            char quote=c;
            i=0;
            string[i]=quote;
            i++;
            c=getc(f1);
            while (c!=quote)
            {
                string[i]=c;
                c=getc(f1);
                i++;
                col++;
            }
            string[i]=quote;
            i++;
            string[i]='\0';
            printf("%s String Literal row-%d col-%d\n",string,cur_row,cur_col);
            col++;
        }
        else
        {
            printf("%c Special Symbol row-%d col-%d\n",c,row,col);
            col++;
        }
        c=getc(f1);
    }
}
int main()
{
    FILE *f1;
    f1=fopen("s4.txt", "r");
    identify_operators(f1);
    fclose(f1);
}