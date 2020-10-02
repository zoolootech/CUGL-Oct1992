/*
	gpr.c				created: 01-Oct-83

	general purpose utility routines for use with C to simplify
	user/program interaction.

	by Anthony Skjellum

	Copyright 1983, 1984 (c) Caltech.  All rights reserved.
	This subroutine library may be distributed freely and
	used for all non-commercial purposes but may not be sold.

	Routines:

		iinp():  integer input w/ prompt, range check + retry
		finp():  float   input w/ prompt, range check + retry
		sinp():  string  input w/ prompt
		cinq():  yes/no question processor w/ prompt + retry
		display(): display a file 

	updated: 20-Jul-84

	testing information: this code was tested with Aztec C 1.05j
*/

#include <stdio.h>

/* Unix flag (used by display()) */
/*
#define UNIX 1		
*/
#ifndef UNIX
#define TEOF	26	/* ^Z */
#endif

/* general purpose subroutines: */

/* iinp(): integer input with range checking, prompt and retry */

iinp(prompt,cflag,low,high)
char *prompt;		
char cflag;
int low;
int high;
{
	int ival;

	while(1)
	{
		printf("%s",prompt);
		if(scanf("%d",&ival) < 1)
			while(getchar() != '\n')
				;

		if((!cflag)||(ival >= low)&&(ival <= high)) 
			break;
				/* no checking, or within bounds */

		printf("\nValue out of range, try again...\n");
	}

	return(ival); /* return the value */

} /* end iinp() */


/* finp(): floating point input with range checking, prompt and retry */

double finp(prompt,cflag,low,high)
char *prompt;
char cflag;
double low;
double high;
{
	double fval;

	while(1)
	{
		printf("%s",prompt);
		while(scanf("%lf",&fval) < 1)
			while(getchar() != '\n')
				;

		if((!cflag)||(fval >= low)&&(fval <= high)) 
			break;
				/* no checking, or within bounds */

		printf("\nValue out of range, try again...\n");
	}

	return(fval); /* return the value */

} /* end finp() */

/* subroutine sinp(): input a string with prompt, length limit */

sinp(prompt,string,length)
char *prompt;
char *string;
int length;
{
	int len;			/* length of actual string input */
	char chr;
	char *fgets();			/* string input function */

	printf("%s",prompt);			/* display the prompt */
	while(isspace(chr = getchar()))
		;
	ungetc(chr,stdin);
	fgets(string,length,stdin);	/* input the string */	
	
	if((len = strlen(string)))
		string[strlen(string)-1] = '\0';

	return(len);
} /* end sinp() */

/* subroutine cinq(): yes no question processor with prompt, retry  */

cinq(prompt)
char *prompt;
{
	char chr;

	while(1)
	{
		printf("%s",prompt);

		do /* drain spurious 'white space' */
		{
			chr = tolower(getchar()); /* use first char */
		} 
		while(isspace(chr));

		if((chr == 'y')||(chr == 'n')) break;

		printf("\nRespond with Y or N, please try again...\n");
	}

	return((chr == 'y') ? 1 : 0);
}


/* display(): subroutine to print an ascii file on the console */

display(fname)
char *fname;
{
	char c;		/* character to output */
	FILE *disp;

	if((disp = fopen(fname,"r")) == NULL)
		return(-1);	/* can't open file */

	while((c = getc(disp)) != EOF) /* print the file */
	{
#ifndef UNIX
		if(c == TEOF) /* text end of file */
			break;
#endif
	
		putchar((c & 127));    /* output each character less parity */
	}

	fclose(disp);	/* close the file */
	return(0);	/* successful completion */
}

