/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_COPY.C   ***************************/

#include "mydef.h"


/*****************************************************************

 Usage: void copy (char *from,char *to,int first,int length);

  char *from  = string to copy from.
  char *to    = string to copy to.
  int first   = position within string to start copying.
  int length  = number of character to copy.

  Copies a section of text, beginning at position "first" in
  string "from" and copies "length" number to string "to".
  (zero based counting, begins at zero not one)

  Example: copy (&to "test",1,2);
           results: to = "es"

*****************************************************************/

void copy (char *from,char *to,int first,int length)
{
int i;
 if ( (first <0) ) return;  /*  invalid number */

 /* if attempt made to copy beyond end of string then adjust*/
 if((first+length+1 ) > strlen(from))length=strlen(from)-first ;

  for(i=0;i<length;i++)
   to[i]= from[(first)+i];
  to[i]='\0';
}
