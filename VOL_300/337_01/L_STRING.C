/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_STRING.C   ***************************/

#include "mydef.h"


/*****************************************************************

 Usage: int pos(char *string,char *pattern)

  char *string= string to search.

  char *pattern= text to search for.

  Returns an integer value representing the location of
  pattern within string.

  Example:  string= "this is a test"
            i=  pos(&string,'test');

            results: i=10

*****************************************************************/

int pos(char *string,char *pattern)

{
  int i,j,found=0;

   for(i=0;i<= strlen(string)-strlen(pattern);i++){

    /* find first location */
    if (toupper(string[i]) == toupper(pattern[0])){  
     found=1;

       for (j=1;j<=strlen(pattern)-1;j++){
          if (toupper(string[i+j]) != toupper(pattern[j])) {
           found=0;
           break;
          }
       }
    }
   if(found) return (i);
  }
return (-1);
}


/*****************************************************************

 Usage: int caps(char *string);

 char *string= string to make upper case

 Converts all letters in string to uppercase.

*****************************************************************/


/***CAPS***/
void caps(char *string)
{
 int i;

 for (i=0;string[i] != '\0';i++)  string[i]= toupper(string[i]);
}
