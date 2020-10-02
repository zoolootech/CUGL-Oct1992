/* [TRANSLIT.C of JUGPDS Vol.46] */
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
*	Compiler Option: -cc -k0(1) -ms -n -v -w		*
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

#define	UBUFSIZE	64

/* translit - map characters */

int	debug;					/* debug switch */

void main(argc, argv)
int	argc;
char	*argv[];

{
char	from[MAXSET], to[MAXSET], *ap;
int	c, allbut, collap, i, lastto;
void error();
int makset(),xindex();	

	debug = NO;
	if(argc < 2)
		error("TRA901 Usage: >translit from to (-d)\n");
	ap = argv[1];
	if (*ap == NOT) {
		allbut = YES;
		ap++;
		}
	else
		allbut = NO;
	if (makset(ap, 0, from, MAXSET) == NO)
		error("TRA902 from: too large.\n");
	ap = argv[2];
	if (argc == 2)
		to[0] = EOS;
	else if (makset(ap, 0, to, MAXSET) == NO)
		error("TRA903 to: too large.\n");
	if(argc ==4)
		if( *(argv[3]) == '-' && tolower(*(argv[3]+1)) == 'd')
			debug =YES;
	lastto = strlen(to);
	collap = ( (strlen(from) > lastto || allbut == YES) ? YES : NO );
	lastto--;
	if(debug) {				/* append 1986-11-26 */
	    fprintf(stderr,"\nTRS801 Debug data for translit");
	    fprintf(stderr,"\n       from = %s",from);
	    fprintf(stderr,"\n       to   = %s",to);
	    fprintf(stderr,"\n       YES  = %d , NO = %d",YES,NO);
	    fprintf(stderr,"\n       lastto = %d , collap = %d ,allbut = %d\n"
			      , lastto ,collap ,allbut );
	}
	FOREVER {
		i = xindex(from, (c=getchar()), allbut, lastto);
		if(debug) {
			printf("\nTRS802 Debug data for translit");
			printf("\n       i = %d , c = %x ",i ,c);
			}
		if (collap == YES && i >= lastto && lastto >= 0) {
			putchar(to[lastto]);
			while((	i= xindex(from, (c=getchar()), allbut, lastto)) 				>= lastto);
			}
		if (c == EOF)
			break;
		if (i >= 0 && lastto >= 0)
			putchar(to[i]);
		else if (i == ERROR)
			putchar(c);
		}
}

int  makset(array, k, set, size)
char *array, *set;

{
int	i, j;

	i = k;
	j = 0;
	filset(EOS, array, &i, set, &j, size);
	return (addset(EOS, set, &j, size));
}
