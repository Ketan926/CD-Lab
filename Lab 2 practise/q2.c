#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main()
{
    FILE *f1, *f2;
    char line[1024],filename[100];
    printf("Enter file name to read\n");
    scanf("%s",filename);
    f1=fopen(filename,"r");
    printf("Enter file name to write\n");
    scanf("%s",filename);
    f2=fopen(filename,"w+");
    while(fgets(line,sizeof(line),f1)!=NULL)
    {
        if(line[0]!='#')
            fputs(line,f2);
    }
    fclose(f1);
    fclose(f2);
}