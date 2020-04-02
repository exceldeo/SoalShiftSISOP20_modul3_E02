#include<stdio.h>
#include<string.h>
/* search a word from a string) */
int main()
{
char TextString[]="hello Begin hi hour End";
char StringToSearch[]="Begin";
char *tmp;

tmp = strstr(TextString,StringToSearch);
if (tmp != NULL)
printf("word is presnt in the string\n");
else
printf("word is not presnt in the string\n");

return 0;
}