/* [TRANSLIT.C of JUGPDS Vol.17]
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
	Usage: tr from to 
*/

/* translit - map characters */

#include "stdio.h"
#include "def.h"

#define	BDS		1

#ifdef	BDS
#include <dio.h>
#define	stderr		STDERR
#define	stdin		STDIN
#endif

#define MAXSET 256

main(argc, argv)
int	argc;
char	*argv[];

{
	char	from[MAXSET], to[MAXSET], *temp, *ap;
	int	c, allbut, collap, i, lastto;
	
#ifdef	BDS
	dioinit(&argc,argv);
#endif
	if(argc < 2) {
		fprintf(stderr, ">tr from to.\n");
		exit();
		}
	ap = argv[1];
	if (*ap == NOT) {
		allbut = YES;
		ap++;
		}
	else
		allbut = NO;
	if (makset(ap, 0, from, MAXSET) == NO) {
		fprintf(stderr, "from: too large.\n");
		exit();
		}
	ap = argv[2];
	if (argc == 2)
		to[0] = EOS;
	else if (makset(ap, 0, to, MAXSET) == NO) {
		fprintf(stderr, "to: too large.\n");
		exit();
		}
	lastto = strlen(to);
	collap = ( (strlen(from) > lastto || allbut == YES) ? YES : NO );
	lastto--;
	while(1) {
		i = xindex(from, (c=getchar()), allbut, lastto);
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
#ifdef	BDS
	dioflush();
#endif
}

makset(array, k, set, size)
char *array, *set;

{
	int	i, j;

	i = k;
	j = 0;
	filset(EOS, array, &i, set, &j, size);
	return (addset(EOS, set, &j, size));
}
