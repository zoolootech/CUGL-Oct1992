/*
			clock.c	
	FDC - I Clock for Monitor version 4.0c
		Alan Coates, 1 Oct 1984
		Hi Tech C Compiler

	FDC-I stores a real time clock, which is not battery backed up,
	at a location SEC which can be located from the monitor using 
	the monitor routine RTSEC.  The location of RTSEC varies with the 
	version of the monitor. Version 4.0c has a base page of F8, and 
	locates RTSEC at F839H.  This in turn gives SEC at F7A9H.  Rather
	than look this all up each time the program is run, I define it
	as SECADR below.  If a different monitor version is used, you can
	get the address of SEC using DDT to examine the location jumped
	to by the instruction at BP39.

 */
#include <stdio.h>	/* "libc.h" if using Aztec compiler	*/

/* Hazeltine screen control codes - octal values! CLS merely clears the screen
   using the (switch selectable) lead-in tilde plus FS, while CURPOS places 
   the cursor at x,y - the last two digits after lead-in, DC1.  The location 
   should not be in the display area of the clock, since the cursor spends
   most of each second here. Hence the \t in the printf format.
*/

#define CLS	"\176\034"
#define CURPOS	"\176\021\025\027"	

#define SECADR  0xF7A9		/* Address given by RTSEC in FDC Monitor */

static char *month[] =	{ "  ", "Jan", "Feb", "March", "April", "May", "June",
				"July", "Aug", "Sept", "Oct", "Nov", "Dec"};


main()
	{
	char lastsec, *sec, *min, *hr, *day, *mno,  *yr;

	sec = (char *)SECADR;	/* cast to avoid HiTech warning message! */
	min = sec + 1;		/* other locations as in FDC-I manual    */
	hr  = sec + 2;
	yr  = sec + 3;
	day = sec + 4;
	mno = sec + 5;
	printf("%s %s", CLS, CURPOS);	/* or wherever you use the clock */
	for (;;)			/* break out with ^C	*/
		{
		if (lastsec != *sec)	/* dont update unless secon changed */
		printf("\t%2d  %s  %04d\t %02d:%02d:%02d %s", *day, 
		month[*mno], 1900 + *yr, *hr, *min, lastsec=*sec, CURPOS);
	}
	exit(0);			
}
