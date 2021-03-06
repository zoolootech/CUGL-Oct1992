/*
 * plot.h - header file for the JTL printer graphics (epson flavor)
 *	code.
 *	copyright (c) 1982, JTL Computer Services
 *	2/82 - mwm
 *
 * version 1.0:	September 1985
 *	added version number
 *	added #defines from plot.inc header
 *
 *	this header is used by plot.c
 */

/*
 * defines for the user of the line  genline routines
 */
#define UP	0
#define DOWN	1
#define TEXT	2
#define ENDPLOT	9

/*#define HIDEN		on for 960 dots per line, off for 480 */
#define COMPRESS	on for compressed text, off for normal text */

#ifdef	HIDEN
#define	LINELEN	960
#define PAGELEN	24
#define LENSTR	"%cL"
#else
#define	LINELEN	480
#define PAGELEN	792
#define LENSTR	"%cK"
#endif

#ifdef	COMPRESS
#define CDEN	2/7	/* don't parenthesize this! */
#define CLEN	7/2
#define CHARS	131
#else
#define CDEN	1/6	/* or this!!!!!!! */
#define CLEN	7/2
#define CHARS	79
#endif

#define VDEN	8	/* # of vertical dots on a line */
#define PLEN	(PAGELEN / VDEN + ((PAGELEN % VDEN) ? 1 : 0))
#define ESC	0x1b
/*
 * the mapping for the symbol table on any given plot is as follows -
 *	using symbol, plotsym (and tagsym), you get the symbols from
 *	1 to MAXSYM (this ought to be 255). Using plots, plotc (and tag)
 *	you get the characters from 1 to SYMSEP (ought to be 127, or DEL)
 *	and the symbols from SYMSEP+1 to MAXSYM. Note that the symbol
 *	routines are set up to handle strings, or arrays of chars, so MAXSYM
 *	CANNOT be larger thatn 255 (unless you change the code...)
 */
#define MAXSYM	255
#define	SYMSEP	127	/* last valid ascii character */
#define	SYMLEN	6	/* length of a non-compressed charcter in dots */
/*
 * the symbol table structure. MAXSYM per plot page.
 */
#define symtab	struct SYMTAB
symtab {
	char	s_symbol[SYMLEN] ;
	char	s_def ;			/* is this defined? */
	} ;
/*
 * this structure holds one print line of a plot
 */
#define plotline STRUCT PLOTLINE
plotline {
	char	*p_points ;		/* the points to plot on this line */
	char	*p_text ;		/* optional text to put on the line */
	} ;
/*
 * The actual plot structure - holds all the info needed to plot a window
 */
#define plot	STRUCT PLOT
plot {
	int		p_xmax, p_ymax ;	/* edges of the plot ... */
	plotline	p_page[PLEN] ;
	int		p_x, p_y ;		/* current "pen" positions */
	symtab		p_symtab[MAXSYM] ;	/* symbol table */
	} ;
                                                                           