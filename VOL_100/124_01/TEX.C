/*
 * small tex - (tau epsilon chi; pronounced `tech') A stripped down
 *	version of Knuths TEX for micros. This uses the Fancy Font/Epson
 *	system as an output device.
 *
 *	copyright (c) 1982, Mike Meyer
 *
 *	This code and all documentation that accompanies it may be freely
 *	distributed by anyone who has a copy, under the following provisions:
 *
 *		1) My copyright notices stay where they are.
 *		2) The source is distributed with the package.
 *		3) Fixes and improvements eventually come back to me.
 *
 *	Note that this does not disallow you from selling copies of this program *	to other people, so long as my copyright notices, etc stay with it. I
 *	don't care if you make money of my work - I just want credit for having
 *	done it.
 *
 *		<mike
 */
#define VERSION		1
#define RELEASE		'a'
#define	DATE		"December 1982"

#include <a:bdscio.h>

/*
 * DEBUG - until such a time as I can make the #)(%&*#% thing justify
 *	with my margins, the code to let users play with the right margin
 *	is turned off. This hardwired to RIGHTDEFAULT.
 */
#define	RIGHT		FALSE

/*
 * TRACEPOINTS - turns on some printfs to trace the margins/breakpoints.
 */
#define	TRACEPOINTS	FALSE

/* Some characters that are magic to tex */
#define SPACE		' '
#define NL		'\012'
#define CR		'\015'
#define TAB		'\t'
#define OPEN		'{'
#define CLOSE		'}'
#define COMMAND		'\\'
#define MATHMODE	'$'
#define	COMMENT		'%'
#define	PARAMETER	'#'
#define FONTCHAR	':'

/* the names  of some strings we print every once and a while (ff commands) */
#define ENDLINE		"\n"
#define JUSTON		"\\j"
#define JUSTOFF		"\\k"
#define CENTER		"\\c"
#define	LEFTIFY		"\\b"
#define RIGHTIFY	"\\r"
#define UNDERLINE	"\\u"
#define ONMATH		""
#define OFFMATH		""
#define ONDISPLAY	"\\b\n\n\\c"
#define OFFDISPLAY	"\n\n"
#define PARAGRAPH	"\\b\n\n"
#define	PAGE		"\\b\n\\p"

/* And some formats for ff commands for us to use */
#define FORMLINE	"\\a%04d"
#define FORMCHAR	"\\d%03d"
#define FORMHORIZ	"\\i%03d"
#define	FORMVERT	"\\b\n\\v%04d"
#define FORMFONT	"\\f%d"

/* and now some magic constants */
#define LEFTDEFAULT	0
#define RIGHTDEFAULT	900
#define INDENTDEFAULT	0
#define DEFAULTCHARLEN	12
#define	FILENAMELEN	15
#define COMLENGTH	10
#define	NUMFONTS	10
#define NUMDEFS		200
#define ASCIICHARS	128
#define INBUF		1024

/* and a (the?) magic file name... */
#define FFIFILE		"tex.ffi"

/* and a few macros */
#define strequal(x,y)	(!strcmp(x, y))

/* and a few global variables */
char	mathmode, displaymode ;		/* a pair of mathmode flags */
char	fillmode  ;			/* are we doing word filling? */
char	FFflag ;			/* Fancy Font commands valid? */
char	mathfont, textfont ;		/* the math and text mode fonts */
char	*endpointer, inbuffer[INBUF] ;/* input buffer & pointer into it */
int	inchar ;			/* the next char to be processed */
int	rightmargin, leftmargin ;	/* current margin settings */
int	indent ;			/* indent for paragraphs */
int	currentpoint, lastbreakpoint ;	/* last word break & current char */
char	outfile[BUFSIZ] ;		/* output file pointer */
int	nextdef ;			/* the next free defdata structure */
int	cfont ;				/* the current font number */
char	passtoff[INBUF] ;		/* user commands to fancy fonts */
char	fontnames[NUMFONTS][FILENAMELEN] ;
					/* the names of the font files used */
struct	chardata {
	char	f_width ;		/* how much space it takes */
	char	f_leftmargin ;		/* how much left margin it has */
	char	f_inkwidth ;		/* the width of ink in the character */
	char	f_inkheight ;		/* how high the ink is */
	char	f_inkbottom ;		/* how low the ink goes */
	} fontdata[NUMFONTS][ASCIICHARS] ;

struct	defdata {
	char	d_defname[COMLENGTH] ;/* the define name */
	char	*d_parameter ;		/* does it have a parameter */
	char	*d_altparm ;		/* a pointer into parameter */
	char	*d_replace ;		/* the replacement string */
	char	*d_nextchar ;		/* a pointer into it */
	} defines[NUMDEFS] ;

/* and a few non-int global functions */
char	width() ;
int	readch(), defch(), stringch() ;

/* these are a few (crock, a LOT) of my favorite (?) things */

