/*
S→ a | > | ( T )
T→ T,S | S
*/

/*
S-> a | > | ( T )
T-> ST'
T'-> ,ST' | 𝜖
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int curr = 0;
char str[100];
void S();
void T();
void Tprime();
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
void T()
{
	S();
	Tprime();
}
void Tprime()
{
	if(str[curr]==',')
	{
		curr++;
		S();
		Tprime();
	}
}
void S()
{
	if(str[curr]=='a')
	{
		curr++;
		return;
	}
	else if(str[curr]=='>')
	{
		curr++;
		return;
	}
	else if(str[curr] == '(')
	{
		curr++;
		T();
		if(str[curr] == ')')
		{
			curr++;
			return;
		}
		else
		invalid();
	}
	else
		invalid();
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