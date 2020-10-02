/*
HEADER: CUG 121.??;

    TITLE:	plot - an epson plottingg package;
    VERSION:	1.0;
    DATE:	01/10/86;
    DESCRIPTION: "This program reads an input file of plot points and sends
		commands to an epson-like printer to plot these points via
		the graphics modes.
		This program contains useful general-purpose plot routines
		which could be extracted to form a library.";
    KEYWORDS:	plot, graphics, epson;
    SYSTEM:	CP/M;
    FILENAME:	PLOT.C;
    WARNINGS:	"Copyright (c) 1982, JTL Computer Services.
		Requires local.h & plot.h for compile.
		Requires local.c for link.
		Requires Epson MX-80 with Graftrax.";
    SEE-ALSO:	PLOT.H (header file), PLOT.DAT (sample data file);
    AUTHORS:	Mike W. Meyer;
    COMPILERS:	BDS-C 1.50;
*/
/*
 * plot - a package to use the dot-matrix abilities of the Epson
 *	mx-80 line of printers.
 *
 */

#include <bdscio.h>
#include "local.h"
#include "plot.h"

/*
 * section the first - the general plot manipulation routines.
 *	initialize & print plots.
 */

/*
 * initplot - zero out everything in the plot...
 */
initplot(page, x, y) plot *page; {
	int	i, j ;
	plotline *line ;

	page -> p_x = page -> p_y = 0 ;
	page -> p_xmax = min(x, LINELEN) ;
	page -> p_ymax = min(y, PAGELEN) ;
	line = page -> p_page ;
	for (i = 0; i < PLEN; line++) {
		line -> p_points = line -> p_text = NULL ;
		page -> p_symtab[i++] . s_def = FALSE ;
		}
	}
/*
 * printplot - output the plot to the printer
 */
printplot(page, file) plot *page; char *file; {
	int	i, j, len, plen ;
	plotline *line ;

	/* set up the epson so that it has the right line spacing */
	fprintf(file, "%cP%cA%c\r", ESC, ESC, VDEN) ;
	line = page -> p_page ;
	plen = page -> p_ymax / VDEN + ((page -> p_ymax % VDEN) ? 1 : 0) ;
	for (i = 0; i++ < plen; line++) {
		if (line -> p_points) {
			for (len = page -> p_xmax; len > 0; len--)
				if (line -> p_points[len - 1]) break ;
			fprintf(file, LENSTR, ESC) ;
			putc(len % 256, file) ;
			putc(len / 256, file) ;
			for (j = 0; j < len;)
				putc(line -> p_points[j++], file) ;
			free(line -> p_points) ;
			}
		if (line -> p_text) {
			fprintf(file, "\r%s", line -> p_text) ;
			free(line -> p_text) ;
			}
		fputs("\n", file) ;
		}
	/* Now, put the epson back in the normal 6 lpi mode */
	fprintf(file, "%cQ%c2", ESC, ESC) ;
	}
/*
 * section the second - routines used to put characters on the page.
 */

/*
 * plots - put string on page at position x, y (or thereabouts. Printer
 *	accuracy, you know).
 */
plots(page, x, y, string) char *string; plot *page; {
	int i ;

	do
		if (*string < 0x20) return ERROR ;
		else if ((x = plotc(page, x, y, *string)) == ERROR)
			return ERROR ;
		while (*string++) ;
	return OK ;
	}
/*
 * plotc - plot the character/symbol at x, y (or thereabouts).
 */
plotc(page, x, y, c) plot *page; {
	int i ;
	plotline *line ;

	if (c > SYMSEP)		/* plotting a symbol... */
		return (plotsym(page, x * CDEN * CLEN, y * VDEN / VDEN, c)
			+ CLEN) * CDEN * CLEN ;
	else {			/* normal character */
		if (!(line = page -> p_page[y / VDEN]) . p_text) {
			if ((line -> p_text = alloc(CHARS + 1)) == ERROR)
				return ERROR ;
			for (i = 0; i < CHARS;) line -> p_text[i++] = ' ' ;
			line -> p_text[i] = NULL ;
			}
		line -> p_text[x = x * CDEN] = c ;
		return (x + 1) * CLEN + 1 ;	/* + 1 to force new char */
		}
	}
