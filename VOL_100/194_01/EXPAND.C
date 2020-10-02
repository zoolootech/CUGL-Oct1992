/ª [EXPAND.C of JUGPDS Vol.17]
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

/*  expand - uncompress standard input */

#include "stdio.h"
#include <dio.h>

#define	RCODE		127

main(argc,argv)
int	argc;
char	*argv[];

{
	int	c, code;

	dioinit(&argc, argv);
	if (argc < 2)
		error("Usage: expnd <infile >outfile ^Z");
	while ((code = getchar()) != EOF)
		if (code == RCODE) {
			if ((c = getchar()) == EOF)
				break;
			if ((code = getchar()) == EOF)
				break;
			code -= '0';
			while (code--)
				putchar(c);
		} else {
			code -= '0';
			while (code--) {
				if ((c = getchar()) == EOF)
					break;
				putchar(c);
				}
			if ( c == EOF )
				break;
			}
	dioflush();
}
