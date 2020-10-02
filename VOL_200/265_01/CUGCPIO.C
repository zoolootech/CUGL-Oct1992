/*
 *	c u g c p i o . c
 *	-----------------
 *	The CUG Data Exchange Format Archive Creator/Extractor.
 *
 *	Written by	Rainer Gerhards
 *			Petronellastr. 6
 *			D-5112 Baesweiler
 *			West Germany
 *
 *			Phone (West Germany) 2401 - 1601
 *
 *	release history
 *	---------------
 *	Mar 26, 1988	begin implementation
 *	Nov 16, 1988	switches d and p added
 */
#include "environ.h"
#include <stdio.h>
#include <ctype.h>


/*
 *	preprocessor constants
 *	----------------------
 */
#define	VERSION		2		/* current program version	*/
#define	MAGIC		070707		/* magic archive number		*/
#define	TRAILER		"TRAILER!!!"	/* trailer file name		*/
#define	MAKEFILE	"makefile"	/* name of makefiles		*/
#define	FILLCHR		'\032'		/* block fill character		*/
#define	MAXNMSZ		100		/* longest possible file name	*/
#define	BUFSIZE		512		/* C-library max buffer size	*/
#define	TABCOLS		8		/* OS tab settings		*/
#define	BLOCKSZ		512		/* output block size		*/

/*
 * The following defines describe the processing modes. They are
 * mainly used to avoid enums.
 */
#define	CREATE		0
#define	EXTRACT		1
#define	CATALOG		2

/*
 *	static data
 *	-----------
 */
STATIC FILE	*archive;		/* archive file pointer		*/
STATIC char	opn_mode[8];		/* archive file open mode	*/
STATIC int	procmode = CATALOG;	/* processing mode		*/
STATIC int	verbose = FALSE;	/* verbose output		*/
STATIC int	silent = FALSE;		/* totally silent mode		*/
STATIC int	exptab = FALSE;		/* expand tabulation stops	*/
STATIC int	convert = FALSE;	/* do several conversions	*/
STATIC int	makeflok = FALSE;	/* look for makefiles		*/
STATIC int	ignpath = FALSE;	/* ignore path names in archive */
STATIC int	destroy = FALSE;	/* destroy existing files	*/

/*
 *	functions
 *	---------
 */


/*---------------------------------------------------------------------------
 * Display usage information. This function terminates the program!
 */
void		usage()
{
fprintf(stderr, "Usage:\tcugcpio -[i|o|t][cdempsv] archive [files...]\n");
fprintf(stderr, "\nprocessing modes:\n");
fprintf(stderr, "i\textract files\n");
fprintf(stderr, "o\tcreate archive\n");
fprintf(stderr, "t\tlist archive directory\n");
fprintf(stderr, "\nprocessing modifiers (options):\n");
fprintf(stderr, "c\tdo several conversions\n");
fprintf(stderr, "d\tsilently overwrite existing files on extract\n");
fprintf(stderr, "e\texpand horizontal tabs (input only)\n");
fprintf(stderr, "m\tdon't expand HTs in makefiles\n");
fprintf(stderr, "p\tignore stored path name\n");
fprintf(stderr, "s\tsilent operation - no messages displayed\n");
fprintf(stderr, "v\tverbose listing\n");
exit(2);
/*NOTREACHED*/
}


/*---------------------------------------------------------------------------
 * Check if file is existent. We assume it isn't, if we can't open it.
 * This may not be the safest method, but it should be the most portable one.
 */
int		exist(file)
char		*file;
{
FILE		*fp;
int		ret;

ret = ((fp = fopen(file, OPM_RT)) == NULL) ? 0 : 1;
if(ret)
	fclose(fp);
return(ret);
}


/*---------------------------------------------------------------------------
 * Test if character is pathname-delimiter. Returns 1 if yes, 0 otherwise.
 */
STATIC int	pathdelm(c)
int		c;
{
int		state;

if((c == '/') || (c == '\\'))
	state = 1;	/* delemiter found!	*/
else
	state = 0;
return(state);
}


/*---------------------------------------------------------------------------
 * Get filename only (path stripped).
 */
STATIC char	*nameonly(cp)
register char	*cp;
{
register char	*orgcp;

for(orgcp = cp ; *cp ; ++cp)
	;		/* search end of string		*/
for( ; !pathdelm(*cp) && (cp != orgcp) ; --cp)
	;		/* search path delemiter	*/
if(pathdelm(*cp))
	++cp;
return(cp);
}


/*---------------------------------------------------------------------------
 * Get pathname only (filename stripped).
 */
