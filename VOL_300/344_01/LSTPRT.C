/*		lstprt (c) 1989, 1990 Bryan R Leipper, Reno NV

SYNOPSIS: print files to PCL printer with graphics in landscape two up 80 x 69

DESCRIPTION: A utility to list files to a printer. Uses HP LJ II+ graphics to
  shade margin areas where headers, footers, and line numbering information is
  printed. Expands tabs and emulates formfeeds. Carriage return, linefeeds, or
  a combination are all taken as new line commands (file is considered an MSDOS
  text file). Up to 69 lines can be printed on each page half. Lines over 80
  characters are wrapped with an indicator in the margin instead of a line
  number.  An index file may be printed if an I=filename runstring option is
  given.  The file listed as an index is printed after all of the listing files
  are printed. Non printable characters can be printed as underlined byte
  values. Pages to print can be selected for two sided copy or destroyed page
  reprint. Manual feed and copies can be specified.

USAGE: lstprt optionlist filemask

valid options are:

Index=index file	(e.g. "I=files.lst")
	An index file is created listing all files in listing with page
	numbers and size information. This will be printed at the end of
	the listing.

Height=height of page (1 to 69 lines)	(e.g. "H=66")
	Normally up to 69 lines are printed on each half page. Long lines,
	lines made long by non printable codes, or formfeeds will shorten
	the number of lines printed. This option provides another means to
	reduce the number of lines printed on a half page.

Tabs=tab stop (1 to half line length)	(e.g. "T=4")
	Tabs are expanded. The default is an eight character expansion.
	This option allows setting tab expansion to a value that may be more
	suitable to the files being listed (T=4 is good for program listings).

Width=width of line (1 to 80 characters)	(e.g. "W=72")
	Up to 80 characters are printed on a line. This option will cause
	automatic line wrap at less than 80 characters.

Codes=None, Decimal, Octal, or Hex	(e.g "C=n")
	Non printable codes can be ignored or can be printed as underlined
	numeric codes in hex, decimal, or octal. Decimal is the default. 
	Setting codes=n will ignore codes and print the file as if they did
	not exist. (note that code 26 or control Z is an end of file and
	codes 8, 9, 10, 12, and 13 are interpreted).

Label=label	(e.g. "L=This_list_of_files_is_for_documentation")
	Label=footer_description prints a footer in the margin at the bottom
	left of each page. Since blanks are not allowed within an option,
	a multi-word footer must use periods, underscores, or a similar
	character between words instead of a space.

Pages=Odd, Even or All pages printed	(e.g. "P=O")
	This option allows printing on two sides of the paper in two runs.
	First select P=O. Then reset the paper and run with P=E to print
	the back sides (even numbered physical pages). If a page get jammed,
	use the STArt and STOp options to reprint only the destroyed pages.

STArt=first page to print
STOp=last page to print (1 to whatever)		(e.g. "STA=3 STO=5")
	Sets the physical (or paper) page number limits printed. This is
	intended to allow reprinting selected pages destroyed by paper
	jams or other accident.

COPies=number of copies		(e.g. "COP=2")
	Tells the printer to produce multiple copies of each page printed.

Feed=Manual feed	(e.g. "f=m")
	Tells the printer to use manual paper feed.

OUtput=filename		(e.g. "OUt=output.prn")
	Sets output to a device or file other than the standard printer.

FIles=file mask		(e.g. "f=c:\doc\*.txt")
	Explicitly states the files to be printed. This is not required as
	the last runstring argument that cannot be recognized as an option
	is assumed to be the file mask.

Query=Y		(e.g. "q=y")
	If set to Yes, the query option will ask if ok to continue after
	examining the runstring. Use this option if there is any doubt
	about the action of the listing command with options.

NEwline=decimal ASCII code	(e.g. "NE=31" for Borland Sprint *.spr files)
	Force a new line whenever this character code is found in the file
	being listed. Allows more reasonable printing of word processing
	files.

Mask=numvalue (e.g. "M=0x7F")
	A value that is 'AND'ed with each character printed. This is provided
	to facilitate printing of files such as those in Wordstar format.

TMargin=top margin lines (0 to 8) (e.g. "TO=3")
	Sets number of lines for top margin. This is to allow for printer
	variations and also adjusting margins as might be needed for hole
	punching.

PRinter=init_string	(e.g. "PR=\27\69")
	Do not use HP LJ II printer control codes. Sets H=62 and W=78.
	init_string is a printer setup string. A backslash digits in the
	init_string is converted to a code of the decimal value of the
	digits. For example, to set an IBM printer to 12 character per
	inch spacing, use the option "PRINTER=\27:". To set to draft
	quality print use "PRINTER=\27\73\1" on IBM printers. This option
	disables all margin data including headers, footers, and line
	numbering. Formfeeds are inserted between pages.

unknown options or any argument without an = is considered a file mask.
Only enough option name to uniquely identify it is required (in caps above).
If an option is duplicated, the last one in the runstring is used.

RETURNS: exits with a value of zero unless error in redirection or argument

CAVEATS: The filemask of files to print is a standard DOS filemask.
	Equal signs in the file mask should be avoided as there may be
	confusion with an option name.

	=>Only visible files can be found for listing.
	=>non printables near end of line in listing may cause line overflow.
	=>No blanks allowed in any option definition!

FILES:	input:		files derived from runstring file mask.
	 output:		PRN unless overridden
					NUL for pages not being printed
	err and status: standard error
	index:			as defined by I= runstring option.

SEE ALSO: PC Magazine laserlst.asm utility June 1989
	4PRINT shareware program.
	BLASER laser printer user's/programmer's guide.

REGISTRATION: A registration fee of $10 to
	Bryan R Leipper, 714 Terra Court, Reno, NV 89506
	would be appreciated.

	Comments, criticisms, and suggestions are welcome.
	There is no warranty of any sort on this software.
	Users should verify suitability before registering license to use.

HISTORY
	version A (initial beta test version) released 12 June 1989.
	version B 4 July 1989
		added start, stop, noprt, query, files, output options
		enhanced to allow flexible option names
	version C - 19 April 1990
		generic Microsoft C 5.1 (no additional support needed)
		added MASK and TOPMARGIN options

*/
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */

