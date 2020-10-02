/* [concat0.c of JUGPDS Vol.46] */
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

/* concat - concatenate named files onto standard output */

void    main(argc, argv)
int	argc;
char 	**argv;

{
FILE *fp;
void	filecopy();

	if (argc < 2) {
		error("CON911 Usage: concat0 file1 file2 ... >outfile");
		exit();
		}
	if (argc == 1)
		filecopy(STDIN);
	else
		while (--argc > 0)
			if ((fp = fopen(*++argv,"r")) == NO ) {
				printf("\nCON912 can't open %s\n", *argv);
				exit(1);
				}
			else {
				filecopy(fp);
				fclose(fp);
				}
}


void filecopy(inbuf)
FILE *inbuf;
{
int c;

	while ((c = getc(inbuf)) != EOF)
		putchar(c);
}
