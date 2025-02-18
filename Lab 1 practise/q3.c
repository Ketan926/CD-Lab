#include<stdio.h>
#include<stdlib.h>
int main()
{
    FILE *f1, *f2, *f3;
    char filename[100],c1,c2;
    int i,res;
    printf("Enter the name of the filename to read\n");
    scanf("%s",filename);
    f1=fopen(filename,"r");
    printf("Enter the name of the filename to read\n");
    scanf("%s",filename);
    f2=fopen(filename,"r");
    printf("Enter file name to write\n");
    scanf("%s",filename);
    f3=fopen(filename,"w+");
    while(1)
    {
        if(c1!=EOF)
        {
            c1=fgetc(f1);
            while(c1!='\n')
            {
                if(c1==EOF)
                    break;
                fputc(c1,f3);
                c1=fgetc(f1);
            }
            if(c1=='\n')
                fputc('\n',f3);
        }
        if(c2!=EOF)
        {
            c2=fgetc(f2);
            while(c2!='\n')
            {
                if(c2==EOF)
                    break;
                fputc(c2,f3);
                c2=fgetc(f2);
            }
            if(c2=='\n')
                fputc('\n',f3);
        }
        if(c1==EOF && c2==EOF)
            break;
    }
}