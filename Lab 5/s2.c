#include <stdio.h>
int main()
{
	int a,b;
	char c[20];
	WRITE("abcd\n");
	READ("%d",&a);
	READ("%d",&b);
	WRITE("cdef\n");
	READ("%s",c);
}