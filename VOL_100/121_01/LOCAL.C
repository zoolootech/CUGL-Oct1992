/*
HEADER: CUG 121.??;

    TITLE:	local - misc functions used on this disk;
    VERSION:	1.0;
    DATE:	12/01/85;
    DESCRIPTION: "This file contains functions used by call.c, if.c, tel.c
		and tl370.c.  The following functions are included:
		abort(), barf(), sign(), stacktop(), strncat(), strncmp(),
		strncpy(), strlower(), strupper(), strindex(), index(),
		findline(), getdph(), extract(), mputs(), mputc(), moready(),
		mgetc(), miready(), hayesput(), hayesget(), globarg(),
		posarg()";
    KEYWORDS:	functions;
    SYSTEM:	CP/M;
    FILENAME:	LOCAL.C;
    WARNINGS:	"This file contains some functions that are not used by
		anything on this disk, but may be of general interest or use.";
    AUTHORS:	Mike W. Meyer, Steve Kenton;
    COMPILERS:	BDS-C 1.50;
*/
/*
 * local.c - the local library of C functions for BDS C.
 *	copyright (c) 1981, Mike Meyer
 *	parts copyright (c) 1982, Steve Kenton
 *		modified stylewise to fit my conventions - mwm
 *
 *	modified by rfb (Dec '85) -
 *		changed modem i/o to use v1.50 #defines, added mgetc()
 *		changed stacktop() to use topofmem()
 *		modified style & made it consistent throughout (apologies to
 *		anyone offended)
 *		added #include for v1.50 hardware values
 */

#include <bdscio.h>
#include <hardware.h>
#include "local.h"

/*
 * barf - exit with a non-zero value, possible printing a message
 */
barf(chuck)
char *chuck;
{
	abort(chuck);
}

/*
 * abort - the real body of barf, used by the squeamish.
 */
abort(charles)
char *charles;
{
	if (charles)
		puts(charles);
	exit(1);
}

/*
 * sign - return 0, 1, -1 depending on the sign of x
 */
int sign(x)
{
	return (x > 0) - (x < 0);
}

/*
 * stacktop - just returns address of the top of the stack.
 *	uses v1.50 function topofmem()
 */
char *stacktop()
{
	return topofmem();
}

/*
 *	strncpy - copy a substring up to N characters long
 */
strncpy(str1, str2, n)
char *str1, *str2;
unsigned n;
{
	while (n-- && *str2)
		*str1++ = *str2++;
	*str1 = NULL;
}

/*
 * strncat - cat a substring of up to N characters
 */
strncat(str1, str2, n)
char *str1, *str2;
unsigned n;
{
	while (*str1++)
		;
	strncpy(str1 - 1, str2, n);
}

/*
 *	strncmp - compare a substring up to N characters long
 */
int strncmp(str1, str2, n)
char *str1, *str2;
unsigned n;
{
int i;
	while (n--)
		if (!*str1 && !*str2)
			break;
		else if (i = *str2++ - *str1++)
			return(i);
	return(0);
}

/*
 * strlower - map a string to lower case
 */
char *strlower(string)
char *string;
{
char *x;

	x = string;
	do
		*x = tolower(*x);
	while (*++x);
	return string;
}

/*
 * strupper - map a string to upper case
 */
char *strupper(string)
char *string;
{
char *x;

	x = string;
	do
		*x = toupper(*x);
	while (*++x);
	return string;
}

/*
 *	strindex - return the index of substr in str or error
 */
unsigned strindex(str, substr)
char *str, *substr;
{
int i, len, limit;
char save, *savloc;	/* not needed if using strncmp */

	len = strlen(substr);
	limit = strlen(str) - len + 1;
	for (i = 0; i < limit; i++) {
		savloc = str + len;
		save = *savloc;		/* so save the current value */
		*savloc = '\0';		/* and then terminate the substring */
		if (strcmp(str, substr) == 0) {
			*savloc = save;	/* restore the saved character */
			return(i);	/* and return the index */
		}
		*savloc = save;		/* restore the saved character */
		str++;	/* otherwise, increment the start address and repeat */
	}
	return(ERROR);			/* not found, so return ERROR */
}

/*
 *	index - return the index of the first occurrence of C
 *	in str or error if it is not found
 */
unsigned index(str, c)
char *str, c;
{
int i;

	for (i = 0; *str; i++)
		if (*str++ == c)
			return(i);	/* found it */
	return(ERROR);			/* did not find it */
}

