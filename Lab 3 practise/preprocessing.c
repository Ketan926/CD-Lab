#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
void remove_comments(FILE *f1, FILE *f2)
{
    char c;
    char d;
    char line[1024];
    c=getc(f1);
    while (c!=EOF)
    {
        if(c=='/')
        {
            d=getc(f1);
            if(d=='/')
            {
                while(c!='\n' && c!=EOF)
                {
                    c=getc(f1);
                }
            }
            else if(d=='*')
            {
                do
                {
                    while (c!='*' && c!=EOF)
                    {
                        c=getc(f1);
                    }
                    c=getc(f1);
                } while (c!='/' && c!=EOF);
            }
            else
            {
                putc(c,f2);
                putc(d,f2);
            }
        }
        else
        {
            putc(c,f2);
        }
        c=getc(f1);
    }  
    fclose(f1);
    fclose(f2);
}
void remove_headers(FILE *f1, FILE *f2)
{
    char c;
    char d;
    char line[1024];
    while(fgets(line,sizeof(line),f1)!=NULL)
    {
        if(line[0]!='#')
        {
            fputs(line,f2);
        }
    }
    fclose(f1);
    fclose(f2);
}
void remove_spaces(FILE *f1, FILE *f2)
{
    char c;
    char d;
    char line[1024];
    c=getc(f1);
    while(c!=EOF)
    {
        if(c==' ')
        {
            while (c==' ' && c!=EOF)
            {
                c=getc(f1);
            }
            putc(' ',f2);
        }
        else if(c=='\t' && c!=EOF)
        {
            while (c=='\t')
            {
                c=getc(f1);
            }
            putc(' ',f2);
        }
        else
        {
            putc(c,f2);
            c=getc(f1);
        }
    }
    fclose(f1);
    fclose(f2);
}
int main()
{
    FILE *fa, *fb, *fc, *fd, *fe, *ff;
    fa = fopen("s1.txt", "r");
    fb = fopen("s2.txt", "w+");
    remove_comments(fa, fb);
    fclose(fa);
    fclose(fb);
    fc = fopen("s2.txt", "r");
    fd = fopen("s3.txt", "w+");
    remove_headers(fc, fd);
    fclose(fc);
    fclose(fd);
    fe = fopen("s3.txt", "r");
    ff = fopen("s4.txt", "w+");
    remove_spaces(fe, ff);
    fclose(fe);
    fclose(ff);
}