/*
	env.c				created: 25-Mar-84

	This package echoes the environment to the standard output.

	example of using long pointers with lsup package.

	This is set-up to work with Aztec C.

	by Anthony Skjellum. (C) 1984. All rights reserved.
	Released for non-commercial purposes only.

	This program echoes the environment block to the console.
	In effect, this is the same as the DOS 2.0 SET command.
	Nevertheless, it illustrates the usefulness of long pointers.

	---------------------------------------------

	The following changes were made to the $begin
	routine of the Aztec C 1.05i module calldos.asm:

		i) a new global variable called envseg was created

			envseg_ segment word common 'data'
			$envdat dw 0
			$envseg dw ?
			envseg_ ends

		ii) On entry to $begin, when es contains the 
		    program segment prefix (PSP), es:[2ch] contains
		    the segment address of the environment.  This
		    segment address is stored into the second word
		    of envseg_ (ie $envseg).

		    The environment may now be referred to through
		    the external LPTR envseg.

	       iii) If DOS 2.0 allocation is to be used, be sure to
		    shrink the program size using the SETBLOCK function.
		    This must also be done in $begin where the psp,
		    ds, segments are both available.

*/

#include <stdio.h>
#include "lsup.h"	/* support for long pointers */

extern	LPTR envseg;	/* envseg is a structure of type LPTR */

main(argc,argv)
int argc;
char *argv[];
{
	char chr;
	int i;
	LPTR lptr;

	lassign(lptr,envseg); /* get long pointer to environment */

	while(1)	/* loop */
	{
		chr = lchr(&lptr);	/* get the next byte */

		if(!chr) /* we have hit the end of the environment */
			break;

		while((chr = lchr(&lptr))) /* get characters of string */
		{
			putchar(chr);	 /* write them to console */
			linc(&lptr);	 /* increment pointer */
		}

		linc(&lptr);	/* pass the zero byte just encountered */

		putchar('\n');  /* add new line between entries */

	} /* end while(1) */

}