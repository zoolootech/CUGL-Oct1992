/************************************************************************
*									*
*	lsup.c				created: 25-Mar-84		*
*									*
*	long pointer support for small memory model 8086 C compilers.	*
*									*
*	version 1.00 as of 25-Mar-84					*
*									*
*	Copyright 1984 (c) Anthony Skjellum.				*
*	All rights reserved.  						*
*									*
*	This program may be freely distributed for all non-commercial	*
*	purposes, but may not be sold.					*
*									*
*	The routines contained here are designed to be portable to	*
*	a large variety of compilers.  Currently they have been tested	*
*	with Aztec C86 v 1.05i only.					*
*									*
*	Modules comprising this package:				*
*									*
*		lsup.c		this file.				*
*		lsup.h		header/definition file.			*
*		_lsup.h		lower level header for this file	*
*		llsup.asm	assembly language support (compiler	*
*				independent)				*
*		llint.asm	compiler interface code   (compiler	*
*				dependent)				*
*									*
*	Subroutines included here:					*
*	(those marked with an asterisk are only included if compiler	*
*	 used lacks some preprocessor support feature)			*
*									*
*									*
*									*
************************************************************************/

#include "_lsup.h"		/* header with definitions */

/*
	Special routines: Included only if compiler lacks one of
	several features.
*/

/* lassign(dest,source): assignment of type LPTR to the left */

#ifndef	MSUBST

lassign(dest,source)
LPTR dest;
LPTR source;
{
	dest._llong = source._llong;	/* assignment */
}

#endif

/*
	General purpose routines:
*/

/* llstrcpy(dest,src):	copy null terminated strings between long ptrs */

llstrcpy(dest,src)
LPTR *dest;
LPTR *src;
{
	char chr;			/* temporary */

	while(1)			/* loop */
	{
		chr = lchr(&src);	/* get   a character */
		l_stchr(&dest,chr);	/* store a character */

		linc(&dest);		/* increment destination ptr */
		linc(&src);		/* and source pointer */

		if(!chr)		/* we are done at eos */
			break;
	}

}

/* debugging routines: */

lprint(lptr)
LPTR *lptr;
{
	printf("%lx",lptr->_llong);
}



