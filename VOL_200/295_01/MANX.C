/*	Copyright (c) 1989 Citadel	*/
/*	   All Rights Reserved    	*/

/* #ident	"manx.c	1.2 - 89/10/31" */

#if __STDC__ == 1
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CONST const
#else
#include <errno.h>
#include <limits.h>
#include <stdio.h>
void exit();			/* stdlib.h */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS	(0)
#define EXIT_FAILURE	(1)
#endif
long strtol();
char *strstr();			/* string.h */
int strncmp();
int strcmp();
int strlen();
#define CONST
#endif

#define PAGELEN		(60)		/* default page length */
#define BACKSLASH	('\\')		/* char to begin escape sequence */
#define PAGINATOR	("\f")		/* string to use as page separator */
#define MAXLINE		(256)		/* maximum line length */

typedef struct {		/* language definition structure */
	char option[11];	/* language command line option */
	char begin[21];		/* marker for beginning of manual entry */
	char end[21];		/* marker for end of manual entry */
	char comment[11];	/* character sequence that begins comment */
	int  flags;		/* flags */
} lang_t;

/* lang_t bit flags */
#define LINECMT		(1)
#define BLOCKCMT	(2)

/* language definitions */
lang_t langs[] = {
	{"a",  "--man", "--end", "--", LINECMT},	/* Ada */
	{"c",  "/*man", "*/",    "/*", BLOCKCMT},	/* C */
	{"f",  "Cman", "Cend",   "C",  LINECMT},	/* Fortran */
	{"l",  ";man", ";end",   ";",  LINECMT},	/* Lisp */
	{"m",  "(*man", "*)",    "(*", BLOCKCMT},	/* Modula-2 */
	{"p",  "{man",  "}",     "{",  BLOCKCMT},	/* Pascal */
	{"pl", "\"man", "\"",    "\"", BLOCKCMT},	/* Prolog */
	{"st", "\"man", "\"",    "\"", BLOCKCMT}	/* Smalltalk */
};

/* index into langs array for each language */
#define ADA		(1)
#define C		(2)
#define FORTRAN		(3)
#define LISP		(4)
#define MODULA_2	(5)
#define PASCAL		(6)
#define PROLOG		(7)
#define SMALLTALK	(8)

#define LANG		C		/* default language */

/* command syntax */
#define USAGE		("Usage: manx [language] [page-length]")

#if __STDC__ == 1
static int manputs(const char *cs, FILE *fp);
#else
static int manputs();
#endif