main(argc, argv) char **argv; {
	char	*p, filename[FILENAMELEN], infile[BUFSIZ] ;

	printf("tex version %d%c - %s\n", VERSION, RELEASE, DATE) ;
	puts("For use with pfont version 1\n") ;
	puts("Copyright (c) 1982, Mike Meyer\n\n") ;

	if (argc > 2 || argc < 2) error("usage: tex <file>\n", NULL, 1) ;

	for (p = argv[1]; *p && *p != '.'; p++)
		;

	sprintf(filename, *p ? "%s" : "%s.tex", argv[1]) ;
	if (strlen(filename) >= FILENAMELEN)
		error("tex: Invalid file name: %s\n", argv[1], 1) ;
	if (fopen(filename, infile) == ERROR)
		error("tex: Can't open file: %s\n", filename,  1) ;
	*p = NULL ;		/* the output filename WILL be *.ff */
	sprintf(filename, "%s.ff", argv[1]) ;
	if (fcreat(filename, outfile) ==  ERROR)
		error("tex: Can't create file: %s\n", filename, 1) ;

	/* initialize the globals... */
	init() ;

	inchar = readch(infile) ;
	if (processtext(readch, infile) != EOF)
		printf("tex: Missing right brace in file: %s.TEX\n", argv[1]) ;
	if (mathmode) puts("tex: Document completed in math mode\n") ;

	/* cleanup the last pieces of text, and close everything */
	fputs(inbuffer, outfile) ;
	fputs(PARAGRAPH, outfile) ;
	putc(CPMEOF, outfile) ;
	fflush(outfile) ;
	fclose(outfile) ;
	puts("tex: Run complete\n") ;

	unlink(FFIFILE) ;
	if (fcreat(FFIFILE, outfile) == ERROR)
		error("tex: Can't create temp file: %s\n", FFIFILE, 1) ;
	/* Possibly not portable, but... */
	if (fprintf(outfile, "%s\n", passtoff) == ERROR
		|| fprintf(outfile, "+FO %s %s %s %s %s %s %s %s %s %s ,",
			fontnames[0], fontnames[1], fontnames[2], fontnames[3],
			fontnames[4], fontnames[5], fontnames[6], fontnames[7],
			fontnames[8], fontnames[9]) == ERROR)
		error("tex: Write error on file: %s\n", FFIFILE,1) ;
	putc(CPMEOF, outfile) ;
	fflush(outfile) ;
	fclose(outfile) ;

	execl("pfont", filename, "<", FFIFILE, 0) ;
	puts("tex: Can't execute pfont\n") ;
	return ERROR ;
	}
/*
 * init - make the global variables have reasonable values
 */
init() {
	int	i, j ;

	puts("tex: Initializing ... ") ;
	fputs(JUSTON, outfile) ;		/* justification defaults on */
	_allocp = NULL ;
	fillmode = TRUE ;
	FFflag = displaymode = mathmode = FALSE ;
	textfont = mathfont = nextdef = cfont = 0 ;
	*passtoff = *(endpointer = inbuffer) = NULL ;
	currentpoint = lastbreakpoint = leftmargin = LEFTDEFAULT ;
	rightmargin = RIGHTDEFAULT ;
	indent = INDENTDEFAULT ;
	for (i = 0; i < NUMFONTS; i++) {
		*(fontnames[i]) = 0 ;
		for (j = 0; j < ASCIICHARS; j++)
			fontdata[i][j] . f_width =
			fontdata[i][j] . f_leftmargin =
			fontdata[i][j] . f_inkwidth =
			fontdata[i][j] . f_inkheight =
			fontdata[i][j] . f_inkbottom =
			DEFAULTCHARLEN ;
		}
	puts("done\n") ;
	}
/*
 * processtext - do the actuall text formatting. This routine consists
 *	of a driver loop that reads characters with the following entry
 *	invariants:
 *		1) currentpoint < rightmargin
 *		2) inchar has the NEXT character to be processed
 *		4) endpointer points to next empty slot in inbuffer
 */
processtext(nextchar, arg) int (*nextchar)(); {
	char	myfont, myfill ;
	int	myleft, myright, myindent ;

	for (;; *endpointer = NULL) {
		if (endpointer - inbuffer > INBUF) /* unlikely, but... */
			error("tex: Out of buffer space\n", NULL, 1) ;
#if		TRACEPOINTS
		printf("Char %c, current %d, lastbreak %d, width %d\n",
			inchar, currentpoint, lastbreakpoint, width(inchar)) ;
#endif
		if (!fillmode && inchar == NL) {
				inchar = SPACE ;	/* will be tossed */
				makenewline(ERROR) ;
				inchar = (*nextchar)(arg) ;
				}
		else if (inchar == SPACE || inchar == TAB || inchar == NL)
			if (!mathmode) dowhite(nextchar, arg) ;
			else inchar = (*nextchar)(arg) ;
		else if (inchar == COMMAND) docommand(nextchar, arg) ;
		else if (inchar == MATHMODE) domath(nextchar, arg) ;
		else if (inchar == COMMENT) skipto(NL, nextchar, arg) ;
		else if (inchar == OPEN) {
			/* stack the environment */
			myfill = fillmode  ;
			myfont = cfont ;
			myleft = leftmargin ;
			myright = rightmargin ;
			myindent = indent ;

			/* jump down a level */
			inchar = (*nextchar)(arg) ;
			processtext(nextchar, arg) ;

			/* now unstack the environment */
			setfill(myfill) ;
			changetofont(myfont) ;
			leftmargin = myleft ;
			rightmargin = myright ;
			indent = myindent ;
			}
		else if (inchar == CLOSE)
			return inchar = (*nextchar)(arg) ;
		else if (inchar == EOF) return EOF ;
		else if ((currentpoint += width(inchar)) < rightmargin) {
			*endpointer++ = inchar ;
			inchar = (*nextchar)(arg)  ;
			}
		else {
			if (lastbreakpoint == leftmargin) makenewline(ERROR) ;
			else makenewline(currentpoint - lastbreakpoint) ;
			inchar = (*nextchar)(arg) ;
			}
		}
	return OK ;
	}
/*
 * dowhite - handle the word break code...
 */
