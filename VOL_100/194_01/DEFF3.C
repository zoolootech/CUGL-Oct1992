/* [DEFF3.C of JUGPDS Vol.17]
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

/* Library functions for Software Tools */

#include "stdio.h"

#define	UBUFSIZE	64

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
	*s = '\0';
	return(s-p);
}


getwrd (in, i, out)
char in[], out[];
int  *i;
{
	int	j;

	while (in[(*i)] == ' ' || in[(*i)] == '\t')
	        (*i)++;
	j = 0;
	while (in[(*i)] != '\0' && in[(*i)] != ' '
		&& in[(*i)] != '\t' && in[(*i)] != NEWLINE) {
		out[j] = in[(*i)];
        	(*i)++;
	        j++;
        	}
	out[j] = '\0';
	return j;
}


getword(w, lim)
char *w;
int  lim;
{
	int c, t;

	if( type(*w++ = c = getchar()) != LETTER) {
		*w = '\0';
		return(c);
	}
	while(--lim > 0) {
		t = type(*w++ = c = getchar());
		if( t != LETTER && t != DIGIT ) {
			ungetch(c);
			break;
		}
	}
	*(w-1) = '\0';
	return(LETTER);
}


type(c)
int c;
{
	if(isdigit(c)) return(DIGIT);
	if(isalpha(c)) return(LETTER);
	return(c);
}


fgetlin(fd, s, lim)
char	*s;
int	lim;
{
	char	*p;
	int	c;

	p = s;
	while(--lim > 0 && (c=getc(fd)) != EOF && c != CPMEOF && c != NEWLINE)
		*s++ = c;
	if(c == NEWLINE)
		*s++ = c;
	*s = '\0';
	return(s-p);
}

error(str)
{
	fprintf(STDERR,"%s\n", str);
	exit();
}


char *itoa(s,n)
char *s;
{
	char *p;

	p = s;
	if (n < 0) {
		*s++ = '-';
		n = -n;
		}
	s = _itoa(s,n);
	*s = '\0';
	return p;
}


_itoa(s,n)
char *s;
{
	int i;

	if ((i = n/10) != 0)
		s = _itoa(s,i);
	*s++ = n % 10 + '0';
	return s;
}


quick(v, l, r, comp, exch)
char	*v[];
int	l, r;
int	(*comp)(), (*exch)();
{
	int	vx, i, j;

	i = l;  j = r;
	vx = v[ (l+r)/2 ];
	while(i <= j) {
		while( (*comp)(v[i], vx) < 0 )
			i++;
		while( (*comp)(vx, v[j]) < 0 )
			j--;
		if(i <= j) {
			(*exch)(&v[j], &v[i]);
			i++;
			j--;
			}
		}
	if(l < j) quick(v,l,j,comp,exch);
	if(i < r) quick(v,i,r,comp,exch);
}

atoi(n)
char *n;
{
	int val, sign;
	char c;

	val=0;	sign=1;
	while (iswhite(c = *n)) ++n;
	if (c== '-') {sign = -1; n++;}
	while (isdigit(c = *n++)) val = val * 10 + c - '0';
	return sign*val;
}

char *strsave(s)
char *s;
{
	char *p, *alloc();

	if ((p = alloc(strlen(s)+1)) != NULL)
		strcpy(p, s);
	return p;
}


char *strcat(s, t)
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


strdfcmp(s,t)
char *s, *t;
{
int i;

i = 0;
	do {
		if (i > 0)
		  {s++; t++;}

		while( !(isalpha(*s) || isspace(*s) || isdigit(*s)) )
			s++;
		while( !(isalpha(*t) || isspace(*t) || isdigit(*t)) )
			t++;
		i++;
		} while (tolower(*s) == tolower(*t) && *s != '\0');
	return( tolower(*s) - tolower(*t) );
}


strdcmp(s,t)
char *s, *t;
{
int i;

i = 0;
	do {
		if (i > 0)
		  {s++; t++;}

		while( !(isalpha(*s) || isspace(*s) || isdigit(*s)) )
			s++;
		while( !(isalpha(*t) || isspace(*t) || isdigit(*t)) )
			t++;
		i++;
		} while (*s == *t && *s != '\0');
	return( *s - *t);
}


strfcmp(s,t)
char *s, *t;
{
	for ( ; tolower(*s) == tolower(*t); s++, t++)
		if (*s == '\0')
			return 0;
	return(tolower(*s) - tolower(*t));
}

poke(c,d)
char *c, d;
{
	*c = d;
}

/*----------------------------------------------------------*/
/* Some functions from "The C programing language"
                  by KERNIGHAN & RITCHIE */
/*----------------------------------------------------------*/


swap(px, py)
int	*px, *py;
{
	int	temp;

	temp = *px;
	*px = *py;
	*py = temp;
}

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
