/*! wALT.c
 *
 *   file contains function declarations for testing ALT keys
 */

#include "wsys.h"

int ALT (int val) {

	return (
		(128 + (int) *( (val) - 65 +
	/*  a   b   c   d   e   f   g   h   i   j   k   l   m */
	"\x1E\x30\x2E\x20\x12\x21\x22\x23\x17\x24\x25\x26\x32"

	/*  n   o   p   q   r   s   t   u   v   w   x   y   z */
	"\x31\x18\x19\x10\x13\x1F\x14\x16\x2F\x11\x2d\x15\x2C"
				) )
		);
	} /*end ALT*/







int unALT (int val)
	{
	return (

   (    ((val) >= 0x10 +128 && ((val) <= 0x19 +128)) ?
		/*top row*/  *( ((val)-0x10 -128) + "QWERTYUIOP" )
	:  (	((val) >= 0x1E +128 && ((val) <= 0x26 +128))  ?
	   /*2nd row*/  *( ((val)-0x1E -128) + "ASDFGHJKL" )
		      : (   ((val) >= 0x2C +128 && ((val) <= 0x32 +128))  ?
		      /*3rd row*/  *( ((val)-0x2C -128) + "ZXCVBNM" )
				 : /*error*/ 0
			)
	   )
   )
		);
	} /*end unALT*/