dowhite(nextchar, arg) int (*nextchar)(arg); {

	fputs(inbuffer, outfile) ;		/* flush the current word */
	endpointer = inbuffer ;

	do {	/* eat spaces */
		if (inchar == NL) {		/* test for paragraph break */
			if ((inchar = (*nextchar)(arg)) == EOF) break ;
			if (inchar == NL) {	/* DO a paragraph break */
				fputs(PARAGRAPH, outfile) ;
				fprintf(outfile, FORMLINE,
					indent + leftmargin) ;
				currentpoint = lastbreakpoint
					= leftmargin + indent ;
#if				TRACEPOINTS
				puts("currentpoint reset to margin+indent\n") ;
#endif
				return skipwhite(nextchar, arg) ;
				}
			}
		else if ((inchar = (*nextchar)(arg)) == EOF) break ;
		if (inchar == COMMENT && skipto(NL, nextchar, arg) == EOF)
			break ;
		} while (inchar == SPACE || inchar == TAB || inchar == NL) ;

	if ((currentpoint += width(SPACE)) < rightmargin) {

		lastbreakpoint = currentpoint ;
		*endpointer++ = SPACE ;
		}
	else {
		currentpoint = lastbreakpoint = leftmargin ;
#if		TRACEPOINTS
		puts("currentpoint reset to margin\n") ;
#endif
		fputs(ENDLINE, outfile) ;
		fprintf(outfile, FORMLINE, leftmargin) ;
		}
	return inchar ;
	}
/*
 * domath - change the magic math modes around. All this does is
 *	toggles the mathmode/displaymode flags, and outputs the
 *	things to make it change modes. Assumes that the first $
 *	has been read, but not the (possible) second. Leaves inchar
 *	correct for next loop.
 *
 *	The decision table for this toy is as follows:
 *
 *				Mathmode
 *
 *			TRUE		FALSE
 *		TRUE	User Error	Internal Error		inchar != $
 *		FALSE	MM=F		MM=T
 * Displaymode
 *		TRUE	MM=DM=F		Internal Error		inchar == $
 *		FALSE	User Error	MM=DM=T
 *
 *	N.B. - this code should probably be rewritten as an else-if chain,
 *	as it isn't used very often; We could afford the loss in speed for
 *	the gain in size. Hence this is a good place to trim.
 */
domath(nextchar, arg) int (*nextchar)(); {

	inchar = (*nextchar)(arg) ;
	fputs(inbuffer, outfile) ;
	endpointer = inbuffer ;

	switch ((mathmode ? 1 : 0)
	   + (displaymode ? 2 : 0)
	   + (inchar == MATHMODE ? 4 : 0)) {	

		case 0: /* !mathmode, !displaymode, inchar != $ */
			mathmode = TRUE ;
			fputs(ONMATH, outfile) ;
			textfont = cfont ;
			changetofont(mathfont) ;
			return inchar ;

		case 1: /* mathmode, !displaymode, inchar != $ */
			mathmode = FALSE ;
			fputs(OFFMATH, outfile) ;
			changetofont(textfont) ;
			return inchar ;

		case 2: /* !mathmode, displaymode, inchar != $ */
		case 6:	/* !mathmode, displaymode, inchar == $ */
			error("tex: Internal error in mathmode: inchar = %c\n",
				inchar, 1) ;

		case 3: /* mathmode, displaymode, inchar != $ */
			puts("tex: Leaving display mode with `$'\n") ;
			mathmode = displaymode = FALSE ;
			changetofont(textfont) ;
			fputs(OFFDISPLAY, outfile) ;
			fprintf(outfile, FORMLINE, leftmargin) ;
			lastbreakpoint = currentpoint = leftmargin ;
			return skipwhite(nextchar, arg) ;

		case 4: /* !mathmode, !displaymode, inchar == $ */
			mathmode = displaymode = TRUE ;
			fputs(ONDISPLAY, outfile) ;
			textfont = cfont ;
			changetofont(mathfont) ;
			lastbreakpoint = currentpoint = leftmargin ;
			return inchar = (*nextchar)(arg) ;

		case 5: /* mathmode, !displaymode, inchar == $ */
			puts("tex: Leaving math mode with `$$'\n") ;
			mathmode = displaymode = FALSE ;
			fputs(OFFMATH, outfile) ;
			changetofon(textfont) ;
			return inchar = (*nextchar)(arg) ;

		case 7: /* mathmode, displaymode, inchar == $ */
			mathmode = displaymode = FALSE ;
			changetofont(textfont) ;
			fputs(OFFDISPLAY, outfile) ;
			fprintf(outfile, FORMLINE, leftmargin) ;
			lastbreakpoint = currentpoint = leftmargin ;
			inchar = (*nextchar)(arg) ;
			return skipwhite(nextchar, arg) ;

		default: /* I can't get here from there! */
			error("tex: Domath compiler error!\n", NULL, 1) ;
		}
	}
/*
 * docommand - take care of command strings in the text of the
 *	program.  This is generally handled by dumping magic
 *	straight to outfile, or invoking processtext (recursively)
 *	on some other intput stream. Note that this assumes that inchar
 *	has the COMMAND char in it, and that it leaves inchar updated...
 */
