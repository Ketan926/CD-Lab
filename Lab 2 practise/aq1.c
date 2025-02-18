#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int count_arguments(char *line)
{
    int count=1;
    if(strcmp(line,"void")==0 || strlen(line)==0 || line[1]==')')
        return 0;
    while (*line) 
    {
        if (*line == ',')
            count++;
        line++;
    }
    return count;
}
void extract(char *line)
{
    char return_type[50];
    char function_name[50];
    char arguments[100];
    if(sscanf(line,"%49s %49[^ (] (%99[^)])",return_type, function_name, arguments)==3)
    {
        if (strchr(function_name, '=') || strstr(return_type, "for") || strstr(return_type, "if") || strstr(return_type, "while")) {
            return;
        }
        printf("Function Name: %s\n", function_name);
        printf("Return Type: %s\n", return_type);
        printf("Number of Arguments: %d\n", count_arguments(arguments));
    }
}
int main( )
{
    FILE *f1, *f2;
    char filename[100],line[1024];
    char c;
    printf("Enter the file name to open to read\n");
    scanf("%s",filename);
    f1=fopen(filename,"r");
    while(fgets(line,sizeof(line),f1)!=NULL)
    {
        if(line[0]=='#' || strstr(line,"//") || strstr(line,"/*"))
            continue;
        extract(line);
    }
    fclose(f1);
    return 0;
}