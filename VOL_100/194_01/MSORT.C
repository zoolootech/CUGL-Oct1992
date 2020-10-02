/* [MSORT.C of JUGPDS Vol.17]
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

/* msort - sort of text (ASCII) file 

	Usage: msort -{dfnlr} <infile >outfile ^Z");
where
	-d: dictionary order 
	-f: fold order
	-n: numeric order
	-l: line # listing
	-r: reverse (decreasing) order 

*/

#include "stdio.h"
#include "def.h"
#include <dio.h>

#define LINES	2000
#define MAXLEN	128
#define MAXBUF	30

char	allocbuf[1024][MAXBUF], *alloc_bp, *alloc_ep;

char	opt_d;		/* dictionary order */
char	opt_f;		/* fold order	    */
char	opt_n;		/* numeric order    */
char	opt_l;		/* line_no listing  */
char	opt_r;		/* reverse order    */


main(argc, argv)
int argc;
char *argv[];

{
	char	*ap;
	int	i;

	dioinit(&argc, argv);
	if (argc < 3) {
		error("Usage: msort -{dfnlr} <infile >outfile ^Z");
		exit();
		}
	else
	opt_d = opt_f = opt_n = opt_l = opt_r = OFF;
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
			}
	fileSort();
	dioflush();
}


FileSort()
{
	char	*lineptr[LINES];
	int	nlines, high;
	FILE	fp1, fp2;

	high = 0;
	if (Gtext(lineptr, &nlines, LINES, STDIN) == EOF) {
		Sort(lineptr, nlines);
		Ptext(lineptr, nlines, STDOUT);
		}
	else
		fprintf(STDERR, "Input too big to sort\n" );
}


Sort(lineptr, nlines)
char *lineptr[];
{
	int	strdfcmp(), strfcmp(), strcmp(), strdcmp(), numcmp(), swap();

	if (opt_n == ON)
		quick(lineptr, 0, nlines-1, numcmp, swap);
	else if (opt_d == ON && opt_f == ON)
		quick(lineptr, 0, nlines-1, strdfcmp, swap);
	else if (opt_d == ON)
		quick(lineptr, 0, nlines-1, strdcmp, swap);
	else if (opt_f == ON)
		quick(lineptr, 0, nlines-1, strfcmp, swap);
	else
		quick(lineptr, 0, nlines-1, strcmp, swap);
}

 
Gtext(lineptr, nlines, maxlines, fp)
char	*lineptr[];
int	*nlines;
FILE	*fp;
{
	int	len;

	*nlines = 0;
	alloc_bp = allocbuf;
	alloc_ep = &allocbuf[1023][(MAXBUF-1)];
	do {
		if ((len = fgetlin(fp, alloc_bp, MAXLEN)) <= 0)
			return EOF;
		lineptr[(*nlines)++] = alloc_bp;
		alloc_bp += len;
		*(alloc_bp - 1) = '\0';
		} while (alloc_bp + MAXLEN <= alloc_ep && *nlines < maxlines);
	return(len);
}


Ptext(lineptr, nlines, fp)
char	*lineptr[];
int	nlines;
FILE	*fp;
{
	int	lno;

	lno = 1;
	if( opt_r == ON ) lineptr += nlines;
	while (nlines--) {
		if( opt_l == ON )
			fprintf(fp, "%6u: ", lno++);
		fprintf(fp, "%s\n", ( opt_r == ON ? *--lineptr : *lineptr++) );
		}
}


numcmp(s1, s2)
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


swap(px, py)
char	*px[], *py[];
{
	char	*temp;

	temp = *px;
	*px = *py;
	*py = temp;
}