docommand(nextchar, arg) int (*nextchar)(); {
	char	*compointer, command[COMLENGTH] ;

	if ((inchar = (*nextchar)(arg)) == EOF) return EOF ;
	*(compointer = command) = inchar ;

	if (!isalpha(inchar)) {
		command[1] = NULL ;
		inchar = (*nextchar)(arg) ;
		}
	else {
		/*
		 * inchar HAS to be in this loop, to prevent EOF's from
		 *	getting clipped to DEL's...
		 */
		while (isalpha(*++compointer = inchar = (*nextchar)(arg)))
			if (compointer - command >= COMLENGTH - 1) {
				*compointer = NULL ;
				printf("tex: Command too long: %s\n", command);
				return skiptowhite(nextchar, arg) ;
				}
		*compointer = NULL ;
		texcase(command) ;
		}
	skipwhite(nextchar, arg) ;

	if (strequal(command, "f")) return dofont(nextchar, arg) ;
	if (strequal(command, "char")) return dochar(nextchar, arg) ;
	if (strequal(command, "mathonly")) return checkmath(nextchar, arg) ;
	if (strequal(command, "def")) return dodef(nextchar, arg) ;
	if (strequal(command, "eject")) return doeject(nextchar, arg) ;
	if (strequal(command, "ctr")) return docenter(nextchar, arg) ;
	if (strequal(command, "rt")) return doright(nextchar, arg) ;
	if (strequal(command, "lft")) return doleft(nextchar, arg) ;
	if (strequal(command, "underline")) return dounder(nextchar, arg) ;
	if (strequal(command, "fill")) return dofill(nextchar, arg) ;
	if (strequal(command, "vskip")) return dovert(nextchar, arg) ;
	if (strequal(command, "hskip")) return dohoriz(nextchar, arg) ;
	if (strequal(command, "par")) return dopar(nextchar, arg) ;
	if (strequal(command, "indent")) return setindent(nextchar, arg) ;
	if (strequal(command, "lftmarg")) return setleft(nextchar, arg) ;
#if	RIGHT
	if (strequal(command, "rtmarg")) return setright(nextchar, arg) ;
#endif
	if (strequal(command, "input")) return doinput(nextchar, arg) ;
	if (strequal(command, "math")) return setmath(nextchar, arg) ;
	if (strequal(command, "ff")) return doff(nextchar, arg) ;
	if (lookup(command, nextchar, arg)) return inchar ;
	printf("tex: Illegal command: \\%s\n", command) ;
	return inchar ;
	}
/*
 * dofont - take care of font changes/load. Assumes that the font char hasn't
 *	been gotten, but that the ':' in the font command has. Will return
 *	EOF/not EOF
 */
dofont(nextchar, arg) int (*nextchar)(); {
	char	newfont ;

	if (inchar != FONTCHAR) {
		printf("tex: Illegal font command: \\f%c\n", inchar) ;
		return inchar = (*nextchar)(arg) ;
		}
	inchar = (*nextchar)(arg) ;
	if (inchar < '0' || inchar > '9') {
		printf("tex: Illegal font character: %c\n", newfont) ;
		if ((inchar = (*nextchar)(arg)) == EOF) return EOF ;
		if (inchar != '=') return inchar ;
		/* eat up the file name... */
		if ((inchar = (*nextchar)(arg)) == EOF) return EOF ;
		if (skipwhite(nextchar, arg) == EOF
		   || skiptowhite(nextchar, arg) == EOF) return EOF ;
		return skipwhite(nextchar, arg) ;
		}
	newfont = inchar - '0' ;
	if ((inchar = (*nextchar)(arg)) == '=')
		return loadfont(nextchar, arg, newfont) ;
	changetofont(newfont) ;
	return skipwhite(nextchar, arg) ;
	}
/*
 * loadfont - load a font data file into memory. This thing opens the
 *	data file from Softcraft, load the right font file, and then goes
 *	on it's merry way. Note that it doesn't [I repeat - DOES NOT!]
 *	reinitialize the font tables, so loading a second font on top
 *	of a used font character can produce unpredictable results.
 *	This does leave inchar pointed at the next character, and returns
 *	EOF/OK.
 */
loadfont(nextchar, arg, newfont) int (*nextchar)(); {
	char	i, c, fontfile[BUFSIZ], filename[FILENAMELEN] ;
	struct chardata	*pointer ;

	if ((inchar = (*nextchar)(arg)) == EOF) return EOF ;
	getfilename(filename, nextchar, arg) ;
	strcpy(fontnames[newfont], filename) ;
	strcat(filename, ".fon") ;
	if (fopen(filename, fontfile) == ERROR) {
		printf("tex: Can't open font file: %s\n", filename) ;
		return OK ;
		}

	printf("tex: Loading font file %s ... ", filename) ;
	for (i = 0; i++ < 42;)
		getc(fontfile) ;		/* chew up header */
	while (c = getc(fontfile))
		;				/* look for trailing null */
	if (getc(fontfile) != 2 || getc(fontfile) != 3) {
		printf("tex: Bad font file: %s\n", filename) ;
		fclose(fontfile) ;
		return OK ;
		}
	getc(fontfile); getc(fontfile) ;	/* eat more stuff */
	i = getc(fontfile);			/* # of defined characters */
/* DEBUG - you really need to check to see if we ran out of file on these
getc's.. */

	for (c = 6; c++ <= 15;)
		getc(fontfile) ;		/* eat reserved bytes */

	while (i--) {				/* loop over defined bytes */
		c = getc(fontfile) ;		/* the character defined */
		if (c >= ASCIICHARS || c < 0) {	/* signed or unsigned chars? */
			printf("tex: Bad font file: %s\n", filename) ;
			return OK ;
			}
		pointer = &fontdata[newfont][c] ;
		/* now fetch the data that goes with the character */
		pointer -> f_width = getc(fontfile) + 2 ;
		pointer -> f_leftmargin = getc(fontfile) ;
		pointer -> f_inkwidth = getc(fontfile) ;
		pointer -> f_inkheight = getc(fontfile) ;
		pointer -> f_inkbottom  = getc(fontfile) ;
		}
	puts("done\n") ;

	fclose(fontfile) ;		/* clean up after myself */
	return OK ;
	}
