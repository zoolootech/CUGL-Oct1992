/*
			clset.c
	FDC - I Clock Set program for Monitor version 4.0c
		Alan Coates, 1 Oct 1984
		Hi Tech C Compiler v 1.2
 */
#include <stdio.h>

#define SECADR   0xF7A9		/* Address given by RTSEC in FDC Monitor */

main()
	{
	char *sec, *min, *hr, *day, *mno,  *yr, tmp[20], *s;
	
	sec = (char *)SECADR; 		/* else HTC complains about	     */
	min = (char *)SECADR + 1; 	/* assigning an integer to a pointer */
	hr  = (char *)SECADR + 2; 
	yr  = (char *)SECADR + 3; 
	day = (char *)SECADR + 4; 
	mno = (char *)SECADR + 5; 
	printf("Enter date and time DDMMYYhhmmss NOT separated by spaces\n");
	printf("hit <cr> to set exact time:\t");
	scanf("%s", tmp);

/*	Use of a string with explicit ASCII decoding is to allow the
	assignment of decimal digit entry to the single byte locations
	used for the FDC-I real time clock - scanf %c wont take multiple
	digits to one location, and %d overflows the target memory to corrupt
	the next byte.  Code borrowed from Starpri etc.

	Note the initialization of clock locations to 0 just before
	assigning the entered value - otherwise sec and perhaps min
	would increment before the first *10 in each step.
 */
	for (*sec = 0,s = tmp + 10; s < tmp + 12 && isdigit(*s);)
		*sec = *sec*10 + *(s++) - 48;
	for (*day = 0,s = tmp; s < tmp + 2 && isdigit(*s);)
		*day = *day*10 + *(s++) - 48;
	for (*mno = 0,s = tmp + 2; s < tmp + 4 && isdigit(*s);)
		*mno = *mno*10 + *(s++) - 48;
	for (*yr = 0,s = tmp + 4; s < tmp + 6 && isdigit(*s);)
		*yr = *yr*10 + *(s++) - 48;
	for (*hr = 0,s = tmp + 6; s < tmp + 8 && isdigit(*s);)
		*hr = *hr*10 + *(s++) - 48;
	for (*min = 0,s = tmp + 8; s < tmp + 10 && isdigit(*s);)
		*min = *min*10 + *(s++) - 48;
}