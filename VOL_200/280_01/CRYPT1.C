/* [crypt1.c of JUGPDS Vol.46] */
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

#include "fcntl.h"		/* for open function */

/* crypt1 - encrypt and decrypt for binary file */

#define	MAXKEY	32

void	main(argc, argv)
int	argc;
char	*argv[];

{
int	i, keylen, cnt, n, n2 ,loop;
int	fd1 ,fd2;
char	key[MAXKEY], *p, *q;
char	tbuf[(1024*30)];

	p = tbuf;
	loop = 0;
	if (argc < 4)
		error("CRY901 Usage: crypt0 key infile outfile");
	strcpy(key, argv[1]);
	keylen = strlen(key);
	if ((fd1 = open( argv[2],O_RDONLY | O_RAW,0)) == NO) {
		printf("\CRY902 ncrypt1: can't open %s\n", *argv);
		exit();
		}
	else if ((fd2 = open(argv[3],O_CREAT | O_RAW | O_WRONLY,0)) == NO) {
		printf("\CRY903 ncrypt1: can't open %s\n", *argv);
		exit();
		}
	else
		while ((n = read(fd1, p, 256) ) > 0) {
			q = p;
			cnt = n;
			for (i = 1; cnt--; i= i%keylen + 1)
				*q++ ^= key[i];
			n2 = write(fd2, p, n);
			loop++;
			if (n2 != n) {
				break;
			}
		}
	printf("\nCRY101 %d record write.",loop);
	close(fd1);
	close(fd2);
}
