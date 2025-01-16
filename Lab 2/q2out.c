int main()
{
	FILE *fa, *fb;
	int ca,cb;
	char line[1024];
	fa=fopen("q2.c","r");
	if(fa == NULL)
	{
		printf("Cannot open file\n");
		exit(0);
	}
	fb = fopen("q2out.c","w");
	while (fgets(line, sizeof(line),fa))
	{
		if(line[0]!='#')
			fputs(line,fb);
	}
	fclose(fa);
	fclose(fb);
	return 0;
}