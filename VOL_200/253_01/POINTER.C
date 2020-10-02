                                         /* Chapter 8 - Program 1 */
#include "stdio.h"

void main()                        /* illustration of pointer use */
{
int index,*pt1,*pt2;

   index = 39;                             /* any numerical value */
   pt1 = &index;                          /* the address of index */
   pt2 = pt1;
   printf("The value is %d %d %d\n",index,*pt1,*pt2);
   *pt1 = 13;                  /* this changes the value of index */
   printf("The value is %d %d %d\n",index,*pt1,*pt2);
}



/* Result of execution

The value is 39 39 39
The value is 13 13 13

*/
