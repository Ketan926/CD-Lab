#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TABLE_SIZE 100

typedef struct 
{
    char *verb;
    int used;
} HashEntry;

HashEntry Table[TABLE_SIZE];

unsigned int hash(char *str) 
{
    unsigned int hashVal = 0;
    while (*str != '\0')
    {
        hashVal = hashVal*31 + *str++; // hashVal = hashVal*31 + current_char
    }
    return hashVal % TABLE_SIZE;
}

void insert(char *verb) 
{
    int index = search(verb);
    if (index != -1) 
    {
        printf("Verb '%s' already exists at index %d.\n", verb, index);
    } 
    else
    {
        index = hash(verb);
        while (Table[index].used) 
        { 
            index = (index + 1) % TABLE_SIZE;
        }
        Table[index].verb = verb;
        Table[index].used = 1;
        printf("Inserted verb '%s' at index %d.\n", verb, index);
    }
}

int search(char *key) 
{
    int index = hash(key);
    int initialIndex = index;
    while (Table[index].used)
    {
        if (strcmp(Table[index].verb, key) == 0) 
        {
            return index;
        }
        index = (index + 1) % TABLE_SIZE;
        if (index == initialIndex) 
        {
            break; 
        }
    }
    return -1;
}

int isVerb(char *word) 
{
    const char *verbs[] = {"run", "eat", "go", "have", "make", "take", "see", "do", "say", "come"};
    for (int i = 0; i < 10; i++) 
    {
        if (strcmp(word, verbs[i]) == 0) 
        {
            return 1;
        }
    }
    return 0;
}

void processStatement(char *statement) 
{
    char word[100];
    int i = 0;    
    for (int j = 0; j <= strlen(statement); j++) 
    {
        if (isalpha(statement[j])) 
        {
            word[i++] = tolower(statement[j]);
        } 
        else 
        {
            word[i] = '\0';
            if (i > 0 && isVerb(word)) 
            {
                insert(word);
            }
            i = 0;
        }
    }
}

int main() 
{
    for (int i = 0; i < TABLE_SIZE; i++) 
    {
        Table[i].used = 0;
    }
    char statement[500];
    printf("Enter a statement: ");
    fgets(statement, sizeof(statement), stdin);
    processStatement(statement);
    return 0;
}
