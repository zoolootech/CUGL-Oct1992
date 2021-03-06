/*-
 * vis - visual display of files
 *
 * Usage: vis [-et] [file...]
 *
 * Flags:
 * -e	show the ends of lines as '$'
 * -t	show tabs as '^I'
 *
 * Contents of argument files are written to standard output.
 * If there are no arguments, or if a file named "-" is encountered,
 * standard input is written to standard output.
 * Exit status is number of files that couldn't be opened or had read errors.
 *
 * This program is in the public domain.
 * David MacKenzie
 * 6522 Elgin Lane
 * Bethesda, MD 20817
 *
 * Latest revision: 05/08/88
**
**	Roberto Artigas Jr
**	P.O. Box 281415
**	Memphis, TN 38168-1415
**	work: 901-762-6092
**	home: 901-373-4738
**
**	1988.12.14 - Get to run under OS/2.
**		Used C/2 version 1.10 under OS/2 E 1.1
**		cl -c -AL vis.c
**		link vis,/noi,vis,llibce+os2, ;
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

/*
 * Function prototypes 
 */
int vis();

main(argc, argv)
int argc;
char **argv;
{
    int showtabs = 0;		       /* show tabs as ^I? */
    int showend = 0;		       /* show eol as $? */
    int optind;			       /* loop index */
    int errs = 0;		       /* # of files with read errors */

    for (optind = 1; optind < argc && *argv[optind] == '-'; ++optind)
	while (*++argv[optind])
	    switch (*argv[optind])
	    {
	    case 'e':
		showend = 1;
		break;
	    case 't':
		showtabs = 1;
		break;
	    default:
		fprintf(stderr, "Usage: vis [-et] [file...]\n");;
		exit(1);
		break;
	    }

    if (optind == argc)
	errs += vis("-", showtabs, showend);
    else
	for (; optind < argc; ++optind)
	    errs += vis(argv[optind], showtabs, showend);

    return (errs);
    exit(errs);
}
/*
 * Visually display file on standard output with newline and ^Z conversions.
 * If file is "-", use standard input. Return 0 if ok, 1 if error. 
 */

vis(file, showtabs, showend)
char *file;
int showtabs,
 showend;
{
    FILE *fp = stdin;		       /* input file pointer */
    int c;			       /* one byte of input */

#if	DOS || OS2
    if (strcmp(file, "-") && (fp = fopen(file, "rb")) == NULL)
    {
#else
    if (strcmp(file, "-") && (fp = fopen(file, "r")) == NULL)
    {
#endif
	perror(file);
	return 1;
    }
    while ((c = agetc(fp)) != EOF)
    {
	if (c > 127)
	{
	    /* Meta-characters. */
	    putchar('M');
	    putchar('-');
	    c &= 127;
	}
	if (c == '\n' && showend)
	{
	    putchar('$');
	    putchar('\n');
	} else
	if (c < ' ' && (c != '\t' || showtabs) && c != '\n')
	{
	    putchar('^');
	    putchar(c + 'A' - 1);
	} else
	if (c == 127)
	{
	    putchar('^');
	    putchar('?');
	} else
	    putchar(c);
    }
    if (fp != stdin && fclose(fp) == EOF)
    {
	fprintf(stderr, "%s: Read error\n", file);
	return 1;
    }
    return 0;
}
