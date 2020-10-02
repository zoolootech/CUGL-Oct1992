                                        /* Chapter 13 - Program 3 */
#include "stdio.h"

void main()
{
char mask;
char number[6];
char and,or,xor,inv,index;

   number[0] = 0X00;
   number[1] = 0X11;
   number[2] = 0X22;
   number[3] = 0X44;
   number[4] = 0X88;
   number[5] = 0XFF;

   printf(" nmbr  mask   and    or   xor   inv\n");
   mask = 0X0F;
   for (index = 0;index <= 5;index++) {
      and = mask & number[index];
      or = mask | number[index];
      xor = mask ^ number[index];
      inv = ~number[index];
      printf("%5x %5x %5x %5x %5x %5x\n",number[index],
              mask,and,or,xor,inv);
   }

   printf("\n");
   mask = 0X22;
   for (index = 0;index <= 5;index++) {
      and = mask & number[index];
      or = mask | number[index];
      xor = mask ^ number[index];
      inv = ~number[index];
      printf("%5x %5x %5x %5x %5x %5x\n",number[index],
              mask,and,or,xor,inv);
   }
}



/* Result of execution

  nmbr  mask   and    or   xor   inv
     0     f     0     f     f  ffff
    11     f     1    1f    1e  ffee
    22     f     2    2f    2d  ffdd
    44     f     4    4f    4b  ffbb
  ff88     f     8  ff8f  ff87    77
  ffff     f     f  ffff  fff0     0

     0    22     0    22    22  ffff
    11    22     0    33    33  ffee
    22    22    22    22     0  ffdd
    44    22     0    66    66  ffbb
  ff88    22     0  ffaa  ffaa    77
  ffff    22    22  ffff  ffdd     0

*/
