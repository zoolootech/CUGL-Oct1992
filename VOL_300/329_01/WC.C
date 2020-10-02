/*-
 * wc - word count
 *
 * Usage: wc [-lwc] [file...]
 *
 * Count lines, words, and characters in the named files or stdin.
 * Words are delimited by spaces, tabs, or newlines.  Default action is
 * -lwc. 
 *
 * Flags:
 * -l   display number of lines
 * -w   display number of words
 * -c   display number of characters
 *
 * This program is in the public domain.
 * David MacKenzie
 * 6522 Elgin Lane
 * Bethesda, MD 20817
 *
 * Latest revision: 04/23/88
**
**	Roberto Artigas Jr
**	P.O. Box 281415
**	Memphis, TN 38168-1415
**	work: 901-762-6092
**	home: 901-373-4738
**
**	1988.12.14 - Get to run under OS/2.
**		Used C/2 version 1.10 under OS/2 E 1.1
**		cl -c -AL wc.c
**		link wc,/noi,wc,llibce+os2, ;
**+
*/
#define	CPM	0
#define	DOS	0
#define	OS2	1

#include	<stdio.h>

#if	CPM
extern int errno;
#endif

#if	DOS || OS2
#define	agetc	getc
#include	<ctype.h>
#include	<stdlib.h>
#include	<string.h>
#endif


/* Totals in case multiple files are specified. */
long totnlines = 0L;
long totnwords = 0L;
long totnchars = 0L;

int nfiles = 0;			       /* Number of files specified. */

main(argc, argv)
int argc;
char **argv;
{
    void wc();
    int lines = 0;		       /* Count lines? */
    int words = 0;		       /* Count words? */
    int chars = 0;		       /* Count chars? */
    int optind;			       /* Loop index. */

    for (optind = 1; optind < argc && *argv[optind] == '-'; ++optind)
	while (*++argv[optind])
	    switch (*argv[optind])
	    {
	    case 'l':
		lines = 1;
		break;
	    case 'w':
		words = 1;
		break;
	    case 'c':
		chars = 1;
		break;
	    default:
		fprintf(stderr, "Usage: wc [-lwc] [file...]\n");
		exit(1);
	    }

    if (lines + words + chars == 0)
	lines = words = chars = 1;

    if (optind == argc)
    {
	wc("-", lines, words, chars);
	exit(0);
    }
    for (; optind < argc; ++optind)
	wc(argv[optind], lines, words, chars);

    if (nfiles > 1)
    {
	if (lines)
	    printf("%8ld", totnlines);
	if (words)
	    printf("%8ld", totnwords);
	if (chars)
	    printf("%8ld", totnchars);
	printf(" total\n");
    }
    return (0);
    exit(0);
}
/*
 * Calculate and output the specified counts. 
 */

void
wc(file, lines, words, chars)
char *file;
char lines,
 words,
 chars;
{
    char *normalize();
    FILE *fp;			       /* Input file pointer. */
    int c;			       /* One byte of input. */
    long nlines = 0L;		       /* Number of lines in current file. */
    long nwords = 0L;		       /* Ditto for words. */
    long nchars = 0L;		       /* Ditto for characters. */
    char inword = 0;		       /* Were we in a word before this
				        * char? */

    ++nfiles;
    if (!strcmp(file, "-"))
	fp = stdin;
#if	DOS || OS2
    else
    if (!(fp = fopen(file, "rb")))
    {
#else
    else
    if (!(fp = fopen(file, "r")))
    {
#endif
	perror(file);
	return;
    }
    while ((c = agetc(fp)) != EOF)
    {
	++nchars;
	switch (c)
	{
	case '\n':
	    ++nlines;
	    /* Fall through. */
	case '\t':
	case ' ':
	    if (inword)
		++nwords;
	    inword = 0;
	    break;
	default:
	    inword = 1;
	    break;
	}
    }

    if (inword)
    {
	++nlines;
	++nwords;
    }
    totnlines += nlines;
    totnwords += nwords;
    totnchars += nchars;

    if (lines)
	printf("%8ld", nlines);
    if (words)
	printf("%8ld", nwords);
    if (chars)
	printf("%8ld", nchars);
    if (fp != stdin)
	printf(" %s", normalize(file));
    printf("\n");

    if (fp != stdin)
	(void) fclose(fp);
}
/*
 * Convert uppercase letters to lowercase, and non-graphic characters to
 * '?'. 
 */

char *
normalize(s)
char *s;
{
    char *t;

    for (t = s; *t; ++t)
	if (!isascii(*t) || !isgraph(*t))
	    *t = '?';
	else
	if (isupper(*t) && *t != '_')
	    /* Aztec C's ctype thinks that isupper('_') is true . . . */
	    *t = tolower(*t);
    return s;
}
