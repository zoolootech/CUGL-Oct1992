/*-
 * cut - a recreation of the Unix(Tm) cut(1) command.
 *
 * syntax:  cut -cLIST[ file1 ...]
 *		cut -fLIST [-d char][ -s][ file1 ...]
 *
 *	Copyright (C) 1984 by David M. Ihnat
 *
 * This program is a total rewrite of the Bell Laboratories Unix(Tm)
 * command of the same name, as of System V.  It contains no proprietary
 * code, and therefore may be used without violation of any proprietary
 * agreements whatsoever.  However, you will notice that the program is
 * copyrighted by me.  This is to assure the program does *not* fall
 * into the public domain.  Thus, I may specify just what I am now:
 * This program may be freely copied and distributed, provided this notice
 * remains; it may not be sold for profit without express written consent of
 * the author.
 * Please note that I recreated the behavior of the Unix(Tm) 'cut' command
 * as faithfully as possible; however, I haven't run a full set of regression
 * tests.  Thus, the user of this program accepts full responsibility for any
 * effects or loss; in particular, the author is not responsible for any losses, * explicit or incidental, that may be incurred through use of this program.
 *
 * I ask that any bugs (and, if possible, fixes) be reported to me when
 * possible.  -David Ihnat (312) 784-4544 ihuxx!ignatz
**
**	Roberto Artigas Jr
**	P.O. Box 281415
**	Memphis, TN 38168-1415
**	work: 901-762-6092
**	home: 901-373-4738
**
**	1988.12.08 - Get to run under OS/2.
**		Used C/2 version 1.10 under OS/2 E 1.1
**		cl -c -AL cut.c
**		link cut,/noi,cut,llibce+os2, ;
**
**	Worked out includes
**	Fixed command line processing problem
**+
*/
#define	CPM	0
#define	DOS	0
#define	OS2	1

#include	<stdio.h>

#if	CPM
extern int errno;
#endif

#if	CPM
#define	_MAXSZ		512
#else
#define	_MAXSZ		(BUFSIZ*4)
#endif

#if	DOS || OS2
#include	<ctype.h>
#include	<stdlib.h>
#include	<string.h>
#endif

/* I'd love to use enums, but not everyone has them.  Portability, y'know. */
#define BADLIST		1
#define NODELIM		2
#define NOFIELDS	3
#define USAGE		4
#define BADFILE		5
#define BACKERR		6
#define TOOLONG		7

#define	TAB	'\t'
#define BACKSP	0x8
#define COMMAND "cut"

#define	IGNOREIT	0
#define CUTIT		1

char outbuf[_MAXSZ];		       /* Processed output buffer */
char rawbuf[_MAXSZ];		       /* Raw holding buffer for field mode */
#define	FLDFLAG	fields[0]
short int fields[_MAXSZ];	       /* Max number of fields or line
				        * length */

char *cmdnam;

short int cflag,
 fflag,
 sflag;
char delim = TAB;

/*
 * Function prototypes 
 */
void dofile();
void prerr();
int setflds();

/*
 * Begin program here 
 */
