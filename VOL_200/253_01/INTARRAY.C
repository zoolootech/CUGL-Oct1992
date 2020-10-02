                                          /* Chapter 7 - Program 3 */
#include "stdio.h"

void main()
{
int values[12];
int index;

   for (index = 0;index < 12;index++)
      values[index] = 2 * (index + 4);

   for (index = 0;index < 12;index++)
      printf("The value at index = %2d is %3d\n",index,values[index]);

}



/* Result of execution

The value at index =  0 is   8
The value at index =  1 is  10
The value at index =  2 is  12
The value at index =  3 is  14
The value at index =  4 is  16
The value at index =  5 is  18
The value at index =  6 is  20
The value at index =  7 is  22
The value at index =  8 is  24
The value at index =  9 is  26
The value at index = 10 is  28
The value at index = 11 is  30

*/
