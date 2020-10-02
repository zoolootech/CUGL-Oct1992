                                        /* Chapter 8 - Program 3 */
#include "stdio.h"
void fixup(int nuts, int *fruit);

void main()
{
int pecans,apples;

   pecans = 100;
   apples = 101;
   printf("The starting values are %d %d\n",pecans,apples);

				/* when we call "fixup"          */
   fixup(pecans,&apples);       /* we take the value of pecans   */
				/* we take the address of apples */

   printf("The ending values are %d %d\n",pecans,apples);
}

void fixup(int nuts, int *fruit)   /* nuts is an integer value   */
				   /* fruit points to an integer */
{
   printf("The values are %d %d\n",nuts,*fruit);
   nuts = 135;
   *fruit = 172;
   printf("The values are %d %d\n",nuts,*fruit);
}



/* Result of execution

The starting values are 100 101
The values are 100 101
The values are 135 172
The ending values are 100 172

*/
