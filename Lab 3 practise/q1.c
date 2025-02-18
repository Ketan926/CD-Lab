#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
void identify_operators(FILE *f1)
{
    char c,next;
    //c=getc(f1);
    int row=1, col=0;
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
        else if(c=='+' || c=='-' || c=='*' || c=='/')
        {
            printf("%c Arithmetic Operation Row-%d Col-%d\n",c,row,col);
            col++;
        }
        else if(c=='=' || c=='!' || c=='>' || c=='<')
        {
            next=getc(f1);
            col++;
            if(next=='=')
            {
                printf("%c%c Relational Operation Row-%d Col-%d\n",c,next,row,col);
                col++;
            }
            else
            {
                ungetc(next,f1);
                col--;
                if(c=='=')
                {
                    printf("%c Assignment Operation Row-%d Col-%d\n",c,row,col);
                    col++;
                }
                else
                {
                    printf("%c Relational Operation Row-%d Col-%d\n",c,row,col);
                    col++;
                }
            }
        }
        else if(c=='&' || c=='|')
        {
            next=getc(f1);
            col++;
            if(c=='&' && next=='&')
            {
                printf("%c%c Logical Operation Row-%d Col-%d\n",c,next,row,col);
                col++;
            }
            else if(c=='|' && next=='|')
            {
                printf("%c%c Logical Operation Row-%d Col-%d\n",c,next,row,col);
                col++;
            }
            else
            {
                ungetc(next,f1);
                col--;
            }
        }
        else
        {
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