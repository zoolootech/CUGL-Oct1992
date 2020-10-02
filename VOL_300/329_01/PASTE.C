/*-
 * paste - a recreation of the Unix(Tm) paste(1) command.
 *
 * syntax:	paste file1 file2 ...
 *		paste -dLIST file1 file2 ...
 *		paste -s [-dLIST] file1 file2 ...
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
 * Please note that I recreated the behavior of the Unix(Tm) 'paste' command
 * as faithfully as possible, with minor exceptions (noted below); however,
 * I haven't run a full set of regression * tests.  Thus, the user of
 * this program accepts full responsibility for any effects or loss;
 * in particular, the author is not responsible for any losses,
 * explicit or incidental, that may be incurred through use of this program.
 *
 * The changes to the program, with one exception, are transparent to
 * a user familiar with the Unix command of the same name.  These changes
 * are:
 *
 * 1) The '-s' option had a bug in the Unix version when used with multiple
 *    files.  (It would repeat each file in a list, i.e., for
 *    'paste -s file1 file2 file3', it would list
 *    <file1\n><file1\n><file2\n><file1\n><file2\n><file3\n>
 *    I fixed this, and reported the bug to the providers of the command in
 *    Unix.
 *
 * 2) The list of valid escape sequences has been expanded to include
 *    \b,\f,\r, and \v.  (Just because *I* can't imagine why you'd want
 *    to use them doesn't mean I should keep them from you.)
 *
 * 3) There is no longer any restriction on line length.
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
**	1988.12.08 - Changes to get this to work under OS/2
**		Used C/2 version 1.10 under OS/2 E 1.1
**		Commands where:
**		cl -c -AL paste.c
**		link /st:4000 paste,/noi,paste,llibce+os2, ;
**
**	Worked out includes
**	Fixed command line processing problem
**	Fixed delimiter string definition problem
**	Added delimiter string size checking
**
**+
*/
#define	CPM	0
#define	DOS	0
#define	OS2	1

#include <stdio.h>

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
#define NODELIM		1
#define USAGE		2
#define BADFILE		3
#define TOOMANY		4
#define	DELIMTOOLONG	5

#define	TAB		'\t'
#define	NL		'\n'
#define	BS		'\b'
#define	FF		'\f'
#define	CR		'\r'
#define	VT		'\v'
#define	DEL		'\177'

#define _MAXFILES	12
#define	CLOSED		((FILE *)-1)
#define ENDLIST		((FILE *)-2)
#define COMMAND		"paste"
#define	SZDELIM		31

char *cmdnam;

short int dflag,
 sflag;
char delims[SZDELIM] = {TAB, '\0'};    /* Delimiter STRING!!!! */

/*
 * Function prototypes 
 */
void prerr();
void delimbuild();
void docol();
void doserial();


/*-
**	name:		main
**	purpose:	Begin program here
**+
*/
int 
main(argc, argv)
int argc;
char *argv[];
{
    char *strcpy();

    dflag = sflag = 0;

#if	CPM
    cmdnam = COMMAND;
#else
    cmdnam = *argv;
#endif

    /* Skip invocation name */
    argv++;
    argc--;

    if (!argc)
	prerr(USAGE, "");

    /* First, parse input options */

    while (argv[0] && argv[0][0] == '-' && argv[0][1] != '\0')
    {
	switch (argv[0][1])
	{
	case 'd':
	case 'D':
	    /* Delimiter character(s) */
	    if (strlen(&argv[0][2]) > SZDELIM)
		prerr(DELIMTOOLONG,
		      (char *) &argv[0][2]);
	    if (*delims == '\0')
		prerr(NODELIM, "");
	    else
	    {
		strcpy(delims, &argv[0][2]);
		delimbuild(delims);
	    }

	    break;

	case 's':
	case 'S':
	    sflag++;
	    break;

	default:
	    prerr(USAGE, "");
	}
	argv++;
	argc--;
    }

    /*
     * If no files specified, simply exit.  Otherwise, if not the old '-s'
     * option, process all files. If '-s', then process files one-at-a-time. 
     */
    if (!sflag)
	docol(argc, argv);	       /* Column paste */
    else
	doserial(argc, argv);	       /* Serial paste */

    return (0);
}

