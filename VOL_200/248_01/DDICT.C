/*
**	name:		ddict.c
**	purpose:	Display compressed dictionary
**	status:		Copyright (c) 1988 - 1988
**	author:		Roberto Artigas Jr - All rights reserved
**	usage:		Non-commercial use, user supported software
**	NOTES:		This program just uncompresses and displays
**			the main dictionary. The next logical step
**			is to modify it to take to command line
**			parameters same as 'cdict'. I will leave
**			that to the next enterprising indivitual that
**			needs this particular function.
*/
#define	maindef

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"

int		mopen();
void		mclose();
char		*nxtmword();

int	main(argc,argv)
int	argc;
char	*argv[];
{
	char	*word;
	long	count = 0;

fprintf(stderr,"DDICT Display Dictionary Utility for MicroSPELL v1.00\n");
	fprintf(stderr,"[Dictionary display begin]\n");
	if (!mopen()) return(1);
	while ((word = nxtmword()) != hivalue) {
		fprintf(stdout,"%s\n",word);
		count++;
		if (count % 100 == 0) {
			fprintf(stderr,"[%ld words output]\n",count);
		}
	}
	mclose();
	fprintf(stderr,"[%ld words output]\n",count);
	fprintf(stderr,"[Dictionary display ended]\n");
	return(0);
}