#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <limits.h>

/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */

#define MAX_PRT_ROWS 69

static int page_len = MAX_PRT_ROWS;
static int tab_stop = 8;		/* tabs expansion defaults to 8 */
static int line_len = 80;		/* lines wrap at 80 characters */
static int codes = 'd';			/* decimal, octal, hex, or none */
static int copies = 1;			/* copies of each page */
static int tray = 'n';			/* feed from normal or manual */
static int odd_even = 'a';		/* print odd, even, or all pages */
static int query = FALSE;		/* check if ok if TRUE */
static int noprt = FALSE;		/* printer control if FALSE */
static int nl_char = 257;		/* optional additional newline character */
static int charmask = 0xFF;		/* character AND mask value */
static int topmargin = 3;		/* lines top margin */
static char prt_init[24] = "\f";	/* substitute printer init string */
static unsigned int first_page = 1;
static unsigned int last_page = UINT_MAX;
static char *owner_line= NULL;
static char copyright[] =

	"lstprt version C (c) 1990 Bryan R Leipper, Reno NV";

FILE *index;

/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
typedef struct _FILE_ENT {
	char *path;
	char *drive;
	char name[_MAX_FNAME];
	char ext[_MAX_EXT];
	char atrb;		/* R A H S V D or default */
	int mo;		/* date and time values are integer values */
	int da;
	int yr;		/* add 1980 for full value */
	int hr;
	int mn;
	long sz;		/* this structure more complete than this particular */
} FILE_ENT;			/* program needs. (designed for file search utility) */
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */

/* printing functions */

int init_prtr(FILE_ENT *f);			/* printer and variable setup */
int reset_prtr(FILE_ENT *f);		/* printer reset */
int new_page(FILE_ENT *f);			/* file listing page control */
int new_line(FILE_ENT *f);			/* line to line control */
int prtr(FILE_ENT *f);				/* file output manager */
int make_index(char *idx_name, int idx_len, FILE_ENT *f);
									/* print index file listing */

/* support functions */

char *scan_str(char *dest, char *src);	/* translate escape sequences */
unsigned long lgetbits(unsigned long mask, int low_bit, int mask_len);
									/* pull bit value from long integer */
int file_parse(char *inpstr, char *fdrive, char *fpath, char *fname);

/* output file */

FILE *ofl;

