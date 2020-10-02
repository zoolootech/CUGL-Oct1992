/*-
 * cat - concatenate files
 * Usage: cat [-] [file...]
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
**	home: 901-762-6092
**	work: 901-373-4738
**
**	1988.12.09 - Get to run under OS/2.
**		Used C/2 version 1.10 under OS/2 E 1.1
**		cl -c -AL cat.c
**		link cat,/noi,cat,llibce+os2, ;
**+
*/
#define	CMP	0
#define	DOS	0
#define	OS2	1
#include	<stdio.h>

#if	DOS || OS2
#include	<stdlib.h>
#include	<string.h>
#include	<io.h>
#define	agetc	getc
#define	aputc	putc
#endif

/*
 * Function prototypes 
 */
int cat();

int 
main(argc, argv)
int argc;
char **argv;
{
    int errs = 0;		       /* # of files with read errors */
    int optind;			       /* loop index */

    if (argc == 1)
	errs += cat("-");
    else
	for (optind = 1; optind < argc; ++optind)
	    errs += cat(argv[optind]);

    return (0);
    exit(errs);
}
/*
 * Send the contents of file to standard output with no translation. If file
 * is "-", use standard input, and if input is from con:, do newline and ^Z
 * translation. Return 0 if ok, 1 if error. 
 */

int 
cat(file)
char *file;
{
    FILE *fp;			       /* input file pointer */
    int c;			       /* one byte of input */

    if (!strcmp(file, "-"))
    {
	/* Agetc and aputc translate cr-lf to lf and ^Z to EOF. */
	if (isatty(0))
	    while ((c = agetc(stdin)) != EOF)
		aputc(c, stdout);
	else
	    while ((c = getc(stdin)) != EOF)
		putc(c, stdout);
    } else
    {
#if	DOS || OS2
	if ((fp = fopen(file, "rb")) == NULL)
	{
#else
	if ((fp = fopen(file, "r")) == NULL)
	{
#endif
	    perror(file);
	    return 1;
	}
	while ((c = getc(fp)) != EOF)
	    putc(c, stdout);
	if (fclose(fp) == EOF)
	{
	    fprintf(stderr, "cat: %s: Read error\n", file);
	    return 1;
	}
    }
    return 0;
}