void 
docol(nfiles, fnamptr)
int nfiles;
char **fnamptr;
{
    char iobuff[_MAXSZ];	       /* i/o buffer for the fgets */
    short int somedone;		       /* flag for blank field handling */

    /*
     * There is a strange case where all files are just ready to be closed,
     * or will on this round.  In that case, the string of delimiters must
     * be preserved.  delbuf[1] ->delbuf[MAXFILES+1] provides intermediate
     * storage for closed files, if needed; delbuf[0] is the current index. 
     */
    char delbuf[_MAXFILES + 2];

    FILE *fileptr[_MAXFILES + 1];
    FILE *fopen();

    char *fgets();

    int filecnt;		       /* Set to number of files to process */
    register char *delimptr;	       /* Cycling delimiter pointer */
    int index;			       /* Working variable */
    int strend;			       /* End of string in buffer */

    /*
     * Perform column paste.  First, attempt to open all files. (This could
     * be expanded to an infinite number of files, but at the (considerable)
     * expense of remembering the file and its current offset, then
     * opening/reading/closing.  The commands' utility doesn't warrant the
     * effort; at least, to me...) 
     */

    for (filecnt = 0; (nfiles > 0) && (filecnt < _MAXFILES); filecnt++, nfiles--, fnamptr++)
    {
	if (fnamptr[0][0] == '-')
	    fileptr[filecnt] = stdin;
	else
	{
#if	DOS || OS2
	    fileptr[filecnt] = fopen(*fnamptr, "rb");
#else
	    fileptr[filecnt] = fopen(*fnamptr, "r");
#endif
	    if (fileptr[filecnt] == NULL)
		prerr(BADFILE, *fnamptr);
	}
    }

    fileptr[filecnt] = ENDLIST;	       /* End of list. */

    if (nfiles)
	prerr(TOOMANY, "");

    /*
     * Have all files.  Now, read a line from each file, and output to
     * stdout.  Notice that the old 511 character limitation on the line
     * length no longer applies, since this program doesn't do the
     * buffering.  Do this until you go through the loop and don't
     * successfully read from any of the files. 
     */
    for (; filecnt;)
    {
	somedone = 0;		       /* Blank field handling flag */
	delimptr = delims;	       /* Start at beginning of delim list */
	delbuf[0] = 0;		       /* No squirreled delims */

	for (index = 0; (fileptr[index] != ENDLIST) && filecnt; index++)
	{
	    /*
	     * Read a line and immediately output. If it's too big for the
	     * buffer, then dump what was read and go back for more. 
	     *
	     * Otherwise, if it is from the last file, then leave the carriage
	     * return in place; if not, replace with a delimiter (if any) 
	     */

	    strend = 0;		       /* Set so can easily detect EOF */

	    if (fileptr[index] != CLOSED)

		while (fgets(iobuff, (_MAXSZ - 1), fileptr[index]) != (char *) NULL)
		{
		    strend = strlen(iobuff);	/* Did the buffer fill? */

		    if (strend == (_MAXSZ - 1))
		    {
			/* Gosh, what a long line. */
			fputs(iobuff, stdout);
			strend = 0;
			continue;
		    }
		    /* Ok got whole line in buffer. */
		    break;	       /* Out of loop for this file */
		}

	    /*
	     * Ended either on an EOF (well, actually NULL return-- it
	     * *could* be some sort of file error, but but if the file was
	     * opened successfully, this is unlikely. Besides, error
	     * checking on streams doesn't allow us to decide exactly what
	     * went wrong, so I'm going to be very Unix-like and ignore
	     * it!), or a closed file, or a received line. If an EOF, close
	     * the file and mark it in the list.  In any case, output the
	     * delimiter of choice. 
	     */

	    if (!strend)
	    {
		if (fileptr[index] != CLOSED)
		{
		    fclose(fileptr[index]);
		    fileptr[index] = CLOSED;
		    filecnt--;
		}
		/* Is this the end of the whole thing? */
		if ((fileptr[index + 1] == ENDLIST) && !somedone)
		    continue;	       /* EXITS */

		/* Ok, some files not closed this line. Last file? */
		if (fileptr[index + 1] == ENDLIST)
		{
		    if (delbuf[0])
		    {
			fputs(&delbuf[1], stdout);
			delbuf[0] = 0;
		    }
		    putc((int) NL, stdout);
		    continue;	       /* Next read of files */
		} else
		{
		    /* Closed file; setup delim */
		    if (*delimptr != DEL)
		    {
			delbuf[0]++;
			delbuf[delbuf[0]] = *delimptr++;
			delbuf[delbuf[0] + 1] = '\0';
		    } else
			delimptr++;
		}

		/* Reset end of delimiter string if necessary */
		if (*delimptr == '\0')
		    delimptr = delims;
	    } else
	    {
		/* Some data read. */
		somedone++;

		/* Any saved delims? */
		if (delbuf[0])
		{
		    fputs(&delbuf[1], stdout);
		    delbuf[0] = 0;
		}
		/* If last file, last char will be NL. */
		if (fileptr[index + 1] != ENDLIST)
		{
		    if (*delimptr == DEL)
		    {
			delimptr++;
			iobuff[strend - 1] = '\0';	/* No delim */
		    } else
			iobuff[strend - 1] = *delimptr++;
		}
		if (*delimptr == '\0')
		    delimptr = delims;

		/* Now dump the buffer */
		fputs(iobuff, stdout);
	    }
	}
    }
}

