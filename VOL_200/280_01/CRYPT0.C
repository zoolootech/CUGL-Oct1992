/* [crypt0.c of JUGPDS Vol.46] */
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

/* crypt - encryt and decrypt */

#define	MAXKEY	32

void	main(argc, argv)
int	argc;
char	*argv[];

{
int	i, keylen, c;
char	key[MAXKEY];
 
	if (argc < 2) {
		error("CRY911 Usage: crypt0 key <infile >outfile");
		exit(1);
		}
	strcpy(key, argv[1]);
	if ((keylen = strlen(key)) <= 0) {
		error("CRY912 Usage: crypt0 key");
		exit(1);
		}
#if LATTICE
	fmode(stdin,1);			/* Set stdin  to binary mode */
	fmode(stdout,1);		/* Set stdout to binary mode */
#endif
	for (i = 1; (c=getchar()) != EOF; i= i%keylen + 1)
		if ((c ^ key[i]) == CPMEOF)
			putchar((c ^ key[i]) ^ key[i]);
		else
			putchar(c ^ key[i]);
}