/*man---------------------------------------------------------------------------
NAME
     manx - manual entry extracter

SYNOPSIS
     manx [language] [page-length]

DESCRIPTION
     The manx command extracts manual entries from source files.  The
     source files are read from stdin and the manual entries are
     written to stdout.  Each individual manual entry is separated
     into pages by form feeds and terminated with a form feed.

     The language option specifies which language the source file is
     written in.  The languages supported are:

                           Ada           -a
                           C             -c
                           Fortran       -f
                           Lisp          -l
                           Modula-2      -m
                           Pascal        -p
                           Prolog        -pl
                           Smalltalk     -st

     The default language is C.

     The page-length argument can be used to set the page length.
     Pagination may be disabled by specifying a page length of 0.
     The default page length is 60.

     The beginning of a manual entry is marked by the character
     sequence (language dependent) to start a comment immediately
     followed by the characters 'man'.  This marker must appear in the
     leftmost column allowed by the language.  For block comments, the
     end of the manual entry is marked by the end of the comment.  For
     line comments, the end of a manual entry is marked by the
     character sequence to end a comment immediately followed by the
     characters 'end'.  All lines between but not including these
     constitute the manual entry.

     The following escape sequences can be used within a manual entry:

                  audible alert       BEL       \\a
                  backspace           BS        \\b
                  carriage return     CR        \\r
                  horizontal tab      HT        \\t
                  backslash           \\         \\\\

EXAMPLE
     On a UNIX system, the following command would extract the manual
     pages from all C files in the current UNIX directory, paginate
     them every 52 lines, and place them in a file called manual.

          $ cat *.c | manx -c 52 > manual

     Catenating files is much for difficult in MS-DOS.  For this
     system, the following sequence of commands is required.

          > copy file.c/a+file2.c+file3.c+file4.c tmp
          > type tmp | manx -c 52 > manual
          > del tmp

     manx works particularly well when used in conjunction with a make
     utility.  Below are the relevant parts of the makefile for a C
     library for which manx is used to extract the reference manual.

          LIB     = blkio
          MAN     = $(LIB).man

          MANFILES=blkio.h                               \\
                  bclose.c bexit.c  bflpop.c  bflpush.c  \\
                  bflush.c bgetb.c  bgetbf.c  bgeth.c    \\
                  bgethf.c bopen.c  bputb.c   bputbf.c   \\
                  bputh.c  bputhf.c bsetbuf.c bsetvbuf.c \\
                  bsync.c  lockb.c

          man:    $(MANFILES)
                  cat $(MANFILES) | manx > $(MAN)

     The reference manual for this library is generated simply by
     entering

          make man

------------------------------------------------------------------------------*/
#if __STDC__ == 1
int main(int argc, char *argv[])
#else
int main(argc, argv)
int argc;
char *argv[];
#endif
{
	int	lang		= LANG;		/* default language */
	long	pagelen		= PAGELEN;	/* default page length */
	int	page		= 0;		/* page number */
	long	line		= 0;		/* line number */
	int	i		= 0;		/* loop index */
	char	s[MAXLINE + 1];			/* input line */
	char *	endptr		= NULL;		/* pointer for strtol fct */

	/* process command line options and arguments */
	/* language option */
	if ((argc > 1) && (*argv[1] == '-')) {
		argc--;
		argv++;
		lang = 0;
		for (i = 0; i < sizeof(langs)/sizeof(*langs); i++) {
			if (strcmp(*argv + 1, langs[i].option) == 0) {
				lang = i + 1;
				break;
			}
		}
		if (lang == 0) {
			printf("manx: invalid language option\n");
			printf("%s\n", USAGE);
			exit(EXIT_FAILURE);
		}
	}
	/* page length argument */
	if (argc > 1) {
		argc--;
		argv++;
		errno = 0;
		pagelen = strtol(*argv, &endptr, 10);
		if (errno == ERANGE) {
			printf("manx: page length argument out of range\n");
			printf("%s\n", USAGE);
			exit(EXIT_FAILURE);
		}
		if (endptr != NULL) {
			if (endptr[0] != '\0') {
				printf("%s\n", USAGE);
				exit(EXIT_FAILURE);
			}
		}
		if (pagelen < 0) {
			printf("%s\n", USAGE);
			exit(EXIT_FAILURE);
		}
	}
	if (argc != 1) {
		printf("%s\n", USAGE);
		exit(EXIT_FAILURE);
	}

	/* main loop */
	while (1) {
		/* read next line of input */
		if (fgets(s, (int)sizeof(s), stdin) == NULL) {
			if (ferror(stdin) != 0) {
				fprintf(stderr, "*** Error reading standard input.  Exiting.\n");
				exit(EXIT_FAILURE);
			}
			break;
		}
		/* check for manual entry marker at beginning of line */
		if (strncmp(s, langs[lang - 1].begin, strlen(langs[lang - 1].begin)) != 0) {
			continue;
		}
		if (langs[lang - 1].flags & BLOCKCMT) {
			if (strstr(s, langs[lang - 1].end) != NULL) {
				continue;
			}
		}
		/* inner loop */
		line = 0;
		page = 1;
		while (1) {
			line++;
			/* read next line of manual entry */
			if (fgets(s, (int)sizeof(s), stdin) == NULL) {
				if (ferror(stdin) != 0) {
					fprintf(stderr, "*** Error reading standard input.  Exiting.\n");
					exit(EXIT_FAILURE);
				}
				break;
			}
			/* check for end of entry marker */
			if (langs[lang - 1].flags & LINECMT) {
				if (strncmp(s, langs[lang - 1].end, strlen(langs[lang - 1].end)) == 0) {
					break;
				}
				if (strncmp(s, langs[lang - 1].comment, strlen(langs[lang - 1].comment)) != 0) {
					break;
				}
			} else {
				if (strstr(s, langs[lang - 1].end) != NULL) {
					break;
				}
			}
			if (s[strlen(s) - 1] != '\n') {
				fprintf(stderr, "*** Warning.  Maximum line length of %d exceeded.  Page %ld, line %ld.\n", (int)(sizeof(s) - 2), page, line);
				s[strlen(s) - 1] = '\n';
			}
			if (langs[lang - 1].flags & LINECMT) {
				if (manputs(s + strlen(langs[lang - 1].comment) , stdout) == -1) {
					fprintf(stderr, "*** Error writing line %ld, of page %ld.  Exiting\n", line, page);
					exit(EXIT_FAILURE);
				}
			} else {
				if (manputs(s, stdout) == -1) {
					fprintf(stderr, "*** Error writing line %ld, of page %ld.  Exiting\n", line, page);
					exit(EXIT_FAILURE);
				}
			}
			if ((line >= pagelen) && (pagelen != 0)) {
				if (fputs(PAGINATOR, stdout) == EOF) {
					fprintf(stderr, "*** Error writing paginator to standard output.  Exiting.\n");
					exit(EXIT_FAILURE);
				}
				line = 0;
				page++;
			}
		}
		/* write last paginator */
		if ((line != 1) && (pagelen != 0)) {
			if (fputs(PAGINATOR, stdout) == EOF) {
				fprintf(stderr, "*** Error writing paginator to standard output.  Exiting.\n");
				exit(EXIT_FAILURE);
			}
			page++;
		}
		/* check if end of file */
		if (feof(stdin) != 0) {
			break;
		}
	}

	exit(EXIT_SUCCESS);
}

