/* [OVERSTRK.C of JUGPDS Vol.46] */
/*
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*       Modifird by Toshiya Oota   (JUG-CPM No.10)              *
*                   Sakae ko-po 205 				*
*		    5-19-6 Hosoda				*
*		    Katusikaku Tokyo 124			*
*								*
*		for MS-DOS Lattice C V3.1J & 80186/V20/V30	*
*								*
*	Compiler Option: -ccu -k0(1) -ms -n -v -w		*
*								*
*	Edited & tested by Y. Monma (JUG-CP/M Disk Editor)	*
*			&  T. Ota   (JUG-CP/M Sub Disk Editor)	*
*								*
*****************************************************************
*/

/* Library functions for Software Tools */

#include "stdio.h"
#include "dos.h"
#include "ctype.h"
#include "tools.h"
#include "toolfunc.h"

/* overstrike - convert backspaces into multiple lines */

int	cr_flag;

void main(argc, argv)
int	argc;
char 	**argv;
{
FILE	*fp;
void overstrik();

/*	if (wildexp(&argc,&argv) == ERROR)	/* Can't Compile LC-C */
		error("OVR901 Wildexp overflow");  */
	cr_flag = OFF;
	if (argc == 1)
		overstrik(STDIN);
	else
		while (--argc > 0)
			if ((fp=fopen(*++argv, "r")) == NULL) {
			      fprintf(STDERR, "OVR902 Can't open %s\n", *argv);
			      exit(1);
			      }
			else {
				overstrik(fp);
				fclose(fp);
				}
}


#define	SKIP	' '
#define	NOSKIP	'+'
#define	FF	'1'

void overstrik(fp)
FILE	*fp;
{
int	c, col, newcol;
void    putch();

	for (col = 1; ; col = (c == NEWLINE) ? 1 : col + 1) {
		newcol	= col;
		while ((c = getc(fp)) == BACKSPACE)
			newcol = max(newcol-1, 1);
		if (newcol < col) {
			putch(col,NEWLINE);
			putch(col,NOSKIP);
			for (col = 1; col <= newcol; col++)
				putch(col,BLANK);
			/******************************/
			/*******  !! Caution !!  ******/
			/* First BLANK Not Printed!   */
			/* Look at putch() line 23&24 */
			/******************************/
		}
		else if (col == 1 && c != EOF && c != CPMEOF)
			putch(col,SKIP);
		if (c == EOF || c == CPMEOF) {
			putch(col,CPMEOF);
			break;
		}
		putch(col,c);
	}
}


void putch(col,c)
int  col,c;
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
		if(col !=1 || c != SKIP)
			putchar(c);
}
