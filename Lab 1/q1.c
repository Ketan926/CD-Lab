#include <stdio.h>
#include <stdlib.h> // For exit()
int main()
{
	int lines=0, chars=0;
	FILE *fptr1, *fptr2;
	char filename[100], c;
	printf("Enter the filename to open for reading: \n");
	scanf("%s", filename);
	fptr1 = fopen(filename, "r");
	if (fptr1 == NULL)
	{
		printf("Cannot open file %s \n", filename);
		exit(0);
	}
	c = fgetc(fptr1);
	while (c != EOF)
	{
		c = fgetc(fptr1);
		if(c=='\n')
			lines++;
		else 
			chars++;
	}
	printf("number of characters are %d\n",chars);
	printf("number of lines are %d\n",lines);
	fclose(fptr1);
	return 0;
}