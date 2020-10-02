                                        /* Chapter 10 - Program 1 */
#include "stdio.h"
#include "string.h"

void main()
{
FILE *fp;
char stuff[25];
int index;

   fp = fopen("TENLINES.TXT","w");   /* open for writing */
   strcpy(stuff,"This is an example line.");

   for (index = 1;index <= 10;index++)
      fprintf(fp,"%s  Line number %d\n",stuff,index);

   fclose(fp);    /* close the file before ending program */
}



/* Result of execution

(The following is written to the file named TENLINES.TXT)

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

*/
