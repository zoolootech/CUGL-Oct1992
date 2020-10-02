/* [KWIC.C of JUGPDS Vol.46] */
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

/* kwic - make keyword in context index */

#include "stdio.h"
#include "dos.h"
#include "tools.h"
#include "toolfunc.h"

#define	FOLD	36	/* FOLD = '$' */

int	cc;
char	fold;

void putrot(),rotate(),putchr();

void main(argc, argv)
int	argc;
char 	**argv;
{
char	buf[MAXLINE];
char	*ap;
    fold = FOLD;
    while (--argc > 0) {
	if ((*++argv)[0] == '-')
	    for (ap = (*argv)+1; *ap != NULL; ap++) {
		if (tolower(*ap) == 'f') {
		    if(*(ap+1) != NULL)
			fold = *(++ap);
		    else 
			error("KWI901 Do specify FOLD char");
		}
		else
		    error("KWI999 Usage: kwic [-fFOLD] <infile >outfile");
	    }
    }
    cc = 'a';
    while (getlin(buf, MAXLINE) > 0)
	putrot(buf);
}

/* putrot - create lines with keyword at front */
void putrot(buf)
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
void rotate(buf, n)
char buf[];
{
int	i;
    for (i = n; buf[i] != NEWLINE; i++)
	putchr(buf[i]);
    putchr(fold);
    for (i = 0; i < n; i++)
	putchr(buf[i]);
    putchr(NEWLINE);
}

void putchr(c)
int	c;
{
    if (c == '\t')
	c = BLANK;
    if (c != BLANK || cc != BLANK)
	putchar(c);
    cc = c;
}
