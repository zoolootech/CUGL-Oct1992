/*
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
 */

#include <stdio.h>
#include <ctype.h>

/* Totals in case multiple files are specified. */
long    totnlines = 0L;
long    totnwords = 0L;
long    totnchars = 0L;

int     nfiles = 0;		/* Number of files specified. */

_main(argc, argv)
    int     argc;
    char  **argv;
{
    void    wc();
    int     lines = 0;		/* Count lines? */
    int     words = 0;		/* Count words? */
    int     chars = 0;		/* Count chars? */
    int     optind;		/* Loop index. */

    for (optind = 1; optind < argc && *argv[optind] == '-'; ++optind)
	while (*++argv[optind])
	    switch (*argv[optind]) {
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

    if (optind == argc) {
	wc("-", lines, words, chars);
	exit(0);
    }

    for (; optind < argc; ++optind)
	wc(argv[optind], lines, words, chars);

    if (nfiles > 1) {
	if (lines)
	    printf("%8ld", totnlines);
	if (words)
	    printf("%8ld", totnwords);
	if (chars)
	    printf("%8ld", totnchars);
	printf(" total\n");
    }
    exit(0);
}

/*
 * Calculate and output the specified counts.
 */

void
wc(file, lines, words, chars)
    char   *file;
    char    lines, words, chars;
{
    char   *normalize();
    FILE   *fp;			/* Input file pointer. */
    int     c;			/* One byte of input. */
    long    nlines = 0L;	/* Number of lines in current file. */
    long    nwords = 0L;	/* Ditto for words. */
    long    nchars = 0L;	/* Ditto for characters. */
    char    inword = 0;		/* Were we in a word before this char? */

    ++nfiles;
    if (!strcmp(file, "-"))
	fp = stdin;
    else if (!(fp = fopen(file, "r"))) {
	perror(file);
	return;
    }

    while ((c = agetc(fp)) != EOF) {
	++nchars;
	switch (c) {
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

    if (inword) {
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
 * Convert uppercase letters to lowercase, and non-graphic characters to '?'.
 */

char   *
normalize(s)
    char   *s;
{
    char   *t;

    for (t = s; *t; ++t)
	if (!isascii(*t) || !isgraph(*t))
	    *t = '?';
	else if (isupper(*t) && *t != '_')
	    /* Aztec C's ctype thinks that isupper('_') is true . . . */
	    *t = tolower(*t);
    return s;
}
