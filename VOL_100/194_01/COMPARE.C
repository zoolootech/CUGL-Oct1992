/* [COMPARE.C of JUGPDS Vol.17]
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

/* compare - file comarison */

#include "stdio.h"
#include "def.h"

#define LINES 4000

main(argc, argv)
int	argc;
char	*argv[];

{
	char	inbuf1[BUFSIZ], inbuf2[BUFSIZ];
	int	fd1, fd2;

	if (argc < 3) {
		fprintf(STDERR, "Usage: cmp file1 file2\n");
		exit();
		}
	else if (fopen(argv[1], inbuf1) == ERROR) {
		fprintf(STDERR, "file1 open error!\n");
		exit();
		}
	else if (fopen(argv[2], inbuf2) == ERROR) {
		fprintf(STDERR, "file2 open error!\n");
		exit();
		}
	else
		compare(inbuf1, inbuf2);
	fclose(inbuf1);
	fclose(inbuf2);
}


/* compare - compare two files for equality */

compare(inbuf1, inbuf2)
char	*inbuf1, *inbuf2;

{
	char *lineptr[LINES], line[MAXLINE];
	int  nlines, i, j, k, lineno, len;

	_allocp = NULL;
	j = 0;
	lineno = 0;
	if ((nlines = readlines(lineptr, LINES, inbuf2)) > 0) {
		while ((len = getlin(line, MAXLINE, inbuf1)) > 0) {
			for (i = j; i < nlines; i++)
				if (equal(line, lineptr[i]) == YES)
					break;
			if (i == nlines)
				printf("File1 %4d:%s\n", lineno, line);
			else {
				for (k = j; k < i; k++) {
					printf("		");
					printf("File2 %4d:%s\n",k,lineptr[k]);
					}
				j = i + 1;
				}
			lineno++;
			}
		for (k = j; k < i; k++) {
			printf("		");
			printf("File2 %4d:%s", k, lineptr[k]);
			}
		}
}


readlines(lineptr, maxlines, inbuf)
char	*lineptr[], *inbuf;
int	maxlines;

{
	int	len, nlines;
	char	*p, line[MAXLINE], *alloc();

	nlines = 0;
	while ((len = getlin(line, MAXLINE,inbuf)) > 0) {
		if (nlines >= maxlines)
			return(-1);
		else if ((p = alloc(len)) == NULL) {
			fprintf(STDERR ,"allocation over.  p:%04x\n", p);
			return(-1);
			}
		else {
			line[len-1] = '\0';
			strcpy(p, line);
			lineptr[nlines++] = p;
			}
	}
	return(nlines);
}


getlin(s, lim, iobuf)
char	*s, *iobuf;
int	lim;

{
	char	*p;
	int	c;

	p= s;
	while (--lim > 0 && (c = getc(iobuf))!= CPMEOF && c != NEWLINE)
		*s++ = c;
	if (c == NEWLINE)
		*s++ = c;
	*s = EOS;
	return(s-p);
}

equal(s1, s2)
char 	*s1, *s2;

{
	int	i;

	do {
		while (isspace(*s1)) s1++;
		while (isspace(*s2)) s2++;
		if (*s1 == *s2 && *s1 == EOS)
			return(YES);
		} while (*s1++ == *s2++);
	return(NO);
}
