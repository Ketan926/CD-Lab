#include<stdio.h>
#include<stdlib.h>
int main()
{
    FILE *f1, *f2;
    char filename[100],c;
    int i,res;
    printf("Enter the name of the filename to read\n");
    scanf("%s",filename);
    f1=fopen(filename,"r");
    printf("Enter file name to write\n");
    scanf("%s",filename);
    f2=fopen(filename,"w+");
    fseek(f1,0,SEEK_END);
    res=ftell(f1);
    for(i=res-1;i>=0;i--)
    {
        fseek(f1,i,SEEK_SET);
        c=fgetc(f1);
        fputc(c,f2);
    }
    fclose(f1);
    fclose(f2);
}