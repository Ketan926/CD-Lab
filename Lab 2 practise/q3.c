#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
char *words[]={"if","else", "int", "char","break","for","while","return","float","void"};
int check(char *word)
{
    for(int i=0;i<10;i++)
    {
        if(strcmp(word,words[i])==0)
            return 1;
    }
    return 0;
}
void to_uppercase(char *word)
{
    for(int i=0;i<strlen(word);i++)
    {
        word[i]=toupper(word[i]);
    }
}
int main()
{
    FILE *f1, *f2;
    char word[10],filename[100];
    printf("Enter file name to read\n");
    scanf("%s",filename);
    f1=fopen(filename,"r");
    printf("Enter file name to write\n");
    scanf("%s",filename);
    f2=fopen(filename,"w+");
    char c=getc(f1);
    while(c!=EOF)
    {
        if(isalpha(c))
        {
            int i=0;
            while(isalpha(c))
            {
                word[i]=c;
                c=getc(f1);
                i++;
            }
            word[i]='\0';
            if(check(word))
            {
                to_uppercase(word);
                fputs(word,f2);
                putc('\n',f2);
            }
        }
        else
        {
            c=getc(f1);
        }
    }
    fclose(f1);
    fclose(f2);
}