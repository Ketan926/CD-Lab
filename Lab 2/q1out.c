#include<stdio.h>
#include<stdlib.h>
int main()
{
 FILE *fa, *fb;
 int ca,cb;
 fa=fopen("q1.c","r");
 if(fa == NULL)
 {
 printf("Cannot open file\n");
 exit(0);
 }
 fb = fopen("q1out.c","w");
 ca = getc(fa);
 while (ca != EOF)
 {
 if(ca == ' ')
 {
 while(ca == ' ')
 {
 ca = getc(fa);
 }
 putc(' ',fb);
 }
 else if(ca == '\t')
 {
 while(ca == '\t')
 {
 ca = getc(fa);
 }
 putc(' ',fb);
 }
 else
 {
 putc(ca,fb);
 ca = getc(fa);
 }
 }
 fclose(fa);
 fclose(fb);
 return 0;
}