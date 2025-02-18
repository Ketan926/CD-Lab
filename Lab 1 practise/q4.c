#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
struct Hashword
{
    char *word;
    int used;
};
struct Hashword Hashtable[100];
int hash(char *word)
{
    int hashval=0;
    while (*word!='\0')
    {
        hashval=hashval*10+*word;
        word++;
    }
    return hashval%100;
}
int search(char *word)
{
    int index=hash(word);
    int initial=index;
    while (Hashtable[index].used)
    {
        if(strcmp(Hashtable[index].word,word)==0)
            return index;
        index=(index+1)%100;
        if(index==initial)
            break;
    }
    return -1;
}
int isverb(char *word)
{
    char *verbs[]={"run", "eat", "go", "have", "make", "take", "see", "do", "say", "come"};
    for(int i=0; i<10;i++)
    {
        if(strcmp(word,verbs[i])==0)
            return 1;
    }
    return 0;
}
void insert(char *word)
{
    int index=search(word);
    if(index!=-1)
    {
        printf("Word %s exists at %d\n",word,index);
    }
    else
    {
        index=hash(word);
        while(Hashtable[index].used==1)
        {
            index=(index+1)%100;
        }
        Hashtable[index].word=word;
        Hashtable[index].used=1;
        printf("Word %s inserted at %d\n",word,index);
    }
}
void process(char *statement)
{
    char word[100];
    int i=0,j=0;
    for(i=0;i<=strlen(statement);i++)
    {
        if(isalpha(statement[i]))
        {
            word[j]=tolower(statement[i]);
            j++;
        }
        else
        {
            word[j]='\0';
            if(j>0 && isverb(word))
            {
                insert(word);
            }
            j=0;
        }
    }
}
int main()
{
    for(int i=0;i<100;i++)
    {
        Hashtable[i].used=0;
    }
    char statement[100];
    printf("Enter your statement\n");
    fgets(statement,sizeof(statement),stdin);
    process(statement);
    return 0;
}