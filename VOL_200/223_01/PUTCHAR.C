#define NOCCARGC  /* no argument count passing */

#asm

PUTDISP::

#endasm

/*
* putchar()		Normal console output via BDOS(2)
*/
#include <stdio.h>

putchar(c) char c; {

	bdos(2,c);

	   if (c == CR)
		bdos(2,LF);
}

