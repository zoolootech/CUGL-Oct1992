/*
**  getchar()		Direct console input via BDOS(6)
*/

#include <stdio.h>

#asm
GETKBD::
#endasm

   static char c; 

getchar() {

   while(!(c = bdos(6,255)));
   return(c == 0x1a ? -1 : c);
}

