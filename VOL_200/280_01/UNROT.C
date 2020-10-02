/* [UNROT.C of JUGPDS Vol.46]*/
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

#include "stdio.h"
#include "dos.h"
#include "tools.h"
#include "toolfunc.h"

#define	FOLD	36
#define	MAXOUT	80
#define	MIDDLE	40

char fold;

void putlin();

void main(argc, argv)
int	argc;
char 	**argv;
{
int	i, j;
char	inbuf[MAXLINE], outbuf[MAXOUT+1];
char	*ap;

    fold = FOLD;
    while (--argc > 0) {
	if ((*++argv)[0] == '-')
	    for (ap = (*argv)+1; *ap != NULL; ap++) {
		if (tolower(*ap) == 'f') {
		    if(*(ap+1) != NULL)
			fold = *(++ap);
		    else 
			error("UNR901 Do specify FOLD char");
		}
		else
		    error("UNR999 Usage: unrot [-fFOLD] <infile >outfile");
	    }
    }
    while (getlin(inbuf, MAXLINE) > 0) {
	for (i = 0; i < MAXOUT; i++)
	    outbuf[i] = BLANK;
	j = MIDDLE;
	for (i = 0; inbuf[i] != fold && inbuf[i] != NEWLINE; i++) {
	    j++;
	    outbuf[i] = BLANK;
	    if (j >= MAXOUT - 1)
		j = 0;
	    outbuf[j] = inbuf[i];
	}
	if (inbuf[i] == fold) {
	    j = MIDDLE;
	    for (i = index(inbuf, NEWLINE) - 1; i > 0; i--) {
		if (inbuf[i] == fold)
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
	    putlin(outbuf);
	}
}

void putlin(buf)
char *buf;
{
    while (*buf)
	putchar(*buf++);
}

int index(str, c)
char	*str;
int	c;
{
int	i;

    for (i = 0; *str && *str != c; i++ )
	*str++;
    return ( *str ? i : ERROR);
}
