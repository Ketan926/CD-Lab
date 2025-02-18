#include<stdio.h>
#include<stdlib.h>
int main()
{
    FILE *f1, *f2;
    char filename[100],c;
    int char_count=0, line_count=0;
    printf("Enter filename to open for reading\n");
    scanf("%s",filename);
    f1=fopen(filename,"r");
    if(f1==NULL)
    {
        printf("Cannot open file for reading\n");
        exit(0);
    }
    printf("Enter the filename to open for writing\n");
    scanf("%s",filename);
    f2=fopen(filename,"w+");
    c=fgetc(f1);
    while (c!=EOF)
    {
        char_count++;
        if(c=='\n')
        {
            line_count++;
        }
        fputc(c,f2);
        c=fgetc(f1);
    }
    fclose(f1);
    fclose(f2);
    printf("Character count is %d\nLine Count is %d\n",char_count,line_count);
    return 0;
}