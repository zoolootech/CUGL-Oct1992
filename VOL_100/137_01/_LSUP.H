/************************************************************************
*									*
*	_lsup.h				created: 25-Mar-84		*
*									*
*	a component of lsup.c						*
*									*
*	version 1.00 as of 25-Mar-84					*
*									*
*	Copyright 1984 (c) Anthony Skjellum.				*
*	All rights reserved.  						*
*									*
*	This program may be freely distributed for all non-commercial	*
*	purposes, but may not be sold.					*
*									*
*	This is a header/definition file which must be included		*
*	in any module which utilizes long pointers.			*
*									*
************************************************************************/

/*
	compiler feature toggles:
	comment out any which don't apply to the compiler in use.
*/

#define	MSUBST			/* macro substitution supported */

/* typedefs */

typedef struct __lword
{
	unsigned _addr;		/* address */
	unsigned _segm; 	/* segment */
} LWORD;

typedef union __lptr
{
	long	_llong;		/* long format (for assignments) */
	char	_lstr[4];	/* character format */
	LWORD	_lword;		/* long-word format */
} LPTR;

/* constants */

/* macros */

/* lassign(destination,source): effect assignment of type LPTR */

#ifdef	MSUBST
#define	lassign(d,s)	d._llong = s._llong;
#endif

/* function specifications: */

char lchr();


