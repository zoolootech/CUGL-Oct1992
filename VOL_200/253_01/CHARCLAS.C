                                         /* Chapter 13 - Program 2 */
#include "stdio.h"
#include "ctype.h"       /* Note - your compiler may not need this */

void count_the_data(char line[]);

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
         count_the_data(line);
      }
   } while (c != NULL);

   fclose(fp);
}

void count_the_data(char line[])
{
int whites, chars, digits;
int index;

   whites = chars = digits = 0;

   for (index = 0;line[index] != 0;index++) {
      if (isalpha(line[index]))   /* 1 if line[] is alphabetic  */
          chars++;
      if (isdigit(line[index]))   /* 1 if line[] is a digit     */
          digits++;
      if (isspace(line[index]))   /* 1 if line[] is blank, tab, */
          whites++;               /*           or newline       */
   }   /* end of counting loop */

   printf("%3d%3d%3d %s",whites,chars,digits,line);
}



/* Result of execution (This is a portion of the output, but the
          comments have been removed to allow this section to be
          included as one large comment.  This output assumes that
          CHARCLAS.C is selected as the input file.)

  1  0  0
 10 22  2    for (index = 0;line[index] != 0;index++) {
 18 36  1       if (isalpha(line[index]))
 11  5  0           chars++;
 22 32  1       if (isdigit(line[index]))
 11  6  0           digits++;
 18 34  1       if (isspace(line[index]))
 45 15  0           whites++;
 12 17  0    }
  1  0  0
  5 31  3    printf("%3d%3d%3d %s",whites,chars,digits,line);
  1  0  0 }

*/
