#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void printString(char *string)
{
    printf("%s\n", string);
}

char *subString(char *string, int start, int end)
{
    char *substring = malloc(sizeof(char) * (end - start) );
    for (int i = 0; i < strlen(string) ; i++) {
        strncpy(substring, string + start, end - start);
    }
    return substring;
}

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}