int 
main(argc, argv)
int argc;
char **argv;
{
    FILE *fileptr;
    FILE *fopen();
    int filecnt;

    cflag = fflag = sflag = 0;

#if	CPM
    cmdnam = COMMAND;
#else
    cmdnam = *argv;
#endif

    /* Skip invocation name */
    argv++;
    argc--;

    if (!argc)
	prerr(USAGE, NULL);

    /* Most compilers initialize storage to zero; but don't count on it. */

    for (filecnt = 0; filecnt < _MAXSZ; filecnt++)
	fields[filecnt] = IGNOREIT;

    /* First, parse input options */

    while (argv[0] && argv[0][0] == '-')
    {
	switch (argv[0][1])
	{
	case 'c':
	case 'C':
	    /* Build the character position list */
	    if (fflag || cflag)
		prerr(USAGE, NULL);
	    else
	    {
		cflag++;
		setflds(&argv[0][2]);
	    }
	    break;

	case 'f':
	case 'F':
	    /* Build the field position list */
	    if (fflag || cflag)
		prerr(USAGE, NULL);
	    else
	    {
		fflag++;
		setflds(&argv[0][2]);
	    }
	    break;

	case 'd':
	case 'D':
	    /* New delimiter */
	    delim = argv[0][2];
	    if (delim == '\0')
		prerr(NODELIM, NULL);

	    break;

	case 's':
	case 'S':
	    sflag++;
	    break;

	default:
	    prerr(USAGE, NULL);
	}
	argv++;
	argc--;
    }

    /* Finished all setup.  If no fields selected, tell them and exit. */
    if (!(cflag | fflag))
	prerr(BADLIST, NULL);

    if (!FLDFLAG)
	prerr(NOFIELDS, NULL);

    /*
     * If no files specified, process stdin.  Otherwise, process on a
     * file-by-file basis. 
     */
    if (argc == 0)
	dofile(stdin);
    else
	for (filecnt = 0; filecnt < argc; filecnt++, argv++)
#if	DOS || OS2
	    if ((fileptr = fopen(argv[0], "rb")) == (FILE *) NULL)
#else
	    if ((fileptr = fopen(argv[0], "r")) == (FILE *) NULL)
#endif
		prerr(BADFILE, argv);
	    else
	    {
		dofile(fileptr);
		fclose(fileptr);
	    }
    return (0);
}

int 
setflds(fldstr)
char *fldstr;
{
    /*
     * The string, character or field, must have one of the following
     * formats: 
     *
     * n n,m[,...]	where n<m a-b		where a<b -n,m		where n<m;
     * implies 1-n n-		where - implies to end of line or last field 
     */
    int index,
     minflag,
     value,
     fldset;

    minflag = 0;
    value = 0;
    index = 1;
    FLDFLAG = 0;

    for (;;)
    {
	switch (*fldstr)
	{
	case '-':
	    /* Starting a range */
	    if (minflag)
		prerr(BADLIST, NULL);
	    minflag++;
	    fldstr++;

	    if (value)
	    {
		if (value >= _MAXSZ)
		    prerr(BADLIST, NULL);

		index = value;
	    } else
		index = 1;

	    value = 0;
	    break;

	case ',':
	case '\0':
	    /* Ending the string, or this field/column sublist */
	    if (minflag)	       /* Some damnable range */
	    {			       /* Ranges are nasty.  Possibles:
				        * -n,a-n,n-.  In any case, index
				        * contains the start of the range. */
		if (!value)
		{		       /* From index to EOL */

		    FLDFLAG = index;
		    fldset++;
		    value = 0;
		} else
		{
		    if (value >= _MAXSZ)
			prerr(BADLIST, NULL);

		    if (value < index)
			prerr(BADLIST, NULL);

		    /* Already a TOEOL sequence? */
		    if (FLDFLAG)
		    {
			/*
			 * Yes.  Now...is the new sequence already contained
			 * by the old one? If so, no processing is
			 * necessary. 
			 */
			if (FLDFLAG > index)
			{
			    /*
			     * No, the new sequence starts before the old.
			     * Does the range extend into the current EOL
			     * range? If so, simply move the EOL marker. 
			     */
			    if (FLDFLAG < value)
			    {
				FLDFLAG = index;
			    } else
				/* Simple range. Fill it. */
				for (; index <= value; index++)
				    fields[index] = CUTIT;

			    /* In any case, some fields were selected. */
			    fldset++;
			}
		    } else	       /* Ok, no TOEOL sequence */
		    {
			for (; index <= value; index++)
			{
			    fields[index] = CUTIT;
			}
			fldset++;
		    }
		    value = 0;
		}
		minflag = 0;	       /* Reset the field-in-progress flag. */
	    } else
	    if (value)
	    {
		if (value >= _MAXSZ)
		    prerr(BADLIST, NULL);

		fields[value] = CUTIT;
		value = 0;
		fldset++;
	    }
	    if (*fldstr == '\0')
	    {
		/*
		 * Last bit of processing.  If there was an EOL, fill the
		 * array from the EOL point.  In any case, if there were any
		 * fields selected, leave the FLDFLAG value non-zero on
		 * return. 
		 */
		if (FLDFLAG)
		    for (index = FLDFLAG; index < _MAXSZ; index++)
			fields[index] = CUTIT;

		if (fldset)
		    FLDFLAG = 1;

		return (0);
	    }
	    fldstr++;
	    break;

	default:
	    if ((*fldstr < '0') || (*fldstr > '9'))
		prerr(BADLIST, NULL);

	    else
	    {
		value = 10 * value + *fldstr - '0';
		fldstr++;
	    }
	}
    }
}