STATIC char	*pathonly(cp)
char		*cp;
{
register int	i;
register char	*endcp;
static char	pathname[MAXNMSZ];

endcp = nameonly(cp);
for(i = 0 ; cp != endcp ; ++cp, ++i)
	pathname[i] = *cp;
pathname[i] = '\0';
return(pathname);
}


/*---------------------------------------------------------------------------
 * Convert String to lower case.
 */
STATIC void	lowerstr(cp)
register char	*cp;
{
for( ; *cp ; ++cp)
	if(isupper(*cp))
		*cp = tolower(*cp);
}


/*---------------------------------------------------------------------------
 * Set operation modes according to switch characters. If an error is
 * detected in them the usage message is displayed and the program terminates.
 */
STATIC void	setmodes(sw)
register char	*sw;
{
while(*sw)
	switch(*sw++)
		{
		case 'o':	procmode = CREATE;	/* output	*/
				strcpy(opn_mode, OPM_WB);
				break;
		case 'i':	procmode = EXTRACT;	/* input	*/
				strcpy(opn_mode, OPM_RB);
				break;
		case 't':	procmode = CATALOG;	/* catalog	*/
				strcpy(opn_mode, OPM_RB);
				break;
		case 'v':	verbose = TRUE;		/* verbose 	*/
				break;
		case 's':	silent = TRUE;		/* total silent	*/
				break;
		case 'c':	convert = TRUE;		/* conversion	*/
				break;
		case 'e':	exptab = TRUE;		/* expand HT	*/
				break;
		case 'm':	makeflok = TRUE;	/* makefile opt	*/
				break;
		case 'p':	ignpath = TRUE;		/* ignore path	*/
				break;
		case 'd':	destroy = TRUE;		/* destroy files*/
				break;
		default:	usage();
				/*NOTREACHED*/
		}
}


/*---------------------------------------------------------------------------
 * Write archive file header
 */
void		wrthead(name, textsize)
register char	*name;
long		textsize;
{
fprintf(archive,
       "%6.6o%6.6o%6.6o%6.6o%6.6o%6.6o%6.6o%6.6o%11.11lo%6.6o%11.11lo",
       MAGIC, 0, 0, 0100666, 0, 0, 1, 0, 0l, strlen(name) + 1, textsize);
for( ; *name ; ++name)
	fputc(((convert == TRUE) && islower(*name)) ? toupper(*name) : *name,
	       archive);
fputc('\0', archive);
}


/*---------------------------------------------------------------------------
 * Read archive file header. Returns TRUE if read head was trailer record
 * otherwise FALSE. If the magic number is incorrect, an error message is
 * generated and the program terminated.
 */
STATIC int	get_head(name, textsize)
char		*name;
long		*textsize;
{
register char	*cp;
int		magic;
int		namesize;
int		retval;

fscanf(archive, "%6o%*6o%*6o%*6o%*6o%*6o%*6o%*6o%*11lo%6o%11lo",
      &magic, &namesize, textsize);
if(magic == 070707)
	{
	cp = name;
	while(namesize--)
		*cp++ = fgetc(archive);
	*cp = '\0';
	}
else
	{
	fprintf(stderr, "ERROR: Archive file inconsistency, magic = %6.6o\n");
	exit(1);
	}
retval = ((*textsize == 0l) && (!strcmp(name, TRAILER))) ? TRUE : FALSE;
if(convert == TRUE)
	lowerstr(name);
return(retval);
}


/*---------------------------------------------------------------------------
 * Skip n bytes of the input archive file.
 */
STATIC void	skipinp(n)
long		n;
{
fseek(archive, n, 1);
}


/*---------------------------------------------------------------------------
 * Write single file to archive.
 */
void		wrtout(fil)
char		*fil;
{
register FILE	*fp;
register int	c;
int		i;
int		curcol = 0;
long		headpos;
long		textsize = 0l;
long		nonascii = 0l;
long		nonprint = 0l;

if((fp = fopen(fil, "r")) == NULL)
	fprintf(stderr, "ERROR: Can't open %s\n", fil);
else
	{
	headpos = ftell(archive);
	/* The following written dummy header becomes updated at end of
	   processing. The actual file size isn't known at this moment. */
	wrthead(fil, 0l);
	/* copy file */
	while((c = fgetc(fp)) != EOF)
		{
		if((c == '\t') && (exptab == TRUE))
			{
			i = TABCOLS - (curcol % TABCOLS);
			textsize = textsize + i;
			curcol += i;
			while(i--)
				fputc(' ', archive);
			}
		else
			{
			if(c == '\n')
				curcol = 0;
			else
				++curcol;
			if(!isascii(c))
				++nonascii;
			else if(!isprint(c) && (c != '\n'))
				++nonprint;
			fputc(c, archive);
			++textsize;
			}
		}
	/* copy done, update header */
	fseek(archive, headpos, 0);	/* go back to update header	*/
	wrthead(fil, textsize);
	fseek(archive, 0l, 2);		/* back to eof			*/
	if(   (silent == FALSE)
	   && ((verbose == TRUE) || (nonascii > 0l) || (nonprint > 0l))
	  )
		{
		fprintf(stderr, "%s: %ld byte", fil, textsize);
		if(nonascii > 0l)
			fprintf(stderr, ", %ld non-ascii", nonascii);
		if(nonprint > 0l)
			fprintf(stderr, ", %ld non-printable", nonprint);
		fprintf(stderr, ".\n");
		}
	}
fclose(fp);
}


