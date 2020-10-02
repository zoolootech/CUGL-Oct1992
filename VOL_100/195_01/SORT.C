/* [SORT.C of JUGPDS Vol.18]
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

/* sort - sort and merge */

#include "stdio.h"
#include "def.h"
#include <dio.h>

#define LINES	2000
#define MAXLEN	128
#define MAXBUF	16
#define MERGEORDER	7

char	allocbuf[1024][MAXBUF], *alloc_bp, *alloc_ep;
char	opt_d;		/* dictionary order */
char	opt_f;		/* fold order	    */
char	opt_n;		/* numeric order   */
char	opt_l;		/* line_no listing  */
char	opt_r;		/* reverse order    */


main(argc, argv)
int argc;
char *argv[];

{
	char	*ap;
	int	i;

	dioinit(&argc, argv);
	if (argc <3) {
		error("Usage: sort -{dfnlr} file1 file2 ... >outfile");
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
	FileSort();
	dioflush();
}


FileSort()
{
	char	fname[15], *lineptr[LINES], opt_ll;
	int	nlines, t, high, low, lim;
	FILE	infp[MERGEORDER], outfp;

	high = 0;
	t = Gtext(lineptr, &nlines, LINES, STDIN);
	Sort(lineptr, nlines);
	if (t == EOF) {
		Ptext(lineptr, nlines, STDOUT);
		return;
	}
	opt_ll = opt_l;
	opt_l  = OFF;
	MakeFile(high, &outfp);
	Ptext(lineptr, nlines, &outfp);
	putc(CPMEOF, &outfp);
	fclose(&outfp);
	while (t != EOF) {
		t = Gtext(lineptr, &nlines, LINES, STDIN);
		Sort(lineptr, nlines);
		MakeFile(++high, &outfp);
		Ptext(lineptr, nlines, &outfp);
		putc(CPMEOF, &outfp);
		fclose(&outfp);
		}
	opt_l = opt_ll;
	for (low = 0; low <= high; low += MERGEORDER) {
		lim = min(low + MERGEORDER - 1, high);
		Gopen(infp, low, lim);
		MakeFile(++high, &outfp);
		Merge(infp, lim-low+1, &outfp);
		putc(CPMEOF, &outfp);
		fclose(&outfp);
		Gremove(infp, low, lim);
	}

	sprintf(fname, "stemp.%d", high);
	if (fopen(fname, infp) == ERROR) {
		fprintf(STDERR, "file not open: %s\n", fname);
		exit();
		}
	Fcopy(infp, STDOUT);
	fclose(infp);
	unlink(fname);
}


MakeFile(fno, fp)
FILE	*fp;
{
	char	fname[15];

	sprintf(fname, "stemp.%d", fno);
	if (fcreat(fname, fp) == ERROR) {
		fprintf(STDERR, "file not creat: %s\n", fname);
		exit();
		}
}


Fcopy(inbuf, outbuf)
FILE *inbuf, *outbuf;
{
	int c, lno;

	lno = 1;
	while (fgetlin(inbuf, allocbuf, MAXLEN) > 0) {
		if( opt_l == ON )
			fputdec(outbuf, lno++, 6);
		fputlin(outbuf, allocbuf);
		if (outbuf != STDOUT)
			putch(NEWLINE, outbuf);
	}
}


Gopen(fp, low, lim)
FILE	fp[];
{
	int	i;
	char	fname[15];

	for (i = 0; i < lim - low + 1; i++) {
		sprintf(fname, "stemp.%d", low + i);
		if (fopen(fname, fp[i]) == ERROR) {
			fprintf(STDERR, "file not open: %s\n", fname);
			exit();
		}
	}
}


Gremove(fp, low, lim)
FILE	fp[];
{
	char	fname[15];
	int	i;

	for (i = 0; i < lim - low + 1; i++) {
		putc(CPMEOF, fp[i]);
		fclose(fp[i]);
		sprintf(fname, "stemp.%d", low + i);
		unlink(fname);
	}
}

/* merge - merge infile(1)...infile(nfiles) onto outfile */
Merge(infile, nfiles, outfile)
FILE infile[], *outfile;
{
	char	*lineptr[MERGEORDER], *p;
	int	i, nl, inf, len;
	int	strdfcmp(), strfcmp(), strcmp(), strdcmp(), numcmp(), swap();

	p = alloc_bp = allocbuf;
	for (nl = 0, i = 0; i < nfiles; i++)
		if ((len = fgetlin(infile[i], alloc_bp, MAXLEN)) > 0) {
			*(alloc_bp + len - 1) = '\0';
			lineptr[nl++] = alloc_bp;
			alloc_bp += MAXLINE;
		}
	Sort(lineptr, nl--);
	while (nl >= 0) {
		fputlin(outfile, lineptr[0]);
		putch(NEWLINE, outfile);
		inf = (lineptr[0] - p) / MAXLINE;
		if ((len =fgetlin(infile[inf], lineptr[0], MAXLEN)) <= 0)
			lineptr[0] = lineptr[nl--];
		else
			*(lineptr[0] + len - 1) = '\0';
		if (opt_n == ON)
			reheap(lineptr, nl, numcmp, swap);
		else if (opt_d == ON && opt_f == ON)
			reheap(lineptr, nl, strdfcmp, swap);
		else if (opt_d == ON)
			reheap(lineptr, nl, strdcmp, swap);
		else if (opt_f == ON)
			reheap(lineptr, nl, strfcmp, swap);
		else
			reheap(lineptr, nl, strcmp, swap);
		}
}

/* reheap - propagat inbuf(lineptr(1)) to proper place in heap */
reheap(lineptr, nl, comp, exch)
char	*lineptr[];
int	(*comp)(), (*exch)();
{
	int	i, f;
	char	*p;

	for (i = 1; i <= nl; i++) {
		f = (*comp)(lineptr[i-1], lineptr[i]);
		if (opt_r == ON && f >= 0)
			break;
		else if (opt_r==OFF && f <= 0)
			break;
		(*exch)(&lineptr[i-1], &lineptr[i]);
	}
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

/* quick - quicksort for character lines */ 
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
	if(l < j) quick(v, l, j, comp, exch);
	if(i < r) quick(v, i, r, comp, exch);
}


numcmp(s1, s2)
char *s1, *s2;

{
	int	atoi(), v1, v2;

	v1 = atoi(s1);
	v2 = atoi(s2);
	if (v1 < v2)
		return(-1);
	else if (v1 > v2)
		return(1);
	else
		return(0);
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
	if (opt_r == ON) lineptr += nlines;
	while (nlines--) {
		if (opt_l == ON)
			fputdec(fp, lno++, 6);
		fputlin(fp, (opt_r == ON ? *--lineptr : *lineptr++));
		putch(NEWLINE, fp);
		}
}


/* fputdec - put decimal integer in field width >= w for FILE *fp */
fputdec(fp, n, w)
FILE *fp;

{
	char	s[64], itoa();
	int	len;

	len = strlen(itoa(s,n));
	if ((len = w - len) > 0)
		while (len--)
			putch(BLANK, fp);
	fputlin(fp, s);
	putch(':', fp);
}


fputlin(fp, s)
char *s;
FILE *fp;
{
	while (*s)
		putch(*s++, fp);
}


putch(c,fp)
FILE *fp;
{
	if (fp == STDOUT)
		putchar(c);
	else {
		if (c == NEWLINE)
			putc(CRETURN, fp);
		putc(c, fp);
	}
}


fgetlin(fp, s, lim)
char	*s;
int	lim;

{
	char	*p;
	int	c;

	p = s;
	while (--lim > 0 && (c = getch(fp)) != EOF && c != NEWLINE)
		*s++ = c;
	if(c == NEWLINE)
		*s++ = c;
	*s = '\0';
	return(s-p);
}


getch(fp)
FILE *fp;

{
	int	c;

	if (fp == STDIN)
		return getchar();
	else {
		if ((c = getc(fp)) == CRETURN)
			return getch(fp);
		if (c == CPMEOF || c == EOF)
			c = EOF;
		return c;
	}
}
