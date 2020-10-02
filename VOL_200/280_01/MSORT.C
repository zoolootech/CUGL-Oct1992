/* [msort.c of JUGPDS Vol.46] */
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

#define	UBUFSIZE	64


/* msort - sort of text (ASCII) file 

	Usage: msort -{dfnlr} <infile >outfile ^Z");
where
	-d: dictionary order 
	-f: fold order
	-n: numeric order
	-l: line # listing
	-r: reverse (decreasing) order 

*/

#define LINES	1000					/* Modify 1986-11-28 */
#define MAXLEN	128
#define MAXBUF	30

char	allocbuf[LINES][MAXBUF], *alloc_bp, *alloc_ep;

char	opt_d;		/* dictionary order */
char	opt_f;		/* fold order	    */
char	opt_n;		/* numeric order    */
char	opt_l;		/* line_no listing  */
char	opt_r;		/* reverse order    */

int	debug;					/* debug switch */

void main(argc, argv)
int argc;
char *argv[];

{
char	*ap;
int	i;
void    filesort();

	if (argc < 1) {
		error("MSO901 Usage: msort -{dfnlr} <infile >outfile");
		exit(1);
		}
	else
	opt_d = opt_f = opt_n = opt_l = opt_r = OFF;
	debug = OFF;
	i = 0;
	while (--argc > 0 && (*++argv)[0] == '-')
		for (ap = argv[0]+1; *ap != '\0'; ap++)
			switch (tolower(*ap)) {
			case 'd':
				opt_d = ON;
				break;
			case 'f':
				opt_f = ON;
				break;
			case 'n':
				opt_n = ON;
				break;
			case 'l' :
				opt_l = ON;
				break;
			case 'r':
				opt_r = ON;
				break;
			case 'x':
				debug = ON;
				break;
			}
	if(debug) {
	  fprintf(STDERR,"\nMSO801 debug data for msort\n       ");
	  fprintf(STDERR,"opt_d %d,opt_f %d,opt_n %d,opt_l %d,opt_r %d",
				opt_d,opt_f,opt_n,opt_l,opt_r);
	}
	filesort();
}

void filesort()
{
char	*lineptr[LINES];
int	nlines, high;
void	sort(),ptext();

	high = 0;
	if (gtext(lineptr, &nlines, LINES, STDIN) == EOF) {
		sort (lineptr, &nlines);
		ptext(lineptr, &nlines, STDOUT);
		}
	else
		fprintf(STDERR, "MSO902 Input too big to sort\n" );
}


void sort(lineptr, nlines)
char *lineptr[];
int  *nlines;
{
void quick(),swap();
int	strdfcmp(), strfcmp(), strcmp(), strdcmp(), numcmp();

	if (opt_n == ON)
		quick(lineptr, 0, (*nlines)-1, numcmp, swap);
	else if (opt_d == ON && opt_f == ON)
		quick(lineptr, 0, (*nlines)-1, strdfcmp, swap);
	else if (opt_d == ON)
		quick(lineptr, 0, (*nlines)-1, strdcmp, swap);
	else if (opt_f == ON)
		quick(lineptr, 0, (*nlines)-1, strfcmp, swap);
	else
		quick(lineptr, 0, (*nlines)-1, strcmp, swap);
}

 
int  gtext(lineptr, nlines, maxlines, fp)
char	*lineptr[];
int	*nlines;
int	maxlines;
FILE	*fp;
{
int	len;

	*nlines = 0;
	alloc_bp = &allocbuf[0][0];
	alloc_ep = &allocbuf[(LINES-1)][(MAXBUF-1)];
	do {
		if ((len = fgetlin(fp, alloc_bp, MAXLEN)) <= 0)
			return EOF;
		lineptr[(*nlines)++] = alloc_bp;
		alloc_bp += len;
		*(alloc_bp - 1) = '\0';
		} while (alloc_bp + MAXLEN <= alloc_ep && *nlines < maxlines);
	return(len);
}

void ptext(lineptr, nlines, fp)
char	*lineptr[];
int	*nlines;
FILE	*fp;
{
int	lno;

	lno = 1;
	if( opt_r == ON ) lineptr += (*nlines);
	while ((*nlines)--) {
		if( opt_l == ON )
			fprintf(fp, "%6u: ", lno++);
		fprintf(fp, "%s\n", ( opt_r == ON ? *--lineptr : *lineptr++) );
		}
}

int  numcmp(s1, s2)
char *s1, *s2;
{
	int	atoi(), v1, v2;

	v1 = atoi(s1);
	v2 = atoi(s2);
	if(v1 < v2)
		return(-1);
	else if(v1 > v2)
		return(1);
	else
		return(0);
}

void quick(v, l, r, comp, exch)
char	*v[];
int	l, r;
int	(*comp)(), (*exch)();
{
char    *vx;
int	i, j;

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


void swap(px, py)
char	*px[], *py[];
{
char	*temp;

	temp = *px;
	*px = *py;
	*py = temp;
}