/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
main(int argc, char *argv[], char *envp[]) {
/*
	list file to printer with HP LJ II+ capabilities
*/
	struct find_t fdat;
	FILE_ENT fl;
	char *c, *d, *ep
	,  fpath[_MAX_DIR]
	,  *mask
	,  idx_name[_MAX_PATH]
	,  fdrive[_MAX_DRIVE];
	int i, status, count;
	long tot_size;
	time_t tm_now;
	static unsigned attributes = _A_NORMAL | _A_RDONLY | _A_ARCH;
					/*	| _A_HIDDEN | _A_SYSTEM | _A_VOLID | _A_SUBDIR; */

	static char opt_list[] = {
		"iINDEX "		/* index file" */
		"tTABS "		/* tab stop (1 to half line length)" */
		"hHEIGHT "		/* height of page (1 to %d lines)",MAX_PRT_ROWS  */
		"wWIDTH "		/* Width of line (1 to 80 characters)" */
		"cCODES "		/* no, decimal, octal, or hex" */
		"lLABEL "		/* label" */
		"pPAGES "		/* odd, even or all pages printed\n" */
		"fFEED "		/* manual feed" */
		"aSTART "		/* first page to print */
		"bSTOP "		/* last page to print */
		"oOUTPUT "		/* output file name override */
		"#COPIES "		/* number of copies" */
		"qQUERY "		/* check if ok to continue */
		"dPRINTER "		/* printer (device) control */
		"sFILES "		/* file mask */
		"nNEWLINE "		/* additional newline character */
		"mMASK "		/* character AND mask */
		"uTMARGIN "		/* top margin lines */
	};	/* first of arg name in upper case must be first in list */

	static char *opt_desc[] = {
		 "\nvalid options are:"
		, "\n\tIndex=index file"
		, "\n\tHeight=height of page (1 to 69 lines)"
		, "\n\tTabs=tab stop (1 to half line length)"
		, "\n\tWidth=width of line (1 to 80 characters)"
		, "\n\tCodes=None, Decimal, Octal, or Hex"
		, "\n\tLabel=label"
		, "\n\tPages=Odd, Even or All pages printed"
		, "\n\tSTArt=first page to print"
		, "\n\tSTOp=last page to print"
		, "\n\tCOPies=number of copies"
		, "\n\tOUtput=output file name override"
		, "\n\tFeed=Manual feed"
		, "\n\tQuery=Yes, query for ok"
		, "\n\tFIles=file mask"
		, "\n\tNEwline=decimal ASCII code also counted as newline (0 - 255)"
		, "\n\tPRINTER=init_string, no printer control except init string"
		, "\n\tMask=numvalue, character AND mask value"
		, "\n\tTMargin=lines for top margin (0 to 8, defaults to 3)"
		, ""
	};

	static char dflt_opt[] = "sFILES ";

/* prep runstring parameters */
	memset(fpath, EOS, sizeof(fpath));
	memset(fdrive, EOS, sizeof(fdrive));
	memset(idx_name, EOS, sizeof(idx_name));
	memset(&fl, EOS, sizeof(fl));
	index = NULL;
	mask = NULL;
	ofl = stdprn;
	setmode(fileno(ofl), O_TEXT);

	if (argc > 0) {
		for (i = 1; i < argc; ++i) {
			d = strchr(argv[i], '=');		/* set for parameter */
			if (d == NULL) {		/* no arg delim, must be a file */
				d = argv[i];
				c = dflt_opt + 1;
			} else {
				*(d++) = EOS;		/* name and param segregated */
				strupr(argv[i]);
				c = argv[i];		/* find arg name in list */
				c = strstr(opt_list, c);
			}
			if (c == NULL) {
				d = argv[i];	/* must be a file name */
				c = dflt_opt;
			} else {
				c -= 1;
			}
			switch (*c) {
				case 'h' :			/* height */
					status = atoi(d);
					if ((status > 0) && (status < 70)) {
						page_len = status;
fprintf(stderr, "\npage length set to %d lines", page_len);
					}
				break;
				case 't' :			/* tabs */
					status = atoi(d);
					if ((status > 0) && (status < (line_len/2))) {
						tab_stop = status;
fprintf(stderr, "\ntabs set to %d spaces", tab_stop);
					}
				break;
				case 'w' :		/* width */
					status = atoi(d);
					if ((status > 0) && (status < 81)) {
						line_len = status;
fprintf(stderr, "\nline length set to %d chars", line_len);
					}
				break;
				case 'c' : 		/* codes handling */
					if (toupper(*d) == 'N') {
						codes = 'n';		/* ignore non printables */
fprintf(stderr,"\nnon printing codes being ignored");
					} else if (toupper(*d) == 'H') {
						codes = 'h';		/* print values hexadecimal */
fprintf(stderr,"\nnon printing codes printed as hex values");
					} else if (toupper(*d) == 'O') {
						codes = 'o';		/* print values octal */
fprintf(stderr,"\nnon printing codes printed as octal values");
					}	/* default is print codes decimal */
				break;
				case 'l' :		/* left side footer */
					owner_line = d;
fprintf(stderr,"\nleft footer set to %s",owner_line);
				break;
				case 'i' : 		/* index file */
					if ((index = fopen(d,"wt")) == NULL) {
fprintf(stderr,"\ncannot open index file %s", &argv[i][2]);
					} else {
						strcpy(idx_name, d);
fprintf(stderr,"\nindex file %s ready" ,idx_name);
					}
				break;
				case 'f' : 		/* feed override */
					if (toupper(*d) == 'M') {
						tray = 'm';
fprintf(stderr, "\nmanual feed specified");
					}
				break;
				case 'q' : 		/* query for ok  */
					if (toupper(*d) == 'Y') {
						query = TRUE;
					}
				break;
				case 'n' :		/* optional additional newline character */
					status = atoi(d);
					if ((status >= 0) && (status < 256)) {
						nl_char = status;
fprintf(stderr, "\nadditional newline char is %d", nl_char);
					}
				break;
				case 'u' :		/* top margin lines */
					status = atoi(d);
					if ((status >= 0) && (status < 8)) {
						topmargin = status;
fprintf(stderr, "\n topmargin set to %d lines", topmargin);
					}
				break;
				case 'm' : 		/* character AND mask */
					status = (int)strtol(d, &ep, 0);
					if (status) {
						charmask = status;
fprintf(stderr, "\ncharacter AND mask set to %d", charmask);
					}
				break;
				case 'd' : 		/* no printer codes  */
					noprt = TRUE;
					line_len = 78;
					page_len = 62;
					if (strlen(d) < sizeof(prt_init)) {
						scan_str(prt_init, d);
fprintf(stderr, "\nNo printer control codes (+60 lines x 78 char)");
					} else
fprintf(stderr, "\nInvalid printer init string. Formeed used instead");
				break;
				case 'p' : 		/* pages to print */
					if (toupper(*d) == 'O') {
						odd_even = 'o';
fprintf(stderr, "\nprint odd pages only");
					} else if (toupper(*d) == 'E') {
						odd_even = 'e';
fprintf(stderr, "\nprint even pages only");
					}
				break;
				case '#' :		/* copies */
					status = atoi(d);
					if ((status > 1) && (status < 255)) {
						copies = status;
fprintf(stderr, "\n%d copies specified", copies);
					}
				break;
				case 'a' :		/* first page to print */
					status = atoi(d);
					if ((status > 0) && (status < last_page)) {
						first_page = status;
fprintf(stderr, "\nstart print at page %d", first_page);
					}
				break;
				case 'b' : 		/* last page to print */
					status = atoi(d);
					if (status > 0) {
						last_page = status;
fprintf(stderr, "\nstop print at page %d", last_page);
					}
				break;
				case 'o' :		/* output redirection */
					if (freopen(d,"wt", ofl) == NULL) {
						fprintf(stderr,"\ncannot open output file %s",d);
						exit(-1);
					} else
						fprintf(stderr,"\noutput file %s",d);
				break;
				case 's' :		/* file mask (source) */
					mask = d;
					file_parse(d, fdrive, fpath, fl.name);
				break;
				default:
fprintf(stderr,"\n unknown option: %s = %s", argv[i], d);
					query = TRUE;
				break;
			}		/* argument list parse case */
		}		/* argument examination loop */
	} 		/* have runstring arguments */

	if (mask == NULL) {
		fprintf(stderr, "\nusage: %s options filemask",argv[0]);
		for (status = 0; *opt_desc[status];
			fprintf(stderr, "%s", opt_desc[status++]));
		fprintf(stderr, "%s", 
"\nunknown options or any argument without an = is considered a file mask."
"\nOnly enough option name to uniquely identify it is required (in caps above)."
		);
		fprintf(stderr,"\n\n%s\n\n",copyright);
		exit(-1);
	} 

	if (owner_line == NULL)		/* install default owner line */
		owner_line = mask;

	if (query) {
		fprintf(stderr,"\nlist %s",mask);
		fprintf(stderr,"\nOK to continue (Y or N)?");
		i = getchar();
		if (toupper(i) != 'Y') {
			exit(0);
		}	
	}

/* initialize */
	fl.path = fpath;
	fl.drive = fdrive;
	init_prtr(&fl);		/* initialize printer */

	time(&tm_now);
	fprintf(stderr, "\nprint 2 up listings");
	if (noprt == FALSE)
		fprintf(stderr, " to HPLJ II compatible");
	fprintf(stderr, " %s", ctime(&tm_now));

	if (index != NULL) {
		fprintf(index,"\nlisting printed: %s\n", ctime(&tm_now));
		fprintf(index,"\n  file      on page lines (pages) listed\n");
	}

/* read directory of files */
	status = _dos_findfirst(mask, attributes, &fdat);
	count = 0;
	tot_size = 0;
	printf("\n");
	while (status == 0) {
		count += 1;
		tot_size += fdat.size;
		fl.sz = fdat.size;
										/* segregate name and extent */
		for (c = fdat.name, i=0; (*c != EOS) && (*c != '.'); ++c) 
			fl.name[i++] = *c;
		fl.name[i] = EOS;
		if (*c != EOS) {
			for (++c,i=0; (*c != EOS) && (i < (sizeof(fl.ext)-1)); ++c) 
				fl.ext[i++] = *c;
		} else
			i = 0;
		fl.ext[i] = EOS;

		fl.hr = (int)lgetbits((unsigned long)fdat.wr_time, 11, 5);
		fl.mn = (int)lgetbits((unsigned long)fdat.wr_time, 5, 6);
		fl.mo = (int)lgetbits((unsigned long)fdat.wr_date, 5, 4);
		fl.da = (int)lgetbits((unsigned long)fdat.wr_date, 0, 5);
		fl.yr = (int)lgetbits((unsigned long)fdat.wr_date, 9, 7);
		if ((status = prtr(&fl)) != 0)
			fprintf(stderr,"\n error %d reading file %s%s%s.%s", status
				, fl.drive, fl.path, fl.name, fl.ext);
		status =_dos_findnext(&fdat);
	}

/*	if (status != 18) fprintf(stderr,"\n find exit status = %d\n",status); */

	if (index != NULL) {
		fclose(index);
		make_index(idx_name, sizeof(idx_name), &fl);
	}

/* reset printer to original conditions and shut down */
	reset_prtr(&fl);

	if (count == 0) {
		fprintf(stderr,"\nno files matching %s",mask);
		fprintf(stderr,"\nusage: %s optionlist filemask",argv[0]);
		fprintf(stderr,"\nrun with no arguments for assistance\n");
	} else {
		fprintf(stderr,"\n%d files occupying %ld bytes\n",count, tot_size);
	}
	fprintf(stderr,"\n%s\n",copyright);

	exit(0);
}
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */

