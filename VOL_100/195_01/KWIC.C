/* [KWIC.C of JUGPDS Vol.18]
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

/* kwic - make keyword in context index */

#include <stdio.h>
#include <dio.h>

#define	FOLD	36	/* FOLD = '$' */

int	cc;

main(argc, argv)
int	argc;
char 	**argv;

{
	char	buf[MAXLINE];

	dioinit(&argc, argv);
	if (argc <2) {
		error("Usage: kwic <infile >outfile ^Z");
		exit();
	}
	cc = 'a';
	while (getlin(buf, MAXLINE) > 0)
		putrot(buf);
	dioflush();
}


/* putrot - create lines with keyword at front */
putrot(buf)
char buf[];
{
	int	i, t;

	for (i = 0; buf[i] != NEWLINE; i++) {
		t = type(buf[i]);
		if (t == LETTER || t == DIGIT) {
			rotate(buf, i);
			t = type(buf[i+1]);
			for (; t == LETTER || t == DIGIT; t = type(buf[i+1]))
				i++;
		}
	}
}


/* rotate - output rotated line */
rotate(buf, n)
char buf[];
{
	int	i;

	for (i = n; buf[i] != NEWLINE; i++)
		putchr(buf[i]);
	putchr(FOLD);
	for (i = 0; i < n; i++)
		putchr(buf[i]);
	putchr(NEWLINE);
}

putchr(c)
int	c;
{
	if (c == '\t')
		c = BLANK;
	if (c != BLANK || cc != BLANK)
		putchar(c);
	cc = c;
}


getlin(s, lim)
char	*s;
int	lim;
{
	char	*p;
	int	c;

	p = s;
	while(--lim > 0 && (c = getchar()) != EOF && c != NEWLINE)
		*s++ = c;
	if(c == NEWLINE)
		*s++ = c;
	*s = EOS;
	return(s-p);
}
