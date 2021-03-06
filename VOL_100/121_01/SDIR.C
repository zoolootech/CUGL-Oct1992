/*
HEADER: CUG 121.??;

    TITLE:	Sdir - sorted directory list;
    VERSION:	1.0;
    DATE:	09/01/85;
    DESCRIPTION: "This program produces a sorted directory list on the
		console, with four directory entries per line.";
    KEYWORDS:	sorted, directory;
    SYSTEM:	CP/M;
    FILENAME:	SDIR.C;
    WARNINGS:	"Copyright (c) 1982, Steve Blasingame.
		Requires fcb.h for compile & files.c for link.
		Handles a maximum of 128 files (easily changed).";
    SEE-ALSO:	ZDIR.C (another directory lister);
    AUTHORS:	Steve Blasingame;
    COMPILERS:	BDS-C 1.50;
*/

#include <bdscio.h>
#include "fcb.h"

#define	GLOBMAX	128		/* max dir entries */
#define	FILENAMESIZE	15	/* max size of entry */
char	*globv[GLOBMAX];	/* vector */

char	globs[GLOBMAX*FILENAMESIZE]; /* expansion buffer */
int	flag;			/* for lst function */
int	lst();			/* format directory listing */
int	expand();		/* expand global filenames */
int	doglob();		/* parse global file expressions */
int	strcmp();

main(argc,argv)
int argc;
char *argv[];
{
char *cp;

	argv[argc] = 0;	/* kluge */
	++argv;
	flag = 0; /* a kluge */
	if (argc > 1) {
		while (*argv != NULL && **argv != NULL) {
			expand(lst,*argv,0);
			argv++;
		}
	}
	else expand(lst,"*.*",0);

	putchar('\n');
	exit();
}

lst(vector)
char *vector;
{
int q;	/* flag must be an external */

	if (flag == 3) {
		puts(vector);
		putchar('\n');
		flag = 0;
	}
	else {
		puts(vector);
		for (q = 0; q != (20-strlen(vector)); q++)
			putchar(' ');
		flag++;
	}
	return 0;
}

expand(func,parm1,parm2) /* perform global file expansions */
int (*func)();
char *parm1;
int parm2;
{
int count, ret;

	count = GLOBMAX;

	setmem(globs,count*FILENAMESIZE,0xff);
	while (count--)
		globv[count] = &globs[count*FILENAMESIZE];

	globv[GLOBMAX]=0;
	if ((ret = doglob(parm1)) >= 1) {
		qsort(globs,ret,FILENAMESIZE,strcmp);
		count = 0;
		while (count < ret) {
			if ((*func)(globv[count],parm2) == -1)
				break;
			count++;
		}
	}
	else {
		puts(parm1);
		puts(": no match\n");
	}
	return 0;
}

doglob(string) /* glob - expand filename expressions */
char *string;
{
char f, c, buf[20];
fcb address;
fcb *dirbuf;
int flag, q, i, j;

	flag=0;
	i=0;

	if (strlen(string) == 2 && *(string+1) == ':') {
		setfcb(address,"????????.???");
		address.f_entry = tolower(*string)-96;
	}
	else setfcb(address,string);

	for (f=17; (c=bdos(f,&address)) != 255; f=18) {
		dirbuf = (0x80 + (c * 32));
		dirbuf->f_entry = address.f_entry; 
		pickout(dirbuf, buf);
		j = 0;
		do {
			globv[i][j] = tolower(buf[j]);
		} while (buf[j++]);
		if (++i == GLOBMAX)
			break;
	}
	globv[i] = NULL;	/* null terminate vector */
	return i;
}
                                                       