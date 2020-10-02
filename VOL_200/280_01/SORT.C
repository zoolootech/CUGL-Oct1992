/* [SORT.C of JUGPDS Vol.46]*/
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

/* sort - sort and merge */

#include "stdio.h"
#include "dos.h"
#include "tools.h"
#include "toolfunc.h"

#define LINES	4000
#define MAXLEN	128
#define MAXBUF	16
#define MERGEORDER	7

char	allocbuf[1024][MAXBUF], *alloc_bp, *alloc_ep;
char	opt_d;		/* dictionary order */
char	opt_f;		/* fold order	    */
char	opt_n;		/* numeric order   */
char	opt_l;		/* line_no listing  */
char	opt_r;		/* reverse order    */

void FileSort(),Sort(),MakeFile(),Fcopy(),Gopen(),Gremove(),Merge();
void reheap(),sort(),quick(),Ptext(),putdec(),fputlin(),putch(),fputdec();

void main(argc, argv)
int argc;
char *argv[];
{
char	*ap;
int	i;

    if (argc <3)
	error("SRT999 Usage: sort -{dfnlr} file1 file2 ... >outfile");
    else
	opt_d = opt_f = opt_n = opt_l = opt_r = OFF;
    i = 0;
    while ( (*++argv)[0] == '-' ) {
	for (ap = argv[0]+1; *ap != NULL; ap++)
	    switch (tolower(*ap)) {
	    case 'd':
		{ opt_d = ON;break;};
	    case 'f':
		{ opt_f = ON;break;};
	    case 'n':
		{ opt_n = ON;break;};
	    case 'l' :
		{ opt_l = ON;break;};
	    case 'r':
		{ opt_r = ON;break;};
	    }
	argc--;
	}
    if (argc < 1)
	error("SRT999 Usage: sort -{dfnlr} file1 file2 ... >outfile");

    FileSort(argc-1 , argv);
}

void FileSort(argc,argv)
int  argc;
char **argv;
{
static	char	*lineptr[LINES];
char	fname[15], outfname[15], opt_ll;
int	nlines, t, high, low, lim;
FILE	*infp[MERGEORDER], *outfp;

    low = -1;
    high = 0;
    opt_ll = opt_l;
    opt_l  = OFF;
    do {
	do {
	    if( (infp[++low] = fopen(*(argv++),READ) )== NO) {
		fprintf(STDERR, "SRT911 file not open: %s\n", *(argv - 1) );
		exit(1);
	    }
	    t = Gtext(lineptr, &nlines, LINES, infp[low]);
	    Sort(lineptr, &nlines);
	    MakeFile(low, &outfp, outfname);
	    Ptext(lineptr, nlines, outfp);
	    putc(CPMEOF,outfp);
	    if(fclose(outfp) == ERROR) {
		fprintf(STDERR, "SRT912 file close error: %s\n", outfname);
		exit(1);
	    }
	} while (t != EOF);
    } while (--argc > 0);
    high = low + 1;
    opt_l = opt_ll;
    for (low = 0; low < high; low += MERGEORDER) {
	lim = min(low + MERGEORDER - 1, high);
	Gopen(infp, low, lim);
	MakeFile(++high, &outfp, outfname);
	Merge(infp, lim-low, outfp);
	putc(CPMEOF,outfp);
	fclose(outfp);
	Gremove(infp, low, lim);
    }
    sprintf(fname, "stemp.$%d", high);
    if ((infp[high] = fopen(fname,READ)) == NO) {
	fprintf(STDERR, "SRT901 file not open: %s\n", fname);
	exit(1);
    }
    Fcopy(infp[high], STDOUT);
    fclose(infp[high]);
    unlink(fname);
}

void MakeFile(fno, fp, fname)
FILE	**fp;
int	fno;
char	*fname;
{
    sprintf(fname, "stemp.$%d", fno);
    if ((*fp = fopen(fname,WRITE)) == NO) {
	fprintf(STDERR, "SRT902 file not creat: %s\n", fname);
	exit(1);
    }
}

