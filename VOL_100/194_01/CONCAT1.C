/* [CONCAT1.C of JUGPDS Vol.17]
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

#include "stdio.h"
#include <dio.h>

#define cout(s,limit)	while (*s != CPMEOF && limit--) putchar(*s++)

char tbuf[(1024*38)];

main(argc, argv)
char **argv;
{
	int fp;
	char *p;
	int n;


	if (wildexp(&argc, &argv) == ERROR)
		error("WILDEXP  error!");
	dioinit(&argc, argv);
	if (argc < 2) {
		error("Usage: cat1 file1 file2 ... >outfile");
		exit();
		}
	p = tbuf;
	while (--argc > 0) {
		if ( (fp = open( *++argv, 0)) == ERROR) {
			fprintf(STDERR, "cat: can't open %s\n", *argv);
			exit();
			}
		else
			fprintf(STDERR, "file:%s\n", *argv);
			while ((n=read(fp, p, 304)*128 ) > 0) {
				cout(p, n);
				p = tbuf;
			}
		close(fp);
		}
	dioflush();
}
