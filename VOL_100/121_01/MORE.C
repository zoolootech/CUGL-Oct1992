/*
HEADER: CUG 121.??;

    TITLE:	more - list file(s) to terminal;
    VERSION:	1.0;
    DATE:	08/01/85;
    DESCRIPTION: "This program lists one or more files, a page (or a
		user-selectable amount) at a time, on the console.  The
		names of the files to be listed are supplied as arguments
		on the command line; if no arguments are given, the user
		is prompted for filename(s).";
    KEYWORDS:	list, page;
    SYSTEM:	CP/M;
    FILENAME:	MORE.C;
    WARNINGS:	"Requires fseek.c for link.
		The LERASE #define must be customised for the user's
		terminal.";
    AUTHORS:	Mike W. Meyer;
    COMPILERS:	BDS-C 1.50;
*/
/*
	more - scan a file in strange increments...

	Description
	  This program takes zero or more arguments, each of which is the name
	of a file (if no arguments are passed, the user is prompted for the
	filenames - a null entry terminates the prompting).
	  For each file, the first page's worth of lines is listed and then
	then the user is prompted with "more? ".  At this point, the following
	replies are accepted:
		N	stop listing file, go on to next
	 Y or (newline)	list next page
		S	list next half page
		0	go back to start of file, and list first page
	      nnn	list next nnn lines
	     -nnn	back up nnn lines, then list a page
*/

#include <bdscio.h>
#include <hardware.h>

/* The following string is terminal-dependent & must be verified by the user */
#define LERASE	"\027\015\036"	/* A string to back up a line, and erase it */

char *gets();

main(argc, argv)
int argc;
char **argv;
{
char buf[MAXLINE];

	if (argc == 2)
		show(*++argv);
	else if (argc > 2)
		while (--argc) {
			printf("hit newline to see %s",*++argv);
			gets(buf);
			show(*argv);
		}
	else
		for (;;) {
			puts("which file? ");
			if (!*gets(buf))
				exit();
			show(buf);
		}
}
/*
 * show - open the file, display it, and close it
 */
show(name)
char *name;
{
char file[BUFSIZ];

	if (fopen(name, file) == -1) {
		printf("can't open %s\n", name);
		return;
	}
	process(file);
	fclose(file);
}
/*
 * process - go through a file page at a time, with or as
 *	told by user
 */
process(file)
char *file;
{
char buf[MAXLINE], c;

	page(file);
	for (;;) {
		puts("more? ");
		if ((c = tolower(*gets(buf))) == 'n')
			return;
		if (!c || c == 'y') { 
			if (page(file))
				return;
		}
		else if (c == 's') {
			puts(LERASE);
			if (lines(file, TLENGTH / 2))
				return;
		}
		else if (c == '0') {
			fseek(file, 0, 0);
			if (page(file))
				return;
		}
		else if (isdigit(c)) {
			puts(LERASE);
			if (lines(file, atoi(buf)))
				return;
		}
		else if (c == '-') {
			backup(file, atoi(buf));
			if (page(file))
				return;
		}
		else puts(LERASE);
	}
}
/*
 * page - output a screenful of data (returns TRUE iff EOF)
 */
page(file)
char *file;
{
	puts(CLEARS);
	return(lines(file, TLENGTH - 1));
}
/*
 * lines - output count lines to the screen (returns TRUE iff EOF)
 */
lines(file, count)
char *file;
{
int i, j, c;

	for (i = 0; i < count; i++) {
		for (j = 0; j < TWIDTH; j++) {
			if ((c = rdch(file)) == EOF)
				return(TRUE);
			else if (c == '\t') {
				j = j / 8 * 8 + 8;
				if (j < TWIDTH)
					j--;
				else {
					putchar('\n');
					break;
				}
			}
			putchar(c);
			if (c == '\n')
				break;
		}
	}
	return(FALSE);
}
/*
 * rdch - read a character, and take care of the CP/M kludges
 */
rdch(file)
char *file;
{
int c;

	if ((c = getc(file)) == CPMEOF)
		return(EOF);
	if (c == '\r')
		return(getc(file));
	return(c);
}
/*
 * backup - by count lines (or guess at it), and then get to
 *	a line boundary
 */
backup(file, count)
char *file;
{
char buf[MAXLINE];

	fseek(file, count / 3, 4);
	fgets(buf, file);
}
                    