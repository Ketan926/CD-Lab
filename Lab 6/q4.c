/*
S →(L) | a
L → L,S | S
*/
/*
S -> (L) | a
L -> SL'
L'-> ,SL' | 𝜖
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int curr = 0;
char str[100];
void S();
void L();
void Lprime();
void invalid()
{
	printf("-----------------ERROR!----------------\n");
	exit(0);
}
void valid()
{
	printf("----------------SUCCESS!---------------\n");
	exit(0);
}
void L()
{
	S();
	Lprime();
}
void Lprime()
{
	if(str[curr]==',')
	{
		curr++;
		S();
		Lprime();
	}
}
void S()
{
	if(str[curr]=='(')
	{
		curr++;
		L();
		if(str[curr]==')')
		{
			curr++;
			return;
		}
		else 
		{
			invalid();
		}
	}
	else if(str[curr]=='a')
	{
		curr++;
		return;
	}
	else
	{
		invalid();
	}
}
int main()
{
	printf("Enter String: ");
	scanf("%s", str);
	S();
	if(str[curr] == '$')
	valid();
	else
	// printf("%c\n", str[curr]);
	invalid();
}