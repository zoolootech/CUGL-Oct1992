/* [CRYPT1.C of JUGPDS Vol.17]
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

/* crypt1 - encrypt and decrypt for binary file */

#include "stdio.h"

#define	MAXKEY	32

main(argc, argv)
int	argc;
char *argv[];

{
	int	i, keylen, cnt, n, fd, n2, fd2;
	char	key[MAXKEY], *p, *q;
	char	tbuf[(1024*30)];

	p = tbuf;
	if (argc < 4)
		error("Usage: crypt0 key infile outfile");
	strcpy(key, argv[1]);
	keylen = strlen(key);
	if ((fd = open( argv[2], 0)) == ERROR) {
		fprintf(STDERR, "crypt1: can't open %s\n", *argv);
		exit();
		}
	else if ((fd2 = creat( argv[3])) == ERROR) {
		fprintf(STDERR, "crypt1: can't open %s\n", *argv);
		exit();
		}
	else
		while ((n = read(fd, p, 240) ) > 0) {
			printf("%4d sectors READ.\n", n);
			q = p;
			cnt = n * 128;
			for (i = 1; cnt--; i= i%keylen + 1)
				*q++ ^= key[i];
			n2 = write(fd2, p, n);
			printf("%4d sectors WRITE.\n", n2);
			if (n2 != n) {
				break;
			}
		}
	close(fd);
	close(fd2);
}