/* printer management and output routines */

int prt_col		/* column being printed (0 < prt_col <= max) */
	, prt_row		/* row being printed (0 < prt_row < max) */
	, prt_page		/* indicates odd or left col and even or right col */
	, file_page		/* file page < prt_page < paper_page */
	, file_line		/* line in file */
	, paper_page; 	/* pieces of paper in print run */

FILE *evnout, *oddout, *nulout, *ctrl;

/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
int make_index(char *idx_name, int idx_len, FILE_ENT *f) {
	int status, i;
	char *c, *d, fpath[64];

	memset(fpath, EOS, sizeof(fpath));
	for (c = idx_name, d=NULL; *c; ++c) {
		if (*c == '\\') { /* convert all backslashes to forward slashes */
			*c = '/';
			d = c;
		} else if (*c == ':') 
			d = c;		/* trap end of path */
	}
	if (d != NULL) {		/* pull path name for file opens */
		strncpy(fpath, idx_name, (d - idx_name) + 1);
		++d;
	} else 
		d = idx_name;

	f->path = fpath;
									/* segregate name and extent */
	for (c=d, i=0; (*c != EOS) && (*c != '.'); ++c) 
		f->name[i++] = *c;
	f->name[i] = EOS;
	if (*c != EOS) {
		for (++c,i=0; (*c != EOS) && (i < (sizeof(f->ext)-1)); ++c) 
			f->ext[i++] = *c;
	} else
		i = 0;
	f->ext[i] = EOS;

	f->sz = 0;

/*
	fprintf(stderr,"\nindex input %s",idx_name); 
	fprintf(stderr,"\nindex path %s",f->path); 
	fprintf(stderr,"\nindex file %s",f->name); 
	fprintf(stderr,"\nindex ext %s",f->ext); 
*/

	status = strlen(idx_name) + strlen(owner_line) + 5;
	if (status < idx_len) {
		strcat(idx_name," for ");
		strcat(idx_name,owner_line);
		owner_line = idx_name;
	}

	prt_page = 0;
	paper_page = 0;
	putc('\f', ofl);
	status = prtr(f);
	return(status);
}
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
int init_prtr(FILE_ENT *f) {
/*
set ofl to text mode output and send setup string to printer.
			"\033&s1C"			disable end of line wrap
			"\033&l0L"			disable perf skip mode
			"\033&l",n,"X"		make n copies per page
			"\033&l2H"			feed paper from manual slot

*/
	char str[32];
	int i, j;

/* set file stream pointers for odd and even pages and control */
	ctrl = ofl;
	nulout = fopen("NUL","wt");
	if (odd_even == 'o') {
		evnout = nulout;
		oddout = ofl;
	} else if (odd_even == 'e') {
		evnout = ofl;
		oddout = nulout;
	} else {
		evnout = ofl;
		oddout = ofl;
	}		/* ofl always starts out pointing to output file (not NULL) */

	prt_page = 0;
	paper_page = 0;

	if (noprt) {
		fprintf(ctrl, "%s", prt_init);
		return(FALSE);
	}

/* set printer parameters */
	fprintf(ctrl, "\033E"
			"\033&l1O"			/* landscape */
			"\033(s17H"			/* primary font pitch = 17 cpi */
			"\033&l5.14c"		/* c vmi = 5.14/48 in */
			"%de"				/* e top margin lines */
			"71F"				/* F text length = 71 lines */
			"\033(s-3B"			/* light stroke weight */
			"\033&k7H"			/* hmi in 120th inch (7 =171.4 chr / 10 in) */
			"\r", topmargin);

	if (copies > 1)
		fprintf(ctrl, "\033&l%uX", copies);
	
	if (tray == 'm')
		fprintf(ctrl, "\033&l2H");

	return(FALSE);
}
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
int reset_prtr(FILE_ENT *f) {

	if (noprt) {
		fprintf(ctrl, "%s", prt_init);
		return(FALSE);
	}

	fprintf(ctrl, "\033E");
	return(FALSE);
}
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
int new_page(FILE_ENT *f) {
/*
called only by new line routine that takes care of line header after
this is called. This routine prints shaded areas for headers, footers, and
line header areas, prints page headers and footers as appropriate.
increments file page, prt page, and paper page, resets prt col and prt row.
*/

	++file_page;
	prt_col = 1;
	prt_row = 1;

	if ((++prt_page % 2) == 0) {		/* prep for right hand side */
		if (noprt) {
			putc('\f', ofl);
			return(FALSE);
		} else
			fprintf(ofl,"\0339\033&a174m88L\r"); /* rhs: ff, lm=88, rm=174 */
	} else {
		if (paper_page != 0)
			putc('\f', ofl);

		paper_page += 1;
		if ((paper_page < first_page) || (paper_page > last_page))
			ofl = nulout;
		else if ((paper_page % 2) == 0)
			ofl = evnout;
		else
			ofl = oddout;

		if (noprt) 
			return(0);

		fprintf(ofl, "\033&a174m1L\r"	/* rm = 174; lm = 1 */
				"\033*t300R"			/* raster graphics resolution */
				"\033*r0A"				/* start graphics at left margin */
				"\r");

/* title line box */
		fprintf(ofl,"\033*c15g3132a33B"	/* 15% gray scale, wh in dots */
				"\033*p0X"				/* set horiz col */
				"\033*p0Y"				/* set vertical row */
				"\033*c2P\r");			/* print gray scale pattern */

/* ownership line box */
		fprintf(ofl, "\033*p0x2245Y"	/* set vertical row */
				"\033*c2P\r");			/* print gray scale pattern */

/* lhs number box */
		fprintf(ofl,"\033*c106a2250B"	/* size in dots */
				"\033*p0x0Y"			/* set xy in dots */
				"\033*c2P\r");			/* print gray scale pattern */

/* rhs number box */
		fprintf(ofl,	"\033*p1522x0Y"	/* set xy in dots */
				"\033*c2P\r");			/* print gray scale pattern */

		fprintf(ofl, "\033*rB\r");		/* turn off graphics mode */

/* print footer */
		fprintf(ofl,	"\033&a70R\r");		/* set row 70 at left margin */
		fprintf(ofl, "      %-75s page %-5d %78s"
			, owner_line, paper_page, copyright);

/* left hand side initialization */
		fprintf(ofl,"\0339\033&a87m1L\r"); /* lhs: ff row 0, lm=1, rm=87 */
	}

/* print page header */
	fprintf(ofl,	"\033&a0R\r");		/* set row at left margin */
	fprintf(ofl,
"      %8s.%-3s last modified %02d:%02d %02d/%02d/%02d %6ld bytes %20s %d\n"
		, f->name, f->ext, f->hr , f->mn, f->mo , f->da , 80+f->yr, f->sz
		, " file page", file_page
	);
	return(0);
}
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
int new_line(FILE_ENT *f) {

	if (++prt_row > page_len) {
		new_page(f);
	} else {
		putc('\n',ofl);
	}
	file_line += 1;
	if (noprt == FALSE)
		fprintf(ofl,"%5d ",file_line);
	prt_col = 1;
	return(0);
}
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
int prtr(FILE_ENT *f) {

	FILE *ifd;
	int inp_chr, start_page, status;
	long chr_cnt;
	
	char openstr[127];

	if (strlen(f->drive) > 0) 
		strcpy(openstr, f->drive);
	else 
		*openstr = EOS;

	if (strlen(f->path) > 0) 
		strcat(openstr, f->path);

	strcat(openstr, f->name);
	strcat(openstr, ".");
	strcat(openstr, f->ext);

/*	fprintf(stderr,"\nopen %s",openstr); */

	if ((ifd = fopen(openstr,"rt")) == NULL) {
		fprintf(stderr,"\ncannot open file %s",openstr);
		return(-1);
	}

	file_page = 0;
	file_line = 0; 
	chr_cnt = 0;
	prt_row = page_len;
	new_line(f);
	start_page = paper_page;

	inp_chr = fgetc(ifd);
	while (inp_chr != EOF) {
		if ((status = ferror(ifd)) != 0) {
			fclose(ifd);
			return(status);
		}
		inp_chr = charmask & inp_chr; 
		if ((prt_col > line_len) && (strchr("\b\n\r\f",inp_chr) == NULL)) {
			if (++prt_row > page_len) 	/* line overflow */
				new_page(f);
			else 
				putc('\n',ofl);
			if (noprt == FALSE)
				fprintf(ofl," =>  ");
			prt_col = 1;
		}
		++chr_cnt;
		switch (inp_chr) {
		case '\t' :		/* tab */
			do { putc(BLANK, ofl);
			} while (((++prt_col % tab_stop) != 1) && (prt_col < line_len));
		break;
		case '\b' :		/* backspace */
			putc(inp_chr, ofl);
			--prt_col;
		break;
		case '\f' :		/* formfeed */
			prt_row = page_len;
			new_line(f);
		break;
		case '\n' :		/* newline */
		case '\r' :		/* carriage return */
			new_line(f);
		break;
		default :
			if (inp_chr == nl_char)			/* optional additional */
				new_line(f);
			else if (isprint(inp_chr) ) {
				putc(inp_chr, ofl);
				++prt_col;
			} else if (noprt == FALSE) {
				if (codes == 'h') { /* make sure space left on line ??  */
					fprintf(ofl, "\033&dD%2X\033&d@ ",inp_chr); 
					prt_col += 4;
				} else if (codes == 'd') {
					fprintf(ofl, "\033&dD%3u\033&d@ ",inp_chr); 
					prt_col += 5;
				} else if (codes == 'o') {
					fprintf(ofl, "\033&dD%3o\033&d@ ",inp_chr); 
					prt_col += 5;
				}
			}
		break;
		}
		inp_chr = fgetc(ifd);
	}
	if ((++prt_row < MAX_PRT_ROWS) && (noprt == FALSE)) {
		fprintf(ofl, "\n EOF:   \033&d3D%8s.%3s  %5d lines, %ld characters"
				, f->name, f->ext, file_line, chr_cnt );
		if (codes == 'n')
			fprintf(ofl, " (codes ignored)");
		else if (codes == 'h')
			fprintf(ofl, " (codes values hexadecimal)");
		else if (codes == 'o')
			fprintf(ofl, " (codes values octal)");
		fprintf(ofl, "\033&d@");
	}

	fclose(ifd);

	fprintf(stderr, "%8s.%-3s p %4d; %5d lines (%d page halves) \n"
			, f->name, f->ext, start_page, file_line, file_page);

	if (index != NULL)
		fprintf(index , "%8s.%-3s %4d; %5d (%d)\n"
				, f->name, f->ext, start_page, file_line, file_page);

	return(0);
}

