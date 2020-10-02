                                         /* Chapter 6 - Program 3 */
#include "stdio.h"

void main()
{
enum {win,tie,bye,lose,no_show} result;
enum {sun,mon,tues,wed,thur,fri,sat} days;

   result = win;
   printf("    win = %d\n",result);
   result = lose;
   printf("   lose = %d\n",result);
   result = tie;
   printf("    tie = %d\n",result);
   result = bye;
   printf("    bye = %d\n",result);
   result = no_show;
   printf("no show = %d\n\n",result);

   for(days = mon;days < fri;days++)
      printf("The day code is %d\n",days);
}



/* Result of execution
    win = 0
   lose = 3
    tie = 1
    bye = 2
no show = 4

The day code is 1
The day code is 2
The day code is 3
The day code is 4

*/
