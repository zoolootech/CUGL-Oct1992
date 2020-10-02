/* [UNROT.C of JUGPDS Vol.18]
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

#include <stdio.h>
#include <dio.h>

#define	FOLD	36
#define	MAXOUT	80
#define	MIDDLE	40

main(argc, argv)
int	argc;
char 	**argv;

{
	int	i, j;
	char	inbuf[MAXLINE], outbuf[MAXOUT+1];

	dioinit(&argc, argv);
	if (argc <2) {
		error("Usage: unrot <infile >outfile ^Z");
		exit();
	}
	while (getlin(inbuf, MAXLINE) > 0) {
		for (i = 0; i < MAXOUT; i++)
			outbuf[i] = BLANK;
		j = MIDDLE;
		for (i = 0; inbuf[i] != FOLD && inbuf[i] != NEWLINE; i++) {
			j++;
			outbuf[i] = BLANK;
			if (j >= MAXOUT - 1)
				j = 0;
			outbuf[j] = inbuf[i];
		}
		if (inbuf[i] == FOLD) {
			j = MIDDLE;
			for (i = index(inbuf, NEWLINE) - 1; i > 0; i--) {
				if (inbuf[i] == FOLD)
					break;
				if (--j < 0)
					j = MAXOUT - 2;
				outbuf[j] = inbuf[i];
			}
		}
		for (i = MAXOUT - 2; i > 0; i--)
			if (inbuf[i] != BLANK)
				break;
		outbuf[i + 1] = NEWLINE;
		outbuf[i + 2] = EOS;
		putlin(outbuf, STDOUT);
	}
	dioflush();
}


putlin(buf)
char *buf;
{
	while (*buf)
		putchar(*buf++);
}


index(str, c)
char	*str;
int	c;
{
	int	i;

	for (i = 0; *str && *str != c; i++ )
		*str++;
	return ( *str ? i : ERROR);
}