/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
int file_parse(char *inpstr, char *fdrive, char *fpath, char *fname) {
/*
	inpstr parsed into fpath and fname 
	string sizes assumed to be:
	 _MAX_FNAME	9	max. length of file name component 
	 _MAX_EXT	5	max. length of extension component 
	 _MAX_DIR	130	max. length of path component 
	 _MAX_DRIVE	3	max. length of drive component 
	_MAX_PATH	144	max. length of full pathname 

*/
	char *c, *d, *p;

	/* fprintf(stderr,"\n parse %s",inpstr); */

	for (c = inpstr, d=p=NULL ; *c; ++c) {
		if (*c == '\\') {
			*c = '/';		/* convert all backslashes to forward slashes */
			d = c;
		} else if (*c == ':') {
			p = c;			/* trap end of path */
		} else if (*c == '/') {
			d = c;
		}
	}
	if (p != NULL) {
		strncpy(fdrive, inpstr, min((p-inpstr)+1, _MAX_DRIVE));
		/* fprintf(stderr,"\n drive: %s",fdrive);  */
		inpstr = p + 1;
	} else {
		fdrive[0] = EOS;
	}
	if (d != NULL) {		/* pull path name for file opens */
		strncpy(fpath, inpstr, min((d-inpstr)+1, _MAX_DIR));
		inpstr = d + 1;
		/* fprintf(stderr,"\n path: %s",fpath);  */
	} else {
		fpath[0] = EOS;
	}

	/* fprintf(stderr,"\n name: %s",fname); */
	strcpy(fname, inpstr);

}	
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
/* scanstr.c (c) 1990 Bryan R Leipper

SYNOPSIS: copy string replacing escape sequences
DESCRIPTION:
	copy src to dest replacing escape sequences as found
	escape sequences are indicated by a backslash and followed by strtol
	function input values using an implied base (0x for hex 0[1-7] for
	octal [1-9]* for decimal. If value comes up as zero, then try for
	standard codes. Finally just dump character following backslash as is.
RETURNS: pointer to end of destination string
CAVEATS: make sure dest string long enough for output
FILES: none
NOTES:
SEE ALSO: strtol standard C library function
HISTORY: Feb 1988	original version
	Feb 1990 use strtol implicit base directly
*/
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
char *scan_str(char *dest, char *src) {
char *endptr;
long esc_code;

	while (*src) {
		if (*src == '\\') {
			esc_code = strtol(src, &endptr, 0);
			if (esc_code) {		/* non zero result stashed */
				*(dest++) = (char)esc_code;
				src = endptr;
			} else switch (*src) {
				case '0' : *(dest++) = 0; break;
				case 'a' : *(dest++) = '\a'; break;
				case 'b' : *(dest++) = '\b'; break;
				case 'f' : *(dest++) = '\f'; break;
				case 'n' : *(dest++) = '\n'; break;
				case 'r' : *(dest++) = '\r'; break;
				case 't' : *(dest++) = '\t'; break;
				case 'v' : *(dest++) = '\v'; break;
				default	 : *(dest++) = *(src++); break;
			}
		} else {
			*(dest++) = *(src++);
		}
	}
	*dest = 0;
	return(dest);
}
/* =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= =-=-= */
unsigned long lgetbits(unsigned long mask, int low_bit, int mask_len) {
/*
	returns value of selected bits in long integer
*/
	unsigned long i;
	int j, intlen = sizeof(long int) * 8		/* BITSPERBYTE */ ;

	j = intlen - (low_bit + mask_len);
	i = mask << j;
	j += low_bit;
	i >>= j;
	return (i);
}