/*
 * dochar - do the 'char' command. Leaves inchar in the right state for the
 *	next loop of processtext.
 */
dochar(nextchar, arg) int (*nextchar)(); {
	int	tempin ;
	char	tempbuf[FILENAMELEN] ;

	getnumber(&tempin, nextchar, arg) ;
	if (tempin < 0 || tempin > 127) {
		printf("tex: Invalid character: %03d\n", tempin) ;
		return inchar ;
		}
#if	TRACEPOINTS
	printf("currentpoint %d, lastbreak %d, width %d\n",
		currentpoint, lastbreakpoint, width(tempin)) ;
#endif
	currentpoint += width(tempin) ;
	/* Curse you, DRI! */
	if (!tempin || tempin == CPMEOF || tempin == COMMAND)
		sprintf(tempbuf, FORMCHAR, tempin) ;
	else sprintf(tempbuf, "%c", tempin) ;
	forceout(tempbuf) ;
	return inchar ;
	}
/*
 * checkmath - just check to see if we are in math mode, and bitch
 *	if not...
 */
checkmath() {

	if (!mathmode)
		puts("tex: Math only command issued outside math mode\n") ;
	return OK ;
	}
/*
 * doinput - change to a different input stream. This routine
 *	works by running a second copy of processtext with a change
 *	of input stream. Leaves inchar in the right state.
 */
doinput(nextchar, arg) int (*nextchar)(); {
	char	*p, filename[FILENAMELEN], filebuf[BUFSIZ] ;
	int	savechar ;

	getfilename(filename, nextchar, arg) ;
	if (strlen(filename) > FILENAMELEN - 5) {
		printf("tex: Invalid file name: %s\n", filename) ;
		return inchar ;
		}
	for (p = filename; *p;)
		if (*p++ == '.') {
			printf("tex: Invalid file name: %s\n", filename) ;
			return inchar ;
			}
	strcat(filename, ".tex") ;
	if (fopen(filename, filebuf) == ERROR) {
		printf("tex: Can't open file: %s\n", filename) ;
		return inchar ;
		}
	savechar = inchar ;
	inchar = readch(filebuf) ;

	printf("tex: Processing file %s\n", filename) ;
	if (processtext(readch, filebuf) != EOF)
		printf("tex: Missing right brace in file: %s\n", filename) ;
	printf("tex: Finished with file %s\n", filename) ;

	return inchar = savechar ;
	}
/*
 * dodef - do the define command. This expects a '\' in inchar, and
 *	eats up a define command afterwards. The syntax for the
 *	(currently) accepted define commands are:
 *
 *	\def\defname#1{text of define}	\def\x#1{text of define}
 *	\def\defname{text of define}	\def\x{text of define}
 *
 *	The string #1 following the defname indicates a parameter. The
 *	x in the right-hand defines is a single non-alphabetic character.
 */
dodef(nextchar, arg) int (*nextchar)(); {
	char	*defpointer, defname[COMLENGTH] ;

	if (inchar != COMMAND) {
		printf("tex: Bad define character. Expected %c, got %c\n",
							COMMAND, inchar) ;
		return inchar = (*nextchar)(arg) ;
		}

	if ((inchar = (*nextchar)(arg)) == EOF) return EOF ;
	*(defpointer = defname) = inchar ;

	if (!isalpha(inchar)) {
		defname[1] = NULL ;
		inchar = (*nextchar)(arg) ;
		}
	else {
		while ((inchar = (*nextchar)(arg)) != OPEN
		    && inchar != PARAMETER) {
			if (defpointer - defname >= COMLENGTH - 1) {
				*defpointer = NULL ;
				printf("tex: Define to long: %s\n", defname) ;
				return skiptowhite(nextchar, arg) ;
				}
			*++defpointer = inchar ;
			}
		*++defpointer = NULL ;
		texcase(defname) ;
		}

	if (inchar == OPEN) return definsert(defname, nextchar, arg, FALSE) ;
	if (inchar != PARAMETER) {
		puts("tex: Bad define: %s", defname) ;
		return skiptowhite(nextchar, arg) ;
		}
	if ((inchar = (*nextchar)(arg)) != '1') {
		printf("tex: Bad define parameter. Expected 1, got %c", 
			inchar) ;
		return OK ;
		}
	if ((inchar = (*nextchar)(arg)) == OPEN)
		return definsert(defname, nextchar, arg, TRUE) ;
	printf("tex: Bad define: %s", defname) ;
	return skiptowhite(nextchar, arg) ;
	return OK ;
	}
/*
 * definsert - put the named object in the define table. The parameter
 *	flag is copied appropriately.
 *
 *	Note: right now, the parameter flag could be set in dodef, but
 *	if I change the table search (binary search, or hashing, or
 *	whatever), this would no longer be true.
 */
definsert(name, nextchar, arg, parameter) int (*nextchar)(); char *name; {
	char	token[INBUF] ;

	if (nextdef >= NUMDEFS)
		error("tex: To many defines: %s not defined\n", name, 1) ;
	defines[nextdef] . d_parameter = parameter ;
	strcpy(defines[nextdef] . d_defname, name) ;
	gettoken(token, INBUF, nextchar, arg) ;
	if ((defines[nextdef] . d_replace = sbrk(strlen(token) + 2)) == ERROR)
		error("tex: Out of string space\n") ;
	strcpy(defines[nextdef++] . d_replace, token) ;
	return inchar ;
	}
