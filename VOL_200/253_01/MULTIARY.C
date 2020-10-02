                                           /* Chapter 7 - Program 6 */
#include "stdio.h"

void main()
{
int i,j;
int big[8][8],large[25][12];

   for (i = 0;i < 8;i++)
      for (j = 0;j < 8;j++)
         big[i][j] = i * j;       /* This is a multiplication table */

   for (i = 0;i < 25;i++)
      for (j = 0;j < 12;j++)
         large[i][j] = i + j;           /* This is an addition table */

   big[2][6] = large[24][10]*22;
   big[2][2] = 5;
   big[big[2][2]][big[2][2]] = 177;     /* this is big[5][5] = 177; */

   for (i = 0;i < 8;i++) {
      for (j = 0;j < 8;j++)
         printf("%5d ",big[i][j]);
      printf("\n");               /* newline for each increase in i */
   }
}




/* Result of execution

    0     0     0     0     0     0     0     0
    0     1     2     3     4     5     6     7
    0     2     5     6     8    10   748    14
    0     3     6     9    12    15    18    21
    0     4     8    12    16    20    24    28
    0     5    10    15    20   177    30    35
    0     6    12    18    24    30    36    42
    0     7    14    21    28    35    42    49

*/
