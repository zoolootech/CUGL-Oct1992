                                         /* Chapter 11 - Program 7 */
#include "stdio.h"

void main()
{
union {
   int index;
   struct {
      unsigned int x : 1;
      unsigned int y : 2;
      unsigned int z : 2;
   } bits;
} number;

   for (number.index = 0;number.index < 20;number.index++) {
      printf("index = %3d, bits = %3d%3d%3d\n",number.index,
              number.bits.z,number.bits.y,number.bits.x);
   }
}



/* Result of execution

index =   0, bits =   0  0  0
index =   1, bits =   0  0  1
index =   2, bits =   0  1  0
index =   3, bits =   0  1  1
index =   4, bits =   0  2  0
index =   5, bits =   0  2  1
index =   6, bits =   0  3  0
index =   7, bits =   0  3  1
index =   8, bits =   1  0  0
index =   9, bits =   1  0  1
index =  10, bits =   1  1  0
index =  11, bits =   1  1  1
index =  12, bits =   1  2  0
index =  13, bits =   1  2  1
index =  14, bits =   1  3  0
index =  15, bits =   1  3  1
index =  16, bits =   2  0  0
index =  17, bits =   2  0  1
index =  18, bits =   2  1  0
index =  19, bits =   2  1  1

*/
