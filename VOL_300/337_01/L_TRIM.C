/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_TRIM.C   ***************************/

#include "mydef.h"


/*****************************************************************

 Usage: void make_string(char *string,char letter,int count);

  char *string = pointer to string to build.
  char letter  = letter to replicate.
  int count    = length of string.


  Builds a string consisting only of the character specified
  by "ch".  The string is the length specified by "count".

  Example: make_string (string,'x',10);
           results: string = "xxxxxxxxxx"

*****************************************************************/

void make_string(char *string,char letter,int count)
{
int j;
for (j=0;j<count;j++) string[j]=letter;
string[j]='\0';
}


/*****************************************************************

 Usage: void trim_left(char *string)

  char *string = string to strip of left spaces.

  Removes all the leftmost spaces from the string specified by
  "string".

*****************************************************************/

void trim_left(char *string)
{
int i=0;
int j=0;

if (!strlen(string)) return;

 while(string[i]== ' ' && string[i] != '\0') i++;

 while (string[i] != '\0')
  string[j++]= string[i++];

 string[j]='\0';
}


/*****************************************************************

 Usage: void trim_right(char *string);

  char *string = string to strip of right spaces.

  Removes all the rightmost spaces from the string specified by
  "string".

*****************************************************************/

void trim_right(char *string)
{
int pos;
pos = strlen(string)-1;
  while(string[pos]==' '){
   string[pos--]= '\0';
   if (pos < 0) break;
  }
}


/*****************************************************************

 Usage: void trim(char *string)

  char *string = string to strip of left and right spaces.

  Removes all the leftmost and rightmost spaces from the string
  specified by "string".

*****************************************************************/

void trim(char *string)
{
trim_left(string);
trim_right(string);
}
