/* [misc.c of JUGPDS Vol.17] */
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
/****************************************************************************/
/*									    */
/*	All functions do not use Lattice C V3.1J			    */
/*									    */
/****************************************************************************/

#include "stdio.h"
#include "dos.h"
#include "ctype.h"
#include "tools.h"

#if LATTICE
#else
char *strcat(s, t)	/* do not use Lattice C Compiler V3.1 */
char *s, *t;
{
char *p;

	p = s;
	while(*s)
		s++;
	strcpy(s, t);
	return p;
}

int strlen(s)
char *s;
{
char *p;
	p = s;
	while (*p) p++;
	return(p - s);
}

char *strcpy(s, t)
char *s, *t;
{
char *p;
	p = s;
	while (*s++ = *t++)
		;
	return p;
}

int strcmp(s,t)
char *s, *t;
{
	for ( ; *s == *t; s++, t++)
		if (*s == '\0')
			return 0;
	return(*s - *t);
}
#endif

/* */
/****************************************************************************/
/*	This function not be user for 8086 System			    */
/****************************************************************************/
/*
poke(c,d)
char *c, d;
{
	*c = d;
}
*/

/****************************************************************************/


/* Can't compile by [BDS-C compiler]

fillbuf(fp)
register FILE *fp;
{
	static char smallbuf[_NFILE];	/* for unbuffering input */
	char *calloc();

	if ((fp->_flag & _READ) == 0 || (fp->_flag & (_EOF | _ERR)) != 0)
		return(EOF);
	while (fp->_base == NULL)
		if (fp->_flag & _UNBUF)
			fp->_base = &smallbuf[fp->_fd];
		else if ((fp->_base = calloc(_BUFSIZE, 1)) == NULL)
			fp->_flag |= _UNBUF;
		else
			fp->_flag |= _BIGBUF;
	fp->_ptr = fp->_base;
	fp->_cnt = read(fp->_fd, fp->_ptr, fp->_flag & _UNBUF ? 1 : _BUFSIZE);
	if (--fp->_cnt < 0) {
		if (fp->_cnt == -1)
			fp->_flag |= _EOF;
		else
			fp->_flag |= _ERR;
		fp->_cnt = 0;
		return(EOF);
	}
	return(*fp->_ptr++ & CMASK);
}


flushbuf(c,fp)
register FILE *fp;
register char c;
{
	extern FILE _iob[];
	int n;

	if (fp->_base == NULL)
		return(EOF);
	if ((fp->_flag & _WRITE) == 0 || (fp->_flag & (_EOF|_ERR)) != 0)
		return(EOF);
	fp->_ptr = fp->_base;
	n = ( fp->_flag & _UNBUF ) ? 1 : _BUFSIZE;
	if (write(fp->_fd, fp->_ptr, n) != n) {
		fp->_flag |= _ERR;		
		fprintf(stderr, "write error %d\n", fp->_fd);
		exit(1);
		}
	fp->_ptr = fp->_base;
	*fp->_ptr++ = c;
	fp->_cnt = n-1;
	return(c);
}


FILE *fopen(name, mode)
register char *name, *mode;
{
	register int fd;
	register FILE *fp;


	if (*mode != 'r' && *mode != 'w' /* && *mode != 'a' */) {
		fprintf(stderr, "illegal mode %s opening %s\n", mode, name);
		exit(1);
		}
	for (fp = _iob; fp < _iob + _NFILE; fp++)
 		if ((fp->_flag & (_READ | _WRITE)) == 0)
			break;
	if (fp >= _iob + _NFILE)
		return(NULL);
	if (*mode == 'w')
		fd = open(name, WRITE);
	else
		fd = open(name, READ);
	if (fd = -1)
		return(NULL);

	fp->_fd = fd;
	fp->_cnt = 0;
	fp->_base = NULL;
	fp->_flag &= ~(_READ | _WRITE);
	fp->_flag |= (*mode == 'r') ? _READ : _WRITE;
	return(fp);
}


char *calloc(n, size)
unsigned n, size;
{
	char *p, *cp;
	char *alloc();
	int i, j;


	for (i = 0; i < n; i++) {
		if ((p = alloc(size) ) == NULL)
			return(NULL);
		cp = p;
		for (j = 0; j < size; j++)
			*cp++ = 0;
		}
	return((char *)p );
}
*/