/*
 * findline - this routine takes a BDS C file open for formatted I/O,
 *	a buffer buf, and a string.  It returns, in buf, the first line
 *	in the file that matches the string, up to either a tab, a space,
 *	or a semicolon in the line from the file.
 */
int findline(file, line, pat)
char *file, *line, *pat;
{
char *lp, *pp;

	while (fgets(line, file))
		for (lp = line, pp = pat; ; pp++, lp++)
			if (!*pp && index("\t :", *lp) != ERROR)
				return TRUE;
			else if (*pp != *lp || !*pp)
				break;
	return FALSE;
}

/*
 *	getdph - return the address of a disk parameter header
 *	or error if the drive is out of the range 0-15
 *	To guarentee the drive is configured, it is selected first
 *	This is performed by bios call 9, but the Bios() function
 *	can not be used, because it returns in <A> not <HL>
 *	After the header is located, the default drive is reselected
 */
dph *getdph(drive)
int drive;
{
unsigned *warmstart, seldsk, result;
char current;

	current = bdos(GETCURRENT, 0);	/* save the current drive */
	warmstart = 1;
	seldsk = *warmstart + 24;	/* address of SELDSK routine */
	if ((result = call(seldsk, 0, 0, drive, 0)) == 0)
		return(ERROR);		/* invalid drive */
	bios(SELDSK,current);		/* restore the original disk */
	return(result);
}

/*
 *	extract - build a name string from an dir/fcb entry
 *	and place it in name.  Return the user/drive number
 */
char extract(entry, name)
dir *entry;
char *name;
{
int i,n;

	n = 0;
	for (i = 0; (i < 8) && ((name[n] = entry -> dir_name[i]) != ' '); i++)
		n++;
	name[n++] = '.';
	for (i = 0; (i < 3) && ((name[n] = entry -> dir_type[i]) != ' '); i++)
		n++;
	name[n] = '\0';
	return(entry -> dir_user);
}

/*
 * mputs - output the given string on the modem port
 */
mputs(string)
char *string;
{
	while (*string)
		mputc(*string++);
}

/*
 * mputc - output the given character on the modem port
 */
mputc(byte)
char byte;
{
	while (!moready())
		;
	MOD_TDATA(byte);
}

/*
 * moready - is the modem ready to have a character go to it?
 */
int moready()
{
	return MOD_TBE;
}

/*
 * mgetc - input a character from the modem port
 */
char mgetc()
{
	while (!miready())
		;
	return MOD_RDATA;
}

/*
 * miready - is the modem ready to give me a character?
 */
int miready()
{
	return MOD_RDA;
}

/*
 * hayesput - output a command string to the hayes, and check the result
 */
char hayesput(string)
char *string;
{
	mputs(string);
	return hayesget();
}

/*
 * hayesget - get a result byte from the hayes smartmodem
 */
char hayesget()
{
	while (!miready())
		;
	return mgetc();
}

/*
 *	PARSE - last modified [18 FEB 82]
 *	General argument parsing routines for ARGC, and ARGV
 *	Globarg and Posarg are available to the user. All other
 *	routines are for internal use.  There are no externals.
 */

/*
 *	GLOBARG - parse and assign global flags to variables
 *	return FALSE if there were errors in any of the arguments.
 *	Fmt is a format string similar to the one used by printf
 *	except a mandatory prefix and optional suffix can be specified.
 *	The types supported are boolean, char, string, and number.
 *	A default is required for each flag, and follows it in
 *	the format string delimited by a colon.  The following is
 *	an example for two flag, one boolean, and one numeric
 *
 *	..."s|ymbols%b:false c|opies%n:1",&symbol,&copies...
 */
int globarg(argc,argv,formats,args)
int argc;
char *argv[],*formats[],args;
{
int i,flag,**results;

	results = &args;
	argv[0] = 1;	/* argv[0] must be MANUALLY parsed beforehand */
	if (_verify(formats,results) == ERROR)
		return(FALSE);
	while ((i = argv[0]) < argc) {
		if ((flag = _parse(argc,argv,formats,results,TRUE)) == ERROR)
			return(FALSE);
		else if (!flag)
			_incr(argc,argv);
	}
	argv[0] = 0;	/* signal first time to POSARG */
	return(TRUE);
}

/*
 *	POSARG - parse and assign positional flags to variables
 *	return a pointer to the next positional paramters, EOF,
 *	or FALSE if there is an error parsing one of the arguments.
 *	The formats and arguments are identical to those for GLOBARG
 */