/*
 * lookup - decide if command is in the define tables. If it is, feed
 *	it to processtext and return TRUE, otherwise return FALSE.
 *	In either case, make sure that inchar is correct for the future.
 *	If no input is done, this is already true...
 */
lookup(command, nextchar, arg) char *command; int (*nextchar)(); {
	int	i, savechar ;
	char	token[INBUF] ;

	for (i = 0;; i++)
		if (i >= nextdef) return FALSE ;
		else if (strequal(command, defines[i] . d_defname)) break ;

	defines[i] . d_nextchar = defines[i] . d_replace ;
	if (!defines[i] . d_parameter) {
		savechar = inchar ;
		inchar = stringch(&defines[i] . d_nextchar) ;
		processtext(stringch, &defines[i] . d_nextchar) ;
		}
	else {
		gettoken(token, INBUF, nextchar, arg) ;
		savechar = inchar ;
		defines[i] . d_altparm = NULL ;
		defines[i] . d_parameter = token ;
		inchar = defch(defines + i) ;
		processtext(defch, defines + i) ;
		}
	inchar = savechar ;
	return TRUE ;
	}
/*
 * doeject - force a new page to start.
 */
doeject(nextchar, arg) int (*nextchar)(); {

	lastbreakpoint = currentpoint = leftmargin ;
	forceout(PAGE) ;
	return skipwhite(nextchar, arg) ;
	}
/*
 * docenter - Center the following token on the line, doing the
 *	appropriate magic with inchar.
 */
docenter(nextchar, arg) int (*nextchar)(); {
	char	buffer[INBUF], *charpointer ;
	int	savechar ;

	gettoken(charpointer = buffer, INBUF, nextchar, arg) ;
	forceout(CENTER) ;
	savechar = inchar ;
	inchar = stringch(&charpointer) ;
	processtext(stringch, &charpointer) ;
	return inchar = savechar ;
	}
/*
 * doleft - The following code should be on the left of the line, but...
 *	do the appropriate magic with inchar.
 */
doleft(nextchar, arg) int (*nextchar)(); {
	char	buffer[INBUF], *charpointer ;
	int	savechar ;

	gettoken(charpointer = buffer, INBUF, nextchar, arg) ;
	forceout(LEFTIFY) ;
	savechar = inchar ;
	inchar = stringch(&charpointer) ;
	processtext(stringch, &charpointer) ;
	return inchar = savechar ;
	}
/*
 * doright - Put the following text on the far right side of the line, doing
 *	the appropriate magic with inchar.
 */
doright(nextchar, arg) int (*nextchar)(); {
	char	buffer[INBUF], *charpointer ;
	int	savechar ;

	gettoken(charpointer = buffer, INBUF, nextchar, arg) ;
	forceout(RIGHTIFY) ;
	savechar = inchar ;
	inchar = stringch(&charpointer) ;
	processtext(stringch, &charpointer) ;
	lastbreakpoint = currentpoint = leftmargin ;
	makenewline(ERROR) ;
	return inchar = savechar ;
	}
/*
 * dounder - turn on the underline mode for a token or so. Does the
 *	appropriate magic with inchar.
 */
dounder(nextchar, arg) int (*nextchar)(); {
	char	buffer[INBUF], *charpointer ;
	int	savechar ;

	gettoken(charpointer = buffer, INBUF, nextchar, arg) ;
	forceout(UNDERLINE) ;
	savechar = inchar ;
	inchar = stringch(&charpointer) ;
	processtext(stringch, &charpointer) ;
	forceout(UNDERLINE) ;
	return inchar = savechar ;
	}
/*
 * dofill - reads in the next number. If it is zero, turn off word filling,
 *	otherwise turn on word filling. Does the right thing with inchar.
 */
dofill(nextchar, arg) int (*nextchar)(); {
	char	flag, buffer[MAXLINE] ;

	getnumber(&flag, nextchar, arg) ;
	setfill(flag != 0) ;
	return inchar ;
	}
/*
 * setmath - does the \math command to set the mathmode variable to
 *	the rigth thing. Assumes that inchar has the : and that a
 *	digit follows thereafter. Makes inchar right.
 */
setmath(nextchar, arg) int (*nextchar)(arg); {

	if (inchar != FONTCHAR) {
		puts("tex: Illegal command \\math%c\n", inchar) ;
		return inchar = (*nextchar)(arg) ;
		}
	if ((inchar = (*nextchar)(arg)) == EOF) return EOF ;

	if (inchar < '0' || inchar > '9') {
		printf("tex: Illegal font character: %c\n", inchar) ;
		return inchar = (*nextchar)(arg) ;
		}
	mathfont = inchar - '0' ;

	if ((inchar = (*nextchar)(arg)) == EOF) return EOF ;
	return skipwhite(nextchar, arg) ;
	}
/*
 * dopar - force a paragraph to start/end...
 */
dopar(nextchar, arg) int (*nextchar)(); {
	char	buffer[MAXLINE] ;

	currentpoint = lastbreakpoint = leftmargin + indent  ;
	forceout(PARAGRAPH) ;
	sprintf(buffer, FORMLINE, currentpoint) ;
	forceout(buffer) ;
	return OK ;
	}
/*
 * dohoriz - skip over some horizontal number of points...
 */
