					 /* Chapter 5 - Program 5 */
#include "stdio.h"            /* Contains prototype for printf    */
void count_dn(int count);     /* Prototype for count_dn           */

void main()
{
int index;

   index = 8;
   count_dn(index);
}

void count_dn(int count)
{
   count--;
   printf("The value of the count is %d\n",count);
   if (count > 0)
      count_dn(count);
   printf("Now the count is %d\n",count);
}



/* Result of execution

The value of the count is 7
The value of the count is 6
The value of the count is 5
The value of the count is 4
The value of the count is 3
The value of the count is 2
The value of the count is 1
The value of the count is 0
Now the count is 0
Now the count is 1
Now the count is 2
Now the count is 3
Now the count is 4
Now the count is 5
Now the count is 6
Now the count is 7

*/
