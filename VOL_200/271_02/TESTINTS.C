/*
**          TEST PROGRAM FOR INTERRUPT HANDLER FUNCTIONS
**  This program demonstrates the installable interrupt
**  service routines contained in the ticker functions.
**  The ticker routine installs on
**  interrupt 1CH, and chains after completion to whatever
**  routine may have already been hung on that interrupt.
**  Ticker is not a C function as such, but a stand-alone assembly
**  language routine which is installed by installtick() and removed
**  by removetick().
**
**  Copyright 1987, S.E. Margison
**
*/

#include <stdio.h>
#include <dos.h>
#include <smdefs.h>


int count;       /* this is the variable which ticker will manipulate */

main() {
   int i;

/* Use installtick() to pass the desired variable address to the ticker
** process and install it upon the interrupt.
*/
   installtick(&count);


/* this loop demonstrates the use of the ticker function.  As long as
** the count variable is not 0, it will be decremented 18.21 times
** per second.  ticker tests the value for zero and if it is,
** does not decrement it again.  This makes it unnecessary to try to
** "trap" count exactly at zero before the next interrupt occurs.
** To use ticker, place a value in count (18 = 1 second more or less).
** Then, check it every so often to see if it is zero (or whatever
** your heart desires).
*/

   i = 0;
   for ever {
      if(kbhit()) {
         if(getch() is 0x1b) break;  /* exit on ESCape */
         }
      count = 18;            /* set a value of 1 second */
      while(count isnot 0);     /* loop until timed out */
      printf("This is loop #%d\n", i++);  /* do something useful here */
      }                      /* and go again */


/* now, we have to clean up after ourselves.  It is necessary
** to restore the original vector table contents.  Alternate
** option is to turn off the power!!
*/
	removetick();


   }