dohoriz(nextchar, arg) int (*nextchar)(); {
	int	number ;
	char	buffer[MAXLINE] ;

	getnumber(&number, nextchar, arg) ;
	if (inchar != 'p' || (inchar = (*nextchar)(arg)) != 't')
		puts("tex: Need `pt' to follow hskip number\n") ;
	inchar = (*nextchar)(arg) ;	/* skip over the t... */

	currentpoint += number ;
	sprintf(buffer, FORMHORIZ, number) ;
	forceout(buffer) ;
	return skipwhite(nextchar, arg) ;
	}
/*
 * dovert - skip over some vertical number of points...
 */
dovert(nextchar, arg) int (*nextchar)(); {
	int	number ;
	char	buffer[MAXLINE] ;

	getnumber(&number, nextchar, arg) ;
	if (inchar != 'p' || (inchar = (*nextchar)(arg)) != 't')
		puts("tex: Need `pt' to follow vskip number\n") ;
	inchar = (*nextchar)(arg) ;	/* skip over the t... */

	currentpoint = lastbreakpoint = leftmargin ;
	sprintf(buffer, FORMVERT, number) ;
	forceout(buffer) ;
	return skipwhite(nextchar, arg) ;
	}
/*
 * setleft - set the left margin to some new value. Leaves inchar updated.
 */
setleft(nextchar, arg) int (*nextchar)(); {
	int	number, flag ;

	if ((flag = getnumber(&number, nextchar, arg)) == 0)
		leftmargin = number ;
	else if (flag > 0) leftmargin += number ;
	else if (flag < 0) leftmargin -= number ;
	if (leftmargin < 0) {
		puts("tex: Negative left margin, left set to 0\n") ;
		leftmargin = 0 ;
		}
	return inchar ;
	}
#if	RIGHT
/*
 * setright - like setleft, but for the right margin.
 */
setright(nextchar, arg) int (*nextchar)(); {
	int	number, flag ;

	if ((flag = getnumber(&number, nextchar, arg)) == 0)
		rightmargin = number ;
	else if (flag > 0) rightmargin += number ;
	else if (flag < 0) rightmargin -= number ;
	return inchar ;
	}
#endif
/*
 * setindent - setleft/setright again, but for the indentation.
 */
setindent(nextchar, arg) int (*nextchar)(); {
	int	number, flag ;

	if ((flag = getnumber(&number, nextchar, arg)) == 0)
		indent = number ;
	else if (flag > 0) indent += number ;
	else if (flag < 0) indent -= number ;
	if (leftmargin + indent < 0) {
		printf("tex: Indent to small, set to: %d", -leftmargin) ;
		indent = -leftmargin ;
		}
	return inchar ;
	}
/*
 * doff - get things from the input stream to be passed to Fancy Fonts.
 *	This also leaves inchar done up right.
 */
doff(nextchar, arg) int (*nextchar)(); {

	gettoken(passtoff, INBUF, nextchar, arg) ;
	return inchar ;
	}
/*
 * getfilename - go fetch a file name from the current input stream.
 *	reads the name into name, and leaves inchar pointed to the
 *	right thing.
 */
getfilename(name, nextchar, arg) char *name; int (*nextchar)(); {
	char	counter ;

	if (skipwhite(nextchar, arg) == EOF) return EOF ;
	for (counter = 0; inchar != EOF; inchar = (*nextchar)(arg)) {
		if (counter > FILENAMELEN - 5) {
			strcpy(name, "bad name") ;
			if (skiptowhite(nextchar, arg) == EOF)
				return EOF ;
			return skipwhite(nextchar, arg) ;
			}
		if (isalpha(inchar) || isdigit(inchar) || inchar == ':')
			*name++ = inchar ;
		else {			/* got the whole thing */
			*name = NULL ;
			return skipwhite(nextchar, arg) ;
			}
		}
	*name = NULL ;			/* EOF - make sure to turn it off */
	return EOF ;
	}
/*
 * getnumber - fetch a number from the input stream, skipping whitespace
 *	afterwards. Automagically leaves inchar right.
 */
getnumber(number, nextchar, arg) int *number, (*nextchar)(arg); {
	int	sign ;

	sign = *number = 0 ;
	if (skipwhite(nextchar, arg) == EOF) return 0 ;
	if (inchar == '+') sign = 1 ;
	else if (inchar == '-') sign = -1 ;

	if (sign) inchar = (*nextchar)(arg) ;
	while (isdigit(inchar)) {
		*number = 10 * *number + (inchar - '0') ;
		if ((inchar = (*nextchar)(arg)) == EOF) return sign ;
		}
	skipwhite(nextchar, arg) ;
	return sign ;
	}
/*
 * gettoken - fetch a token out of the input stream. A token is defined
 *	as a single character, OR a string surrounded by OPEN and CLOSE.
 *	Expects inchar to have an OPEN (if there is one), and leaves it
 *	updated.
 */
gettoken(token, length, nextchar, arg) int (*nextchar)(); char *token; {
	char	count ;

	if (inchar != OPEN) {
		*token++ = inchar ;
		*token = NULL ;
		inchar = (*nextchar)(arg) ;
		return OK ;
		}

	count = 0 ;
	while ((inchar = (*nextchar)(arg)) != CLOSE || count > 0) {
		if (inchar == EOF) {
			*token = NULL ;
			return ERROR ;
			}
		if (length > 0) {
			*token++ = inchar ;
			length-- ;
			}
		if (inchar == OPEN) count++ ;
		if (inchar == CLOSE) count-- ;
		}
	*token = NULL ;
	inchar = (*nextchar)(arg) ;
	skipwhite(nextchar, arg) ;
	if (length <= 0) puts("tex: Input token to long\n") ;
	return OK;
	}