void Fcopy(inbuf, outbuf)
FILE *inbuf, *outbuf;
{
int  lno;
    lno = 1;
    while (fgetlin(inbuf, allocbuf, MAXLEN) > 0) {
	if( opt_l == ON )
	    fputdec(outbuf, lno++, 6);
	    fputlin(outbuf, allocbuf);
	    if (outbuf != STDOUT)
		putch(NEWLINE, outbuf);
    }
}

void Gopen(fp, low, lim)
FILE	*fp[];
int	low,lim;
{
int	i;
char	fname[15];
    for (i = 0; i < lim - low ; i++) {
	sprintf(fname, "stemp.$%d", low + i);
	if ((fp[i] = fopen(fname,READ)) == NO) {
	    fprintf(STDERR, "SRT903 file not open: %s\n", fname);
		exit(1);
	}
    }
}

void Gremove(fp, low, lim)
FILE	*fp[];
{
char	fname[15];
int	i;
    for (i = 0; i < lim - low; i++) {
	putc(CPMEOF, fp[i]);
	if(fclose(fp[i]) == ERROR) {
	    fprintf(STDERR, "SRT912 file can't close : %s\n", fp[i]);
	    exit(1);
	};
	sprintf(fname, "stemp.$%d", low + i);
	unlink(fname);
    }
}

/* merge - merge infile(1)...infile(nfiles) onto outfile */
void Merge(infile, nfiles, outfile)
FILE	*infile[], *outfile;
int	nfiles;
{
char	*lineptr[MERGEORDER], *p;
int	i, nl, inf, len;
int	strcmp(), numcmp(), swap();

	p = alloc_bp = &(allocbuf[0][0]);
	for (nl = 0, i = 0; i < nfiles; i++)
		if ((len = fgetlin(infile[i], alloc_bp, MAXLEN)) > 0) {
			*(alloc_bp + len - 1) = '\0';
			lineptr[nl++] = alloc_bp;
			alloc_bp += MAXLINE;
		}
	Sort(lineptr, &nl);
	nl--;
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
void reheap(lineptr, nl, comp, exch)
char	*lineptr[];
int	(*comp)(), (*exch)();
int	nl;
{
int	i, f;

	for (i = 1; i <= nl; i++) {
		f = (*comp)(lineptr[i-1], lineptr[i]);
		if (opt_r == ON && f >= 0)
			break;
		else if (opt_r==OFF && f <= 0)
			break;
		(*exch)(&lineptr[i-1], &lineptr[i]);
	}
}

void Sort(lineptr, nlines)
char *lineptr[];
int  *nlines;
{
int	numcmp(), swap();

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
int	*nlines, maxlines;
FILE	*fp;

{
int	len;
	*nlines = 0;
	alloc_bp = &(allocbuf[0][0]);
	alloc_ep = &allocbuf[1023][(MAXBUF-1)];
	do {
		if ((len = fgetlin(fp, alloc_bp, MAXLEN)) <= 0)
			return EOF;
		lineptr[(*nlines)++] = alloc_bp;
		alloc_bp += len;
		*(alloc_bp - 1) = NULL;
		} while (alloc_bp + MAXLEN <= alloc_ep && *nlines < maxlines);
	return(len);
}

void  Ptext(lineptr, nlines, fp)
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
void fputdec(fp, n, w)
FILE *fp;
int  n,w;
{
char	s[64];
int	len;
	len = strlen(itoa(s,n));
	if ((len = w - len) > 0)
		while (len--)
			putch(BLANK, fp);
	fputlin(fp, s);
	putch(':', fp);
}

void fputlin(fp, s)
char *s;
FILE *fp;
{
	while (*s)
		putch(*s++, fp);
}

void putch(c,fp)
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
FILE	*fp;

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
		return (int) getchar();
	else {
		if ((c = getc(fp)) == CRETURN)
			return getch(fp);
		if (c == CPMEOF || c == EOF)
			c = EOF;
		return c;
	}
}
