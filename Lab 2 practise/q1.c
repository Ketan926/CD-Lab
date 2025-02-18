#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    FILE *f1, *f2;
    char filename[100];
    char c;
    printf("Enter the file name to open to read\n");
    scanf("%s",filename);
    f1=fopen(filename,"r");
    printf("Enter the file name to open to write\n");
    scanf("%s",filename);
    f2=fopen(filename,"w+");
    c=getc(f1);
    while(c!=EOF)
    {
        if(c==' ')
        {
            while (c==' ')
            {
                c=getc(f1);
            }
            putc(' ',f2);
        }
        else if(c=='\t')
        {
            while (c=='\t')
            {
                c=getc(f1);
            }
            putc('\t',f2);
        }
        else
        {
            putc(c,f2);
            c=getc(f1);
        }
    }
    fclose(f1);
    fclose(f2);
}