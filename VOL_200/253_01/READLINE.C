                                        /* Chapter 10 - Program 6 */
#include "stdio.h"

void main()
{
FILE *fp1;
char oneword[100];
char *c;

   fp1 = fopen("TENLINES.TXT","r");

   do {
      c = fgets(oneword,100,fp1); /* get one line from the file */
      if (c != NULL)
         printf("%s",oneword);    /* display it on the monitor  */
   } while (c != NULL);          /* repeat until NULL          */

   fclose(fp1);
}



/* Result of execution

This is an example line.  Line number 1
This is an example line.  Line number 2
This is an example line.  Line number 3
This is an example line.  Line number 4
This is an example line.  Line number 5
This is an example line.  Line number 6
This is an example line.  Line number 7
This is an example line.  Line number 8
This is an example line.  Line number 9
This is an example line.  Line number 10
Additional lines.
Additional lines.
Additional lines.
Additional lines.
Additional lines.
Additional lines.
Additional lines.
Additional lines.
Additional lines.
Additional lines.

*/
