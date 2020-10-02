/*
**	name:		dsfx2.h
**	purpose:	Temporary Suffix List for replacer
**	status:		Copyright (c) 1988 - 1988
**	author:		Roberto Artigas - All rights reserved
**	NOTES:		Beware the exponation of the search space
**			the more you add to this suffix table.
*/
char	*sfx2[] = {		/* legal common word suffixes */
	"est",
	"ing",
	"s"
};

#define	NSUFFIX2 sizeof(sfx2) / sizeof(char *)
