#include<stdio.h>
int main()
{
	int a=20;
	int b=10;
	int c=0,d=0;
	printf("Sample Problem\n");
	if(a>=b)
	{
		c=b*2;
		d=a/2;
	}
	else if(a<=b)
	{
		c=b+2;
		d=a-2;
	}
	else if(a==b)
	{
		c=a+b-2;
		d=b-a+10;
	}
	else if(a!=b)
	{
		if(c>d)
		{
			d=d+5;
		}
		else
		{
			c=c+5;
		}
	}
}