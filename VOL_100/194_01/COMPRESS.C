/* [COMPRESS.C of JUGPDS Vol.17]
*****************************************************************
*								*
*	Written by Muneaki Chiba 				*
*	Edited by Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Tested by Y. Monma (JUG-C/M Disk Editor)                * 
*								*
*****************************************************************
*/

#include "stdio.h"
#include <dio.h>

#define	RCODE		127
#define	MAXCHUNK	(RCODE-'0'-1)
#define	THRESH		5

main(argc, argv)
int	argc;
char *argv[];

{
	int	c, lastc, nrep, nsave;
	char	buff[MAXCHUNK];

	dioinit(&argc, argv);
	if (argc < 2)
		error("Usage: cmprs <infile >outfile ^Z");
	nsave = 0;
	for (lastc = getchar(); lastc != EOF; lastc = c) {
		for (nrep = 1; (c = getchar()) == lastc; nrep++)
			if (nrep >= MAXCHUNK)
				break;
		if (nrep < THRESH)
			while (nrep--) {
				buff[nsave++] = lastc;
				if (nsave >= MAXCHUNK)
					putbuf(buff, &nsave);
				}
		else {
			putbuf(buff, &nsave);
			putchar(RCODE);
			putchar(lastc);
			putchar(nrep+'0');
 			}
	}
	putbuf(buff, &nsave);
	dioflush();
}


putbuf(p, nsave)
char	*p;
int	*nsave;

{
	int	i;

	if (*nsave) {
		putchar(*nsave + '0');
		while ((*nsave)--)
			putchar(*p++);
 	}
	*nsave = 0;
}