int posarg(argc,argv,formats,args)
int argc;
char *argv[],*formats[],args;
{
int i,flag,**results;

	results = &args;
	if ((!argv[0]) && (_verify(formats,results) == ERROR))
		return(FALSE);
	_incr(argc,argv);	/* dump the last positional parameter */
	while ((i = argv[0]) < argc) {
		if ((flag = _parse(argc,argv,formats,results,FALSE)) == ERROR)
			return(FALSE);
		else if (!flag)
			return(argv[i]);
	}
	return(EOF);
}

/*
 *	_PARSE - an internal routine to parse the flags
 *	argv[0] contains the current argument index to use.
 *	Return TRUE or FALSE depending on the _match, and
 *	ERROR if a required argument is missing or invalid.
 *	Oneonly is a flag indicating arguments must be unique
 */
int _parse(argc,argv,formats,results,oneonly)
int argc,*results[],oneonly;
char *argv[],*formats[];
{
int i,pos;
char *str;

	if (!formats)	/* no format array specified */
		return(FALSE);
	pos = argv[0];
	if (*argv[pos] == '-')
		for (i=0; str=formats[i]; i++) {
			if (_match(str,argv[pos])) {
				argv[pos++] = NULL;	/* remove it from processing */
				_incr(argc,argv);
				switch (str[index(str,'%')+1]) {
				case 'B':	/* convert to boolean */
					*results[i] = !(*results[i]);
					break;
				case 'C':	/* convert to character */
					poke(results[i],*argv[pos]);
					argv[pos++] = NULL;
					_incr(argc,argv);
					break;
				case 'N':	/* convert to numeric */
					*results[i] = atoi(argv[pos]);
					argv[pos++] = NULL;
					_incr(argc,argv);
					break;
				case 'S':	/* convert to string */
					strcpy(results[i],argv[pos]);
					argv[pos++] = NULL;
					_incr(argc,argv);
					break;
				default:
					puts("This cannot happen #1\n");
					return(ERROR);
				}
			if (oneonly)	/* check for duplicated arguments */
				while (pos < argc) {
					if (_match(str,argv[pos])) {
						printf("Unique argument '%s' duplicated\n",argv[pos]);
						return(ERROR);
					}
					pos++;
				}
			return(TRUE);
			}
		}
	return(FALSE);
}

/*
 *	_VERIFY - verify the format strings and assign defaults
 *	return OK, or ERROR depending whether errors were detected
 */
int _verify(formats,results)
char *formats[];
int *results[];
{
int i,valid,percent,bar,colon;
char *str;

	if (!formats)	/* no format array specified */
		return(OK);
	valid = OK;
	for (i=0; str=formats[i]; i++) {
		strupper(str);	/* fold to caps for the command line */
	 	percent = index(str,'%');
		bar = index(str,'|');
		colon = index(str,':');
		if ((bar == 0) || (percent <= 0) || (colon <= 0)
		|| (colon < percent) || (index("BCNS",str[percent+1]) == ERROR)) {
			printf("Argument parser format error on '%s'\n",str);
			valid = ERROR;
		}
		switch(str[percent+1]) {
		case 'B':	/* Boolean flag */
			if (str[colon+1] == 'T')
				*results[i] = TRUE;
			else	*results[i] = FALSE;
			break;
		case 'C':	/* Character flag */
			poke(results[i],str[colon+1]);
			break;
		case 'S':	/* String flag */
			strcpy(results[i],&str[colon+1]);
			break;
		case 'N':	/* Numeric flag */
			*results[i] = atoi(&str[colon+1]);
			break;
		default:
			puts("This cannot happen #2\n");
			valid = ERROR;
		}
	}
	return(valid);
}

/*
 *	_MATCH - pattern _match the argument flag prefix/suffix
 *	and return TRUE or FALSE depending on the result.
 *	If more than the required prefix is specified, then the
 *	WHOLE string must _match both the prefix and suffix.
 */
int _match(pattern,string)
char *pattern,*string;
{
int i;

	string++;
	if ((i = index(pattern,'|')) == ERROR)
		i = index(pattern,'%');
	if (!strncmp(string,pattern,i))
		return(TRUE);
	return(FALSE);
}

/*
 *	_INCR - increment the argument index in argv[0]
 *	skipping any previously parsed entries (set to NULL)
 */
_incr(argc,argv)
int argc;
char *argv[];
{
int i;

	for (i=argv[0]+1; i<argc; i++) {
		if (argv[i]) {
			argv[0] = i;
			return;
		}
	}
	argv[0] = argc;
}
                                                                                                                  