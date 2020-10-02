

#include "ciao.h"

/*=============================*/
/* keyin()                     */
/*                             */
/* extended ASCII chars are    */
/* returned with high bits set */
/*=============================*/


int keyin( wait )  
void (* wait)();

{
     static int ch;


     do 
     { 
       (* wait)();   /* while waiting for input */
     } 
     while ( !kbhit() );


     ch = getch();
     if ( kbhit() && (ch == '\0'))  /* extended ASCII */
     {
           ch = getch();     /* read extended char (Fn key, Keypad, etc.) */
           ch |= 128;        /* set high bit to flag origin of this char  */
     }

     return( ch & 0xFF );  /* kill sign extension, return values 1..255 */
}

