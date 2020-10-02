/* [concat1.c of JUGPDS Vol.46] */
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

#include "fcntl.h"

char tbuf[TBUFSIZ];

void main(argc, argv)
int  argc;
char **argv;
{
int fp;
char *p;
int n;

	if (argc < 2) {
		error("CON901 Usage: concat1 file1 file2 ... >outfile");
		exit(1);
		}
	p = tbuf;
	while (--argc > 0) {
		if ( (fp = open( *++argv, O_RDONLY | O_RAW ,0)) == ERROR) {
			printf("\nCON902 can't open %s\n", *argv);
			exit(1);
			}
		printf("\nfile:%s\n", *argv);
		while ((n = read(fp, p, 256 )) > 0) {
			while (*p != CPMEOF && n--)
				putchar(*p++);
			p = tbuf;
		}
		close(fp);
	}
}
