/* [COUNT.C of JUGPDS Vol.17]
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

/* count - count characters, lines & words of a file */

#include "stdio.h"
#include <dio.h>	/* Link DIO.CRL */

main(argc, argv)
int	argc;
char	**argv;

{
	int   c, wc, nl, nc, inword;

	dioinit(&argc, argv);	/* Initialize redirection */
	if (argc < 2)		/* Check command lines */
        	error("Usage: count <infile >outfile ^Z");
	nc = 0;
	nl = 0;
	wc = 0;
	inword = NO;
	while ( (c = getchar() ) != EOF ) {
		if ( c == BLANK || c == TAB )
			inword = NO;
		else if ( c == NEWLINE ) {
			inword = NO;
			nl++;
			}
		else if ( inword == NO ) {
			inword = YES;
			wc++;
			}
		nc++;
		}
	printf("chars =%6d, words =%5d, lines =%4d\n", nc, wc, nl);
	dioflush();
}