/*------------------------------------------------------------------------------
DESCRIPTION
     The manputs function writes the null-terminated string pointed to
     by cs to the named output stream.  Any manx escape sequence found
     in the string is converted to the character it represents before
     being output.

DIAGNOSTICS
     Upon successful completion, a value of 0 is returned.  Otherwise,
     a value of -1 is returned.

------------------------------------------------------------------------------*/
#if __STDC__ == 1
static int manputs(const char *cs, FILE *fp)
#else
static int manputs(cs, fp)
char *cs;
FILE *fp;
#endif
{
	char t[MAXLINE + 1];	/* target string */
	int i = 0;		/* index into target string */
	char c = '\0';

	/* convert string to output format */
	for (i = 0; *cs != '\0'; i++) {
		if (i > sizeof(t)) {
			return -1;
		}
		c = *cs++;
		/* check for escape sequence */
		if ((c == BACKSLASH) && (*cs != '\0')) {
			c = *cs++;
			switch (c) {
			case 'a':	/* audible alert */
				c = '\a';
				break;	/* case 'a': */
			case 'b':	/* backspace */
				c = '\b';
				break;	/* case 'b': */
			case 'r':	/* carriage return */
				c = '\r';
				break;	/* case 'r': */
			case 't':	/* horizontal tab */
				c = '\t';
				break;	/* case 't': */
			case '\\':	/* backslash */
				c = '\\';
				break;	/* case '\\': */
			default:	/* ignore backslash */
				break;	/* default: */
			}
		}
		t[i] = c;
	}
	t[i] = '\0';

	/* write converted string to fp */
	if (fputs(t, fp) == EOF) {
		return -1;
	}

	return 0;
}

#if __STDC__ != 1
char *strstr(cs, ct)
char *cs;
char *ct;
{
	int ctlen = 0;

	if ((cs == NULL) || (ct == NULL)) {
		return NULL;
	}

	ctlen = strlen(ct);
	while (*cs != '\0') {
		if (strncmp(cs, ct, ctlen) == 0) {
			return cs;
		}
		cs++;
	}

	return NULL;
}
#endif
