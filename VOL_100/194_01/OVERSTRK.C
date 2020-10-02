/* [OVERSTRK.C of JUGPDS Vol.17]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* overstrike - convert backspaces into multiple lines */

#include "stdio.h"
#include <dio.h>

#define	ON	-1
#define	OFF	0

int	cr_flag;

main(argc, argv)
int	argc;
char 	**argv;

{
	FILE	inbuf, *fp;

	if (wildexp(&argc,&argv) == ERROR)
		exit(puts("Wildexp overflow"));
	dioinit(&argc, argv);
	cr_flag = OFF;
	if (argc == 1)
		overstrik(STDIN);
	else
		while (--argc > 0)
			if (fopen(*++argv, fp) == 0) {
				fprintf(STDERR, "Can't open %s\n", *argv);
				exit(1);
				}
			else {
				overstrik(fp);
				fclose(fp);
				}
	dioflush();
}


#define	SKIP	' '
#define	NOSKIP	'+'
#define	FF	'1'

overstrik(fp)
FILE	*fp;
{
	int	c, col, newcol;

	for (col = 1; ; col = (c == NEWLINE) ? 1 : col + 1) {
		newcol	= col;
		while ((c = getc(fp)) == BACKSPACE)
			newcol = max(newcol-1, 1);
		if (newcol < col) {
			putch(NEWLINE);
			putch(NOSKIP);
			for (col = 1; col < newcol; col++)
				putch(BLANK);
		}
		else if (col == 1 && c != EOF && c != CPMEOF)
			putch(SKIP);
		if (c == EOF || c == CPMEOF)
			break;
		putch(c);
	}
}


putch(c)
{
	if (cr_flag == ON) {
		cr_flag = OFF;
		if (c == NEWLINE) {
			putchar(NEWLINE);
			cr_flag = ON;
		}
		else if (c == NOSKIP)
			putchar(CRETURN);
		else if (c == SKIP)
			putchar(NEWLINE);
		else if (c == FF)
			putchar(FORMFEED);
		else
			putchar(c);
	} else if (c == NEWLINE) {
		putchar(CRETURN);
		cr_flag = ON;
	} else
		putchar(c);
}
