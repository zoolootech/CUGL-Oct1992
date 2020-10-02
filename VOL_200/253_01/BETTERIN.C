                                        /* Chapter 9 - Program 3 */
#include "stdio.h"
#include "conio.h"
#define CR 13       /* this defines CR to be 13 */
#define LF 10       /* this defines LF to be 10 */

void main()
{
char c;

   printf("Input any characters, hit X to stop.\n");

   do {
      c = getch();                    /* get a character */
      putchar(c);                     /* display the hit key */
      if (c == CR) putchar(LF);       /* if it is a carriage return
                                         put out a linefeed too */
   } while (c != 'X');

   printf("\nEnd of program.\n");
}



/* Result of execution

Input any characters, hit X to stop.

(The output depends on what characters you enter.)

End of program.

*/
