/* [compress.c of JUGPDS Vol.46] */
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

#define	RCODE		127
#define	MAXCHUNK	(RCODE-'0'-1)
#define	THRESH		5

void	main(argc, argv)
int	argc;
char    *argv[];

{
int	c, lastc, nrep, nsave;
char	buff[MAXCHUNK];
void	putbuf();

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
}


void	putbuf(p, nsave)
char	*p;
int	*nsave;

{
	if (*nsave) {
		putchar(*nsave + '0');
		while ((*nsave)--)
			putchar(*p++);
	}
	*nsave = 0;
}
