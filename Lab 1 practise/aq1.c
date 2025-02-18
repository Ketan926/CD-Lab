#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>

int main()
{
    FILE *f1;
    char filename[100],c, line[1024];
    printf("Enter the filename to read\n");
    scanf("%s",filename);
    f1=fopen(filename,"r");
    if(f1==NULL)
    {
        printf("cant open file\n");
        exit(0);
    }

    int blank_spaces=0;
    int blank_lines=0;
    int end_lines=0;

    while(fgets(line,sizeof(line),f1)!=NULL)
    {
        int i=0;
        int length=0;
        while(line[i]!='\n' && line[i]!='\0')
        {
            if(isspace(line[i]))
            {
                blank_spaces++;
            }
            i++;
        }
        if(i==0)
        {
            blank_lines++;
        }
        if(i>0 && line[i-1]==';')
        {
            end_lines++;
        }
    }

    printf("Blank Spaces %d\n",blank_spaces);
    printf("Blank Lines %d\n",blank_lines);
    printf("Lines ending with semicolon %d\n",end_lines);
    fclose(f1);
    return 0;
}