                                         /* Chapter 13 - Program 4 */
#include "stdio.h"

void main()
{
int small, big, index, count;

   printf("       shift left      shift right\n\n");
   small = 1;
   big = 0x4000;
   for(index = 0;index < 17;index++) {
      printf("%8d %8x %8d %8x\n",small,small,big,big);
      small = small << 1;
      big = big >> 1;
   }

   printf("\n");
   count = 2;
   small = 1;
   big = 0x4000;
   for(index = 0;index < 9;index++) {
      printf("%8d %8x %8d %8x\n",small,small,big,big);
      small = small << count;
      big = big >> count;
   }
}



/* Result of execution

       1       1   16384    4000
       2       2    8192    2000
       4       4    4096    1000
       8       8    2048     800
      16      10    1024     400
      32      20     512     200
      64      40     256     100
     128      80     128      80
     256     100      64      40
     512     200      32      20
    1024     400      16      10
    2048     800       8       8
    4096    1000       4       4
    8192    2000       2       2
   16384    4000       1       1
  -32768    8000       0       0
       0       0       0       0

       1       1   16384    4000
       4       4    4096    1000
      16      10    1024     400
      64      40     256     100
     256     100      64      40
    1024     400      16      10
    4096    1000       4       4
   16384    4000       1       1
       0       0       0       0

*/