/*
 * width - return the width of the argument character in the current font
 */
char
width(ch) char ch; {

	return fontdata[cfont][ch] . f_width;
	}
/*
 * skipwhite - skips over whitespace in the input stream. Assumes that inchar
 *	has the next char to be processed, and it may/may not need to be
 *	skipped.
 */
skipwhite(nextchar, arg) int (*nextchar)(); {

	while (inchar == SPACE || inchar == TAB || inchar == NL) {
		if ((inchar = (*nextchar)(arg)) == EOF) return EOF ;
		if (inchar == COMMENT) skipto(NL, nextchar, arg) ;
		}
	return inchar ;
	}
/*
 * skiptowhite - skips over non-white things. Generally used for eating
 *	bad commands and the like. Same assumptions as skipwhite
 */
skiptowhite(nextchar, arg) int (*nextchar)(); {

	while (inchar != SPACE && inchar != TAB && inchar != NL
	    && inchar != EOF) {
		if ((inchar = (*nextchar)(arg)) == EOF) return EOF ;
		if (inchar = COMMENT) skipto(NL, nextchar, arg) ;
		}
	return inchar ;
	}
/*
 * skipto - skip to the character asked for.
 */
skipto(ch, nextchar, arg) char ch; int (*nextchar)(arg); {

	while (inchar != ch)
		if ((inchar = (*nextchar)(arg)) == EOF) return EOF ;
	}
/*
 * setfill - set the fill mode to the  passed flag...
 */
setfill(flag) char flag; {

	if (fillmode == flag) return OK ;
	fillmode = flag  ;			/* change fill & */
	/* now set the justification properly */
	forceout(fillmode ? JUSTON : JUSTOFF, outfile) ;
	
	}
/*
 * changetofont - make the current font be the font passed as an argument
 */
changetofont(newfont) char newfont; {
	char	strtemp[5] ;

	if (newfont == cfont) return OK ;	/* right font, don't change */
	if (newfont < 0 || newfont > 9) {
		printf("tex: Illegal font character: %d\n", newfont) ;
		return OK ;
		}
	cfont = newfont ;			/* do the font change */
	sprintf(strtemp, FORMFONT, newfont) ;
	forceout(strtemp) ;
	}
/*
 * texcase - change the case of the passed string to lower,
 *	except for leaving the first character in the case it is in...
 */
texcase(string) char *string; {

	if (!*string)
		error("tex: Internal error in texcase.\n", NULL, 1) ;
	while (*++string)
		*string = tolower(*string) ;
	}
/*
 * forceout - puts the string in the output stream, RIGHT NOW!
 */
forceout(string) char *string; {

	while (*string)
		*endpointer++ = *string++ ;
	*endpointer = NULL ;
	}
/*
 * defch - traipse through the characters in a define, possibly with
 *	parameters.
 */
defch(string) struct defdata *string; {
	char	ch ;

	if (string -> d_altparm) {
		if (ch = *(string -> d_altparm++)) return ch ;
		string -> d_altparm = NULL ;
		}

	if (!(ch = *(string -> d_nextchar++))) return EOF ;
	if (!string -> d_parameter || ch != PARAMETER) return ch ;

	if ((ch = *(string -> d_nextchar++)) == PARAMETER) return PARAMETER ;
	if (ch != '1') {
		printf("tex: Bad parameter number %c\n", ch) ;
		return EOF ;
		}
	string -> d_altparm = string -> d_parameter ;
	return defch(string) ;
	}
/*
 * stringch - go through the characters in a string, one at a time.
 */
stringch(string) char **string; {
	char	ch ;

	if (!(ch = *(*string)++)) return EOF ;
	return ch ;
	}
/*
 * readch - a `censored' read character. Throws out carriage returns and checks
 *	for CPMEOF.
 */
readch(file) char *file; {
	int	c ;

	while ((c = getc(file)) == CR)
		;
	if (c == CPMEOF) return EOF ;
	return c ;
	}
/*
 * makenewline - start us a fresh new line, complete with FF \a command,
 *	etc. If offset is zero, it assumes that the buffer should be flushed.
 *	Otherwise, it assumes that what's in the buffer is offset long, and
 *	should be left in the buffer to start the next line.
 */
makenewline(offset) {

	if (offset == ERROR) {
		fputs(inbuffer, outfile) ;
		endpointer = inbuffer ;
		}
	fputs(ENDLINE, outfile) ;
	fprintf(outfile, FORMLINE, leftmargin) ;

	*endpointer++ = inchar ;
	currentpoint = leftmargin + width(inchar)
		+ (offset == ERROR ? 0 : offset) ;
	lastbreakpoint = leftmargin ;
	if (*inbuffer == SPACE) {
		currentpoint -= width(SPACE) ;
		*endpointer = NULL ;
		strcpy(inbuffer, inbuffer + 1) ;
		endpointer-- ;
		}
#if	TRACEPOINTS
	puts("currentpoint reset to margin + offset (maybe)\n") ;
#endif
	}
/*
 * error - print a message (possibly with an argument), and exit
 */
error(format, arg, code) char *format; {

	printf(format, arg) ;
	exit(code) ;
	}
/*
 * topofstack - debuging routine tells me where the top of the stack is
 */
topofstack(x) {

	return &x ;
	}
 it assumes that the buffer should be flushed.
 *	Otherwise, it assu