/*---------------------------------------------------------------------------
 * Write trailer record.
 */
void		wrttrail()
{
int	i = 512;	/* for compatibility with Unix cpio		*/

wrthead(TRAILER, 0l);
i = BLOCKSZ - ftell(archive) % BLOCKSZ;
while(i--)
	fputc(FILLCHR, archive);
}


/*---------------------------------------------------------------------------
 * Print archive file table of contents. No output files are created.
 */
STATIC void	prnt_toc()
{
long		size;
char		name[MAXNMSZ];

if(verbose == TRUE)
	printf("  Size - Name\n");
while(get_head(name, &size) != TRUE)	/* while(not TRAILER read)	*/
	{
	if(ignpath)
		printf("%6ld - [%s] %s\n", size, pathonly(name),
			nameonly(name));
	else
		printf("%6ld - %s\n", size, name);
	skipinp(size);
	}
}


/*---------------------------------------------------------------------------
 * Extract whole archive file. The extracted files are ctreated in the
 * current working directory.
 */
STATIC void	extract()
{
register FILE	*fp;
long		size;
char		*fname;
char		name[MAXNMSZ];
char		linebuf[BUFSIZE];

while(get_head(name, &size) != TRUE)	/* while(not TRAILER read)	*/
	{
	if(verbose == TRUE)
		if(ignpath)
			printf("[%s] %s: %ld bytes\n", pathonly(name),
				nameonly(name), size);
		else
			printf("%s: %ld bytes\n", name, size);
	if(ignpath)
		fname = nameonly(name);
	else
		fname = name;
	if((destroy == FALSE) && exist(fname))
		{
		fprintf(stderr, "File %s exists - overwrite (y/n)? ", fname);
		gets(linebuf);
		if(tolower(*linebuf) != 'y')
			{
			/*
			 * file can't be written, so 1. skip input,
			 * 2. start new cycle.
			 */
			skipinp(size);
			continue;	 
			}
		}
	if((fp = fopen(fname, OPM_WT)) == NULL)
		{
		fprintf(stderr, "ERROR: Can't output to %s - skipped\n", name);
		skipinp(size);
		}
	else
		{
		while(size--)
			fputc(fgetc(archive), fp);
		fclose(fp);
		}
	}
}


/*---------------------------------------------------------------------------
 * control module
 */
int		main(argc, argv)
int		argc;
char		**argv;
{
int		exptabsv;

if(argc == 1)
	usage();
--argc;					/* programm name not used	*/
if(**++argv == '-')			/* switch character given?	*/
	setmodes((*argv) + 1);		/* yes -> set modes		*/
else
	usage();			/* no -> terminate		*/
if(silent == TRUE)			/* silent mode overrides	*/
	verbose = FALSE;		/* verbose mode			*/
if(verbose == TRUE)
	{
	fprintf(stderr,
	      "The CUG Data Exchange Format Archive File Creator/Extractor.\n");
	fprintf(stderr, "Written by Rainer Gerhards, Version %d.\n\n", VERSION);
	}
--argc;					/* switches processed		*/
if((archive = fopen(*++argv, opn_mode)) == NULL)
	{
	fprintf(stderr, "ERROR: Can't open/create archive file %s\n");
	exit(1);
	/*NOTREACHED*/
	}
if(procmode == CREATE)
	{
	while(--argc)
		{
		++argv;			/* next argument		*/
		if((makeflok == TRUE) && !strcmp(*argv, MAKEFILE))
			{
			exptabsv = exptab;
			exptab = FALSE;
			wrtout(*argv);
			exptab = exptabsv;
			}
		else
			wrtout(*argv);
		}
	wrttrail();
	}
else if(procmode == CATALOG)
	prnt_toc();
else if(procmode == EXTRACT)
	extract();
else
	{
	fprintf(stderr, "ERROR: Program logic fault, invalid procmode %d\n",
		procmode);
	exit(1);
	}
fclose(archive);
return(0);
}