/*
 * section the third - routines to manipulate symbols
 */

/*
 * makesym - create the symbol labeled symno for use on page.
 */
makesym(page, symno, symbol) plot *page; char *symbol; {
	char	i ;
	symtab	*sym ;

	if (!validsym(symno++)) return ERROR ;
	(sym = &(page -> p_symtab[symno])) -> s_def = TRUE ;
	for (i = 0; i < SYMLEN; i++)
		sym -> s_symbol[i] = symbol[i] ;
	}
/*
 * symbol - put an array of symbols on the plot page. X, y is the
 *	lower left hand corner of the first symbol in the string.
 */
symbol(page, x, y, symbols) plot *page; char *symbols; {

	while (*symbols)
		if ((x = plotsym(page, x, y, *symbols)) == ERROR)
			return ERROR ;
	}
/*
 * plotsym - put the symbol on the page. X, y is the lower left hand
 *	corner of the symbol.
 */
plotsym(page, x, y, symno) plot *page; {
	char i, j, byte ;

	if (!inbounds(x, y) || !validsym(symno--)) return ERROR ;
	for (i = 0; i < SYMLEN;) {
		byte = page -> p_symtab[symno] . s_symbol[i++] ;
		for (j = 0; j < CHARLEN; j++, byte >>= 1)
			if ((byte & 1) && on(page, x + i, y - j) == ERROR) 
				return ERROR ;
		}
	}
/*
 * validsym - tells whether or not symno is a valid symbol #. Used
 *	internally.
 */
validsym(symno) {

	return symno <= MAXSYM && symno > 0 ;
	}
/*
 * issym - tells whether or not symno is a defined symbol on page.
 */
issym(page, symno) plot *page; {

	return validsym(symno) && page -> p_symtab[symno] . s_def ;
	}
/*
 * section the fourth - a couple of routines to draw lines of
 *	various flavors
 */

/*
 * line - draw a line from point where we left the pen to x, y.
 *	if pen is down (== 2), or move the pen if it is up (== 3).
 */
line(page, x, y, pen) plot *page; {
	int on() ;

	return genline(page, x, y, pen, on) ;
	}
/*
 * genline - generalized draw a line routine.  This works the same
 *	as the line routine, except that it applies the routine toit
 *	to each point that is on the line, if pen is down (== 2). It
 *	just moves the pen if it is up (== 3).
 *	Algorithm from CALGO (Collected Algorithms of the ACM, for those
 *	of you not in ACM). Translated from the original scuzzy ALGOL to
 *	reasonable ALGOL by J. Jones. Then implemented in C by mwm & J. Jones.
 */
genline(page, x, y, pen, toit) plot *page; int (*toit)(); {
	int 	adx, ady, xstep, ystep, xfarther, dsquare, mindelta ,
		distance, zot ;

	if (pen == UP) {
		page -> p_x = x; page -> p_y = y ;
		return OK ;
		}
	adx = abs(page -> p_x - x) ;
	ady = abs(page -> p_y - y) ;
	xstep = sign(x - page -> p_x) ;
	ystep = sign(y - page -> p_y) ;
	xfarther = adx > ady  ;
	dsquare = abs(adx - ady) ;
	mindelta = min(adx, ady) ;
	distance = adx + ady ;
	for (zot = 0; distance > 0;) {
		if (zot + zot < dsquare - mindelta) {
			zot += mindelta ;
			distance-- ;
			if (xfarther) page -> p_x += xstep ;
			else page -> p_y += ystep ;
			}
		else {
			zot -= dsquare ;
			distance -= 2 ;
			page -> p_x += xstep ;
			page -> p_y += ystep ;
			}
		(*toit)(page, page -> p_x, page -> p_y) ;
		}
	return OK ;
	}

