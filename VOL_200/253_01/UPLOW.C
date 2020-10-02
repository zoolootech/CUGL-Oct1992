                                        /* Chapter 13 - Program 1 */
#include "STDIO.H"
#include "ctype.h"      /* Note - your compiler may not need this */

void mix_up_the_chars(char line[]);

void main()
{
FILE *fp;
char line[80], filename[24];
char *c;

   printf("Enter filename -> ");
   scanf("%s",filename);
   fp = fopen(filename,"r");

   do {
      c = fgets(line,80,fp);   /* get a line of text */
      if (c != NULL) {
         mix_up_the_chars(line);
      }
   } while (c != NULL);

   fclose(fp);
}

void mix_up_the_chars(char line[])
			 /* This function turns all upper case
                            characters into lower case, and all
                            lower case to upper case. It ignores
                            all other characters.               */
{
int index;

   for (index = 0;line[index] != 0;index++) {
      if (isupper(line[index]))     /* 1 if upper case */
         line[index] = tolower(line[index]);
      else {
         if (islower(line[index]))  /* 1 if lower case */
            line[index] = toupper(line[index]);
      }
   }
   printf("%s",line);
}



/* Result of execution

(The selected file is displayed on the monitor with all of
  the upper case characters converted to lower case, and all
  of the lower case characters converted to upper case.)

*/
