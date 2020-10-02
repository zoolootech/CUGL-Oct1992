                                         /* Chapter 11 - Program 5 */
#include "stdio.h"

void main()
{
union {
   int value;     /* This is the first part of the union */
   struct {
      char first;   /* These two values are the second     */
      char second;
   } half;
} number;

long index;

   for (index = 12;index < 300000L;index += 35231L) {
      number.value = index;
      printf("%8x %6x %6x\n",number.value, number.half.first,
              number.half.second);
   }
}



/* Result of execution

       c      c      0
    89ab   ffab   ff89
    134a     4a     13
    9ce9   ffe9   ff9c
    2688   ff88     26
    b027     27   ffb0
    39c6   ffc6     39
    c365     65   ffc3
    4d04      4     4d

*/