void 
dofile(fno)
FILE *fno;
{
    /*
     * This will process the input files according to the rules specified in
     * the fields array. 
     */

    int charcnt,
     poscnt,
     bflag,
     doneflag,
     fldfound;
    register int c;

    char *inbufptr,
    *rawbufptr;

    do
    {
	inbufptr = outbuf;
	rawbufptr = rawbuf;
	charcnt = bflag = doneflag = fldfound = 0;
	poscnt = 1;

	do
	{
	    c = fgetc(fno);
	    if (c == EOF)
	    {
		/* That's it for this file or stream */
		doneflag++;
		break;
	    }
	    if (cflag)
	    {
		/*
		 * In character scan mode.  Look to see if it's an
		 * NROFF-type underlined character; if so, then don't count
		 * the backspace. Notice that this could cause a buffer
		 * overflow in the worst case situation... but that's MOST
		 * unlikely. 
		 */

		if (c == BACKSP)
		{
		    if (bflag)
			prerr(BACKERR);
		    else
		    {
			bflag++;
			*inbufptr++ = (char) c;
		    }
		} else
		{
		    /*
		     * Valid character.  If it's to be sent, stow it in the
		     * outbuffer. 
		     */
		    bflag = 0;

		    if (++charcnt == (_MAXSZ - 1))
			prerr(TOOLONG);

		    if (fields[charcnt] && (c != '\n'))
			*inbufptr++ = (char) c;
		}
	    } else
	    {
		/*
		 * Field processing.  In this case, charcnt does indicate
		 * processed characters on the current line, but that is
		 * all.  Notice that ALL characters are initially stowed in
		 * the raw  buffer, until at least one field has been found. 
		 */
		if (fields[poscnt])
		{
		    /*
		     * Ok, working on a field.  It, and its terminating
		     * delimiter, go only into the processed buffer. 
		     */
		    fldfound = 1;
		    if (c != '\n')
			*inbufptr++ = (char) c;
		} else
		if (!fldfound)
		{
		    charcnt++;
		    if (c != '\n')
			*rawbufptr++ = (char) c;
		}
		/*
		 * In any case, if a delimiter, bump the field indicator. 
		 */
		if (c == delim)
		    poscnt++;
	    }
	} while (c != '\n');

	if ((cflag && charcnt) || (fflag && fldfound))
	{
	    /*
	     * No matter what mode, something was found. Print it. 
	     */

	    if (fflag && (*(inbufptr - 1) == delim))
		--inbufptr;	       /* Supress trailing delimiter */

	    *inbufptr = '\0';	       /* But null-terminate the line. */
	    puts(outbuf);
	} else
	if ((fflag && (!sflag)) && charcnt)
	{
	    /*
	     * In this case, a line with some characters, no delimiters, and
	     * no supression.  Print it. 
	     */

	    *rawbufptr = '\0';
	    puts(rawbuf);
	}
    } while (!doneflag);
}

void 
prerr(etype, estring)
int etype;
char *estring;
{
    switch (etype)
    {
    case BADLIST:
	fprintf(stderr, "%s : bad list for c/f option\n", cmdnam);
	break;

    case USAGE:
	fprintf(stderr, "Usage: %s [-s] [-d<char>] {-c<list> | -f<list>} [file ...]\n", cmdnam);
	break;

    case NOFIELDS:
	fprintf(stderr, "%s : no fields\n", cmdnam);
	break;

    case NODELIM:
	fprintf(stderr, "%s : no delimiter\n", cmdnam);
	break;

    case BADFILE:
	fprintf(stderr, "Cannot open: %s : %s\n", cmdnam, estring);
	break;

    case BACKERR:
	fprintf(stderr, "%s : cannot handle multiple adjacent backspaces\n", cmdnam);
	break;

    case TOOLONG:
	fprintf(stderr, "%s : line too long\n", cmdnam);
    }
    exit(2);
}
