#include <stdio.h>
#include <stdlib.h> // For exit()
int main()
{
	int lines=0, chars=0;
	FILE *fptr1, *fptr2, *fptr3;
	char filename[100], c1,c2;
	printf("Enter the filename to open for reading: \n");
	scanf("%s", filename);
	fptr1 = fopen(filename, "r");
	if (fptr1 == NULL)
	{
		printf("Cannot open file %s \n", filename);
		exit(0);
	}
	printf("Enter the filename to open for reading: \n");
	scanf("%s", filename);
	fptr2 = fopen(filename, "r");
	if (fptr2 == NULL)
	{
		printf("Cannot open file %s \n", filename);
		exit(0);
	}
	printf("Enter the filename to open for writing: \n");
	scanf("%s", filename);
	fptr3 = fopen(filename, "w+"); 
	while(1)
	{
		if(c1!=EOF)
		{
			c1=fgetc(fptr1);
			while(c1!='\n')
			{
				if(c1==EOF)
					break;
				fputc(c1,fptr3);
				c1=fgetc(fptr1);
			}
			if(c1=='\n')
				fputc('\n',fptr3);
		}
		if(c2!=EOF)
		{
			c2=fgetc(fptr2);
			while(c2!='\n')
			{
				if(c2==EOF)
					break;
				fputc(c2,fptr3);
				c2=fgetc(fptr2);
			}
			if(c2=='\n')
				fputc('\n',fptr3);
		}
		if(c1==EOF && c2==EOF)
			break;
	}
	printf("merged files alternatively\n");
	fclose(fptr1);
	fclose(fptr2);
	fclose(fptr3);
	return 0;
}