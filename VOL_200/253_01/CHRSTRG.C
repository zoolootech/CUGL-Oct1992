                                         /* Chapter 7 - Program 1 */
#include "stdio.h"

void main()
{
char name[5];       /* define a string of characters */

   name[0] = 'D';
   name[1] = 'a';
   name[2] = 'v';
   name[3] = 'e';
   name[4] = 0;     /* Null character - end of text */

   printf("The name is %s\n",name);
   printf("One letter is %c\n",name[2]);
   printf("Part of the name is %s\n",&name[1]);
}



/* Result of execution

The name is Dave
One letter is v
Part of the name is ave

*/
