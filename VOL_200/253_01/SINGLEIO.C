                                         /* Chapter 9 - Program 2 */
#include <stdio.h>
#include <conio.h>

void main()
{
char c;

   printf("Enter any characters, terminate program with X\n");

   do {
      c = getch();                     /* get a character */
      putchar(c);                  /* display the hit key */
   } while (c != 'X');

   printf("\nEnd of program.\n");
}



/* Result of execution

Enter any characters, terminate program with X

(The output depends on the characters you type in.)

End of program.

*/
