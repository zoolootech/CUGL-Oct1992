/*
**	name:		replace.c
**	purpose:	Generate possible replacements for misspelled words
**	status:		Copyright (c) 1988 - 1988
**	author:		Roberto Artigas Jr - All rights reserved
**	usage:		Non-commercial use, user supported software
**	syntax:		replace badword
**	samples:	replace A		(test)
**			replace animel		(test)
*/
#define	maindef

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"
#include	"dsfx2.h"

int		matches();
char		*translate_case();
int		mopen();
void		mclose();
char		*nxtmword();

/*
**	name:		trymatch
**	purpose:	Try matching with normal algorithms first
**			then by applying the suffix table to the word
*/
int	trymatch(word,pattern,display)
char	*word;
char	*pattern;
char	*display;
{
	static	int	matched, i;
	static	char	hold[50];

	strcpy(hold,word);
	matched = matches(word,pattern);
	if (matched) {
		fprintf(stdout,"%2d %s\n",matched,display);
		return(matched);
	}
/*
**	This technique is VERY time consuming. Adding a suffix to
**	the word and then going to the match algorithm again just
**	increases the time factor to an undesirable level.
**	THIS ROUTINE COMMENTED OUT -
*/
/* **********
	for (i=0; i < NSUFFIX2; i++) {
		strcpy(word,hold); strcat(word,sfx2[i]);
		matched = matches(word,pattern);
		if (matched) {
			fprintf(stdout,"%2d %s\n",matched,display);
			return(matched);
		}
	}
********** */
	return(0);
}

/*
**	purpose:	Begin program here
*/
int	main(argc,argv)
int	argc;
char	*argv[];
{
	FILE	*textin;
	char	*word, *dummy;
	char	cword[50];
	char	dword[50];
	char	eword[50];
	char	sword[50];
	int	length = 0;
	int	matched = 0;
	int	count = 0;

fprintf(stderr,"REPLACE Dictionary Utility for MicroSPELL v1.00\n");
	fprintf(stderr,"[Display posible words begin]\n");
	strcpy(sword,argv[1]); translate_case(sword);
/*
**	This area processes the common word list (common.txt)
*/
	fprintf(stderr,"[Common search]\n");
	textin = fopen(comlist,"r");
	if (!textin) {
		fprintf(stderr,"Can't open input file %s\n",comlist);
		return(1);
	}
	while (dummy = fgets(eword,sizeof(eword),textin)) {
		if (*eword > *sword) break;
		length = strlen(eword);
		if (eword[length-1] == '\n') eword[length-1] = '\0';
		strcpy(dword,eword); strcpy(cword,argv[1]);
		matched = trymatch(cword,dword,eword);
		if (matched) count++;
	}
	fclose(textin);
/*
**	This area processes the dictionary (dict.dct)
*/
	fprintf(stderr,"[Dictionary search]\n");
	if (!mopen()) return(1);
	while ((word = nxtmword()) != hivalue) {
		if (*word > *sword) break;
		strcpy(dword,word); strcpy(cword,argv[1]);
		matched = trymatch(cword,dword,word);
		if (matched) count++;
	}
	mclose();
	fprintf(stderr,"[%d posible words matched]\n",count);
	fprintf(stderr,"[Display posible words ended]\n");
	return(0);
}