/*
 * section the bottom - this section contains the bottom level routines
 *	for manipulating the plot. These routines turn on & off points in
 *	the page image, and etc.
 */

/*
 * on - turn on the point x, y in the plot page.
 */
on(page, x, y) plot *page; {

	if (!inbounds(page, x, y)) return ERROR ;
	page -> p_page[y / VDEN] . p_points[x] |= 0x80 >> (y % VDEN) ;
	return OK ;
	}
/*
 * off - turn off the point x, y in the plot page off.
 */
off(page, x, y) plot *page; {

	if (!inbounds(page, x, y)) return ERROR ;
	page -> p_page[y / VDEN] . p_points[x] &= ~(0x80 >> (y % VDEN)) ;
	return OK ;
	}
/*
 * toggle - change the point x, y on the plot page.
 */
toggle(page, x, y) plot *page; {
 
	if (!inbounds(page, x, y)) return ERROR ;
 	page -> p_page[y / VDEN] . p_points[x] ^= (0x80 >> (y % VDEN)) ;
	return  OK ;
	}
/*
 * ison - return whether or not the point at x, y is on in the plot page.
 */
ison(page, x, y) plot *page; {

	if (!inbounds(page, x, y)) return ERROR ;
 	return page -> p_page[y / VDEN] . p_points[x] & (0x80 >> (y % VDEN)) ;
	}
/*
 * inbounds - used internally to tell if the point x, y is inbounds or not.
 *	also allocates space if it is needed for that piece of the plot.
 */
inbounds(page, x, y) plot *page; {
	int i ;
	plotline *line ;

	if (y >= page -> p_ymax || y < 0 || x >= page -> p_xmax || x < 0)
		return FALSE ;
	if ((line = page -> p_page[y / VDEN]) -> p_points) return TRUE ;
	if ((line -> p_points = alloc(page -> p_xmax)) == ERROR)
		return FALSE ;
	for (i = 0; i < page -> p_xmax;)
		line -> p_points[i++] = NULL ;
	return TRUE ;
	}
/*
 * section the last ...
 * main - a somewhat usefull demo of the things that this
 *	plot package can do (so far).  This thing reads in triplets
 *	of number from the file argv[1], or STDIN if no argv, and plots
 *	them ala the standard pen plotter graphics.  If there is no third
 *	(pen) argument, it is assumed to be down, except for the first line
 *	of input.  A blank line terminates input
 */
main(argc, argv) char **argv; {
	int	i, j, pen ;
	plot	page ;
	char	*infile, buf[BUFSIZ], *outfile, buf2[BUFSIZ] ,
		buffer[MAXLINE] ;

	_allocp = NULL ;
	if (argc > 3 || (argc > 1 && (*argv)[0] == '?'))
		barf("Usage: plot [<infile>|-] [<outfile>|-]") ;
	if (argc < 2 || *(argv[1]) == '-') infile = STDIN ;
	else if (fopen(argv[1], infile = buf) == ERROR) {
		printf("Can't open file %s.\n", argv[1]) ;
		exit(1) ;
		}
	if (argc < 3 || *(argv[2]) == '-') outfile = STDLST ;
	else if (fcreat(argv[2], outfile = buf2) == ERROR) {
		printf("Can't creat %s!\n", argv[2]) ;
		exit(1) ;
		}
	initplot(&page, LINELEN, PAGELEN) ;
	i = j = 0; pen = UP ;
	while (fscanf(infile, "%d %d %d %s\n", &i, &j, &pen, buffer)) {
		if (pen == TEXT) plots(&page, i, j, buffer) ;
		else if (pen != ENDPLOT) line(&page, i, j, pen) ;
		else {
			printplot(&page, outfile) ;
			initplot(&page, LINELEN, PAGELEN) ;
			}
		pen = DOWN ;
		}
	printplot(&page, outfile) ;
	if (outfile != STDLST) {
		fflush(outfile) ;
		fclose(outfile) ;
		}
	}
 
                                                                                                                       