                                          /* Chapter 7 - Program 5 */
#include "stdio.h"
void dosome(int list[]);

void main()
{
int index;
int matrix[20];

   for (index = 0;index < 20;index++)              /* generate data */
      matrix[index] = index + 1;

   for (index = 0;index < 5;index++)         /* print original data */
      printf("Start  matrix[%d] = %d\n",index,matrix[index]);

   dosome(matrix);              /* go to a function & modify matrix */

   for (index = 0;index < 5;index++)       /* print modified matrix */
      printf("Back   matrix[%d] = %d\n",index,matrix[index]);
}

void dosome(int list[])      /* This will illustrate returning data */
{
int i;

   for (i = 0;i < 5;i++)                   /* print original matrix */
      printf("Before matrix[%d] = %d\n",i,list[i]);

   for (i = 0;i < 20;i++)                   /* add 10 to all values */
      list[i] += 10;

   for (i = 0;i < 5;i++)                   /* print modified matrix */
      printf("After  matrix[%d] = %d\n",i,list[i]);
}



/* Result of execution

Start  matrix[0] = 1
Start  matrix[1] = 2
Start  matrix[2] = 3
Start  matrix[3] = 4
Start  matrix[4] = 5
Before matrix[0] = 1
Before matrix[1] = 2
Before matrix[2] = 3
Before matrix[3] = 4
Before matrix[4] = 5
After  matrix[0] = 11
After  matrix[1] = 12
After  matrix[2] = 13
After  matrix[3] = 14
After  matrix[4] = 15
Back   matrix[0] = 11
Back   matrix[1] = 12
Back   matrix[2] = 13
Back   matrix[3] = 14
Back   matrix[4] = 15

*/
