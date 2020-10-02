/* [CONCAT0.C of JUGPDS Vol.17]
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

/* concat - concatenate named files onto standard output */

#include "stdio.h"
#include <dio.h>

main(argc, argv)
int	argc;
char 	**argv;

{
	struct _buf inbuf, *fp;

	if (wildexp(&argc,&argv) == ERROR)
		exit(puts("WILDEXP overflow!"));
	dioinit(&argc, argv);
	if (argc < 2) {
		error("Usage: cat0 file1 file2 ... >outfile");
		exit();
		}
	fp = &inbuf;
	if (argc == 1)
		filecopy(STDIN);
	else
		while (--argc > 0)
			if (fopen(*++argv, fp) == 0) {
				fprintf(STDERR, "cat: can't open %s\n", *argv);
				exit(1);
				}
			else {
				filecopy(fp);
				fclose(fp);
				}
	dioflush();
}


filecopy(inbuf)
FILE *inbuf;
{
	int c;

	while ((c = getc(inbuf)) != EOF && c != CPMEOF) {
		if (c == '\r')
			if ((c = getc(inbuf)) != '\n')
				putchar('\r');
		putchar(c);
		}
}
