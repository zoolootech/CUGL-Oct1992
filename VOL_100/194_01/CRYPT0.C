/* [CRYPT0.C of JUGPDS Vo.17]
*********x********************************************************
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

/* crypt - encryt and decrypt */

#include "stdio.h"
#include <dio.h>

#define	MAXKEY	32

main(argc, argv)
int	argc;
char *argv[];

{
	int	i, keylen, c;
	char	key[MAXKEY];

 
	dioinit(&argc, argv);
	if (argc < 2) {
		error("Usage: crypt0 key <infile >outfile");
		exit();
		}
	strcpy(key, argv[1]);
	if ((keylen = strlen(key)) <= 0) {
		error("Usage: crypt0 key");
		exit();
		}
	for (i = 1; (c=getchar()) != EOF && c != CPMEOF; i= i%keylen + 1)
		if ((c ^ key[i]) == CPMEOF)
			putchar((c ^ key[i]) ^ key[i]);
		else
			putchar(c ^ key[i]);
	dioflush();
}
