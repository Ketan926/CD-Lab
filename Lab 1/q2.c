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
	printf("Enter the filename to open for writing: \n");
	scanf("%s", filename);
	fptr2 = fopen(filename, "w+"); // Open another file for writing
	fseek(fptr1,0L,SEEK_END);
	int res=ftell(fptr1);
	printf("Length of file is %d\n",res);
	for(int i=res-1;i>=0;i--)
	{
		fseek(fptr1,i,SEEK_SET);
		c=fgetc(fptr1);
		fputc(c,fptr2);
	}
	fclose(fptr1);
	return 0;
}