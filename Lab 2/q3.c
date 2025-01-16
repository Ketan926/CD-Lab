#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>
int check(char word[10])
{
	return (strcmp(word,"else")==0 || strcmp(word, "int")==0 ||
		strcmp(word,"while")==0 || strcmp(word,"if")==0 ||
		strcmp(word,"return")==0 || strcmp(word,"char")==0);
}
void to_uppercase(char word[]) 
{
    for (int i = 0; word[i] != '\0'; i++) 
    {
        word[i] = toupper(word[i]);
    }
}
int main()
{
	FILE *fa, *fb;
	int i,ca,cb;
	char word[10];
	fa=fopen("q3.c","r");
	if(fa == NULL)
	{
		printf("Cannot open file\n");
		exit(0);
	}
	fb = fopen("q3out.c","w");
	ca = getc(fa);
	while (ca != EOF)
	{
		if(ca >= 'a' && ca <= 'z')
		{
			i=0;
			while(ca >= 'a' && ca <= 'z')
			{
				word[i]=ca;
				ca = getc(fa);
				i++;
			}
			word[i]='\0';
			if(check(word))
			{
				to_uppercase(word);
				fputs(word,fb);
				putc('\n',fb);
			}
		}
		else
		{
			ca = getc(fa);
		}
	}
	fclose(fa);
	fclose(fb);
	return 0;
}