#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void display(FILE *f)
{
    int count=0;
    char line[1024];
    while(fgets(line,sizeof(line),f)!=NULL)
    {
        int i=0;
        while (line[i]!='\0')
        {
            printf("%c",line[i]);
            i++;
        }
        count++;
        if(count%5==0)
            break;
    }
}
int main()
{
    FILE *f1, *f2, *f3, *f4;
    char ch;
    f1=fopen("q1.c","r");
    f2=fopen("q2.c","r");
    f3=fopen("q3.c","r");
    f4=fopen("q4.c","r");
    do
    {
        printf("1. q1.c 2. q2.c 3. q3.c 4. q4.c Q. Quit\n");
        printf("Enter your choice:  ");
        scanf(" %c",&ch);
        switch (ch)
        {
        case '1':
            display(f1);
            break;
        case '2':
            display(f2);
            break; 
        case '3':
            display(f3);
            break;
        case '4':
            display(f4);
            break;  
        case 'Q':
            printf("exiting");
            fclose(f1);
            fclose(f2);
            fclose(f3);
            fclose(f4);
            return 0;       
        }
    } while (ch!='Q');
    
}