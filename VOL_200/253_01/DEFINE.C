					/* Chapter 6 - Program 1 */
#include "stdio.h"

#define START  0  /* Starting point of loop */
#define ENDING 9  /* Ending point of loop */
#define MAX(A,B)  ((A)>(B)?(A):(B))  /* Max macro definition */
#define MIN(A,B)  ((A)>(B)?(B):(A))  /* Min macro definition */

void main()
{
int index,mn,mx;
int count = 5;

   for (index = START;index <= ENDING;index++) {
      mx = MAX(index,count);
      mn = MIN(index,count);
      printf("Max is %d and min is %d\n",mx,mn);
   }
}



/* Result of execution

Max is 5 and min is 0
Max is 5 and min is 1
Max is 5 and min is 2
Max is 5 and min is 3
Max is 5 and min is 4
Max is 5 and min is 5
Max is 6 and min is 5
Max is 7 and min is 5
Max is 8 and min is 5
Max is 9 and min is 5

*/