void 
doserial(nfiles, fnamptr)
int nfiles;
char **fnamptr;
{
    /*
     * Do serial paste.  Simply scarf characters, performing one-character
     * buffering to facilitate delim processing. 
     */

    register int charnew,
     charold;
    register char *delimptr;

    register FILE *fileptr;
    FILE *fopen();

    for (; nfiles; nfiles--, fnamptr++)
    {
	if (fnamptr[0][0] == '-')
	    fileptr = stdin;
	else
	{
#if	DOS || OS2
	    fileptr = fopen(*fnamptr, "rb");
#else
	    fileptr = fopen(*fnamptr, "r");
#endif

	    if (fileptr == NULL)
		prerr(BADFILE, *fnamptr);
	}

	/*
	 * The file is open; just keep taking characters, stashing them in
	 * charnew; output charold, converting to the appropriate delimiter
	 * character if needful.  After the EOF, simply output 'charold' if
	 * it's a newline; otherwise, output it and then a newline. 
	 */

	delimptr = delims;	       /* Set up for delimiter string */

	if ((charold = getc(fileptr)) == EOF)
	{
	    /* Empty file! */
	    putc(NL, stdout);
	    continue;		       /* Go on to the next file */
	}
	/* Ok, 'charold' is set up.  Hit it! */

	while ((charnew = getc(fileptr)) != EOF)
	{
	    /* Ok, process the old character */
	    if (charold == NL)
	    {
		if (*delimptr != DEL)
		    putc(*delimptr++, stdout);

		/* Reset pointer at end of delimiter string */
		if (*delimptr == '\0')
		    delimptr = delims;
	    } else
		putc((char) charold, stdout);

	    charold = charnew;
	}

	/* Ok, hit EOF.  Process that last character */

	putc((char) charold, stdout);

	if ((char) charold != NL)
	    putc(NL, stdout);
    }
}
/*-
**	name:		delimbuild
**	purpose:	Build a delimiter string
**+
*/
void 
delimbuild(strptr)
char *strptr;
{
    /*
     * Process the delimiter string into something that can be used by the
     * routines.  This involves, primarily, collapsing the backslash
     * representations of special characters into their actual values, and
     * terminating the string in a manner that the routines can recognize. 
     * The set of possible backslash characters has been expanded beyond
     * that recognized by the vanilla Unix(Tm) version. 
     */
    register char *strout;
    register int delimleft = SZDELIM;  /* Checking delimiter size */

    strout = strptr;		       /* Start at the same place, anyway */

    while (*strptr && (--delimleft))
    {
	if (*strptr != '\\')	       /* Is it an escape character? */
	    *strout++ = *strptr++;     /* No, just transfer it */
	else
	{
	    strptr++;		       /* Get past escape character */
	    switch (*strptr)
	    {
	    case '0':
		*strout++ = DEL;
		break;

#if	CPM || DOS || OS2
	    case 'T':
	    case 't':
#else
	    case 't':
#endif
		*strout++ = TAB;
		break;

#if	CPM || DOS || OS2
	    case 'N':
	    case 'n':
#else
	    case 'n':
#endif
		*strout++ = NL;
		break;

#if	CPM || DOS || OS2
	    case 'B':
	    case 'b':
#else
	    case 'b':
#endif
		*strout++ = BS;
		break;

#if	CPM || DOS || OS2
	    case 'F':
	    case 'f':
#else
	    case 'f':
#endif
		*strout++ = FF;
		break;

#if	CPM || DOS || OS2
	    case 'R':
	    case 'r':
#else
	    case 'r':
#endif
		*strout++ = CR;
		break;

#if	CPM || DOS || OS2
	    case 'V':
	    case 'v':
#else
	    case 'v':
#endif
		*strout++ = VT;
		break;

	    default:
		*strout++ = *strptr;
	    }

	    strptr++;
	}

    }
    *strout = '\0';		       /* Heaven forfend that we forget
				        * this! */
}

void 
prerr(etype, estring)
int etype;
char *estring;
{
    switch (etype)
    {
    case USAGE:
	fprintf(stderr, "%s: Usage: %s [-s] [-d<delimiterstring>] file1 file2 ...\n", cmdnam, cmdnam);
	break;

    case DELIMTOOLONG:
	fprintf(stderr, "%s: Delimiter (%s) too long max is %d\n",
		cmdnam, estring, SZDELIM - 1);
	break;

    case NODELIM:
	fprintf(stderr, "%s: no delimiters\n", cmdnam);
	break;

    case BADFILE:
	fprintf(stderr, "%s: %s : cannot open\n", cmdnam, estring);
	break;

    case TOOMANY:
	fprintf(stderr, "%s: too many files\n", cmdnam);
	break;
    }
    exit(1);
}
