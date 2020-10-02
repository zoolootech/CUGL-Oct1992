/*
 * strings - find printable strings in binary files
 *
 * A string is any sequence of 4 or more printable characters ending
 * with a newline, dollar sign, or null.  Exit status is the number of
 * files that couldn't be opened.
 *
 * Usage: strings [-o$0] [-number] file...
 *
 * Flags:
 * -o       precede each string by its octal offset in the file.
 * -$       don't allow '$' as a string terminator.
 * -0       any string of 4 (number) or more printable characters is a string.
 * -number  use number as minimum string length instead of 4.
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

#define LEN_INITIAL 300		/* Initial space allocated for string. */
#define LEN_INCREMENT 100	/* Amount to increase buffer by on overflow. */

/* Command line arguments. */
int     minlen = 4;		/* Minimum length to be considered a string. */
int     show_offset = 0;	/* Display octal offset of string? */
int     dollar_term = 1;	/* '$' can terminate string? */
int     any_term = 0;		/* Any nonprint char can terminate string? */

char   *savebuf;		/* Save consecutive ASCII chars. */
int     buflen = LEN_INITIAL;	/* Current max. length of savebuf. */

_main(argc, argv)
    int     argc;
    char  **argv;
{
    void    usage();
    char   *malloc();
    int     optind;		/* Loop index. */
    int     errs = 0;		/* Exit status. */

    if (!(savebuf = malloc((unsigned) buflen))) {
	perror("malloc");
	exit(1);
    }

    for (optind = 1; optind < argc && *argv[optind] == '-'; ++optind)
	while (*++argv[optind])
	    switch (*argv[optind]) {
	    case 'o':
		show_offset = 1;
		break;
	    case '$':
		dollar_term = 0;
		break;
	    case '0':
		any_term = 1;
		break;
	    default:
		if ((minlen = atoi(argv[optind])) <= 0)
		    usage();
		/* Break out of while loop. */
		argv[optind] += strlen(argv[optind]) - 1;
		break;
	    }

    if (optind == argc)
	usage();

    for (; optind < argc; ++optind)
	errs += strings(argv[optind]);

    exit(errs);
}

int     consecutives;		/* # consecutive printable characters. */

/*
 * Display the strings in file.  Return 0 if ok, 1 if error.
 */

strings(file)
    char   *file;
{
    long    ftell();
    FILE   *fp;			/* Input file pointer. */
    long    offset = 0L;	/* Location in file for -o flag. */
    int     c;			/* One character of input. */

    if (!(fp = fopen(file, "r"))) {
	perror(file);
	return 1;
    }
    consecutives = 0;
    do {
	c = getc(fp);
	if (c == 0 || c == '\n' || dollar_term && c == '$')
	    flushbuf(offset);
	else if (isascii(c) && isprint(c)) {
	    if (consecutives == 0)
		offset = ftell(fp) - 1L;	/* Loc. of start of string. */
	    buffer(c);
	} else if (any_term)
	    flushbuf(offset);
	else
	    consecutives = 0;
    } while (c != EOF);

    (void) fclose(fp);
    return 0;
}

buffer(c)
    int     c;
{
    char   *realloc();

    savebuf[consecutives] = c;
    if (++consecutives == buflen) {
	buflen += LEN_INCREMENT;
	if (!(savebuf = realloc(savebuf, (unsigned) buflen))) {
	    perror("realloc");
	    exit(1);
	}
    }
}

flushbuf(offset)
    long    offset;
{
    if (consecutives >= minlen) {
	if (show_offset)
	    printf("%7lo ", offset);
	printf("%.*s\n", consecutives, savebuf);
    }
    consecutives = 0;
}

void
usage()
{
    fprintf(stderr, "Usage: strings [-o$0] [-number] file...\n");
    exit(1);
}
