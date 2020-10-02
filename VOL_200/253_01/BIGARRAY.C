                                         /* Chapter 7 - Program 4 */
#include "stdio.h"

char name1[] = "First Program Title";

void main()
{
int index;
int stuff[12];
float weird[12];
static char name2[] = "Second Program Title";

   for (index = 0;index < 12;index++) {
      stuff[index] = index + 10;
      weird[index] = 12.0 * (index + 7);
   }

   printf("%s\n",name1);
   printf("%s\n\n",name2);
   for (index = 0;index < 12;index++)
      printf("%5d %5d %10.3f\n",index,stuff[index],weird[index]);
}



/* Result of execution

First program title
Second program title

    0    10     84.000
    1    11     96.000
    2    12    108.000
    3    13    120.000
    4    14    132.000
    5    15    144.000
    6    16    156.000
    7    17    168.000
    8    18    180.000
    9    19    192.000
   10    20    204.000
   11    21    216.000

*/
