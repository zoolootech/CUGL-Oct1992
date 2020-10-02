/* [compare.c of JUGPDS Vol.46] */
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

/* compare - file comarison */

void	main(argc, argv)
int	argc;
char	*argv[];

{
char	inbuf1[MAXLINE], inbuf2[MAXLINE];
FILE	*fd1,*fd2;
void	compare();

	if (argc < 3)
		error("CMP901 Usage: compare file1 file2\n");
	else if ( (fd1 = fopen((argv[1]),"r") ) == NO)
		error("CMP902 file1 open error!\n");
	else if ( (fd2 = fopen(argv[2],"r") ) == NO)
		error("CMP903 file2 open error!\n");
	else
		compare(inbuf1,inbuf2,fd1,fd2);
	fclose(fd1);
	fclose(fd2);
}


/* compare - compare two files for equality */

void	compare(inbuf1,inbuf2,fd1,fd2)
char	*inbuf1, *inbuf2;
FILE	*fd1,*fd2;

{
char *lineptr[MAXLINE], line[MAXLINE];
int  nlines, i, j, k, lineno, len;
int	readlines(),equal();

	j = 0;
	lineno = 0;
	if ((nlines = readlines(lineptr,MAXLINE, inbuf2,fd2)) > 0) {
		while ((len = fgetlin(fd1,line, MAXLINE)) > 0) {
			for (i = j; i < nlines; i++)
				if (equal(line, lineptr[i]) == YES)
					break;
			if (i == nlines)
				printf("CMP101 File1 %4d:%s\n", lineno, line);
			else {
				for (k = j; k < i; k++) {
					printf("		");
					printf("CMP102 File2 %4d:",k);
					printf("%s\n",lineptr[k]);
					}
				j = i + 1;
				}
			lineno++;
			}
		for (k = j; k < i; k++) {
			printf("		");
			printf("CMP103 File2 %4d:%s", k, lineptr[k]);
			}
		}
}


int	readlines(lineptr, maxlines, inbuf,fd)
char	*lineptr[], *inbuf;
FILE	*fd;
int	maxlines;

{
int	len, nlines;
char	*p, line[MAXLINE], *malloc();

	nlines = 0;
	while ((len = fgetlin(fd,line, MAXLINE)) > 0) {
		if (nlines >= maxlines)
			return(-1);
		else if ((p = malloc(len+1)) == NULL) {
			printf("CMP911 allocation over.  p:%04x\n", p);
			return(-1);
			}
		else {
			strcpy(p, line);
			lineptr[nlines++] = p;
			}
	}
	return(nlines);
}

int	equal(s1, s2)
char 	*s1, *s2;

{
	do {
		while (isspace(*s1)) s1++;
		while (isspace(*s2)) s2++;
		if (*s1 == *s2 && *s1 == EOS)
			return(YES);
		} while (*s1++ == *s2++);
	return(NO);
}
