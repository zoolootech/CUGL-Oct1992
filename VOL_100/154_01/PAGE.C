/* page.c:	UNIX more clone */

/*
	Paginates text files to screen.  Wildcards and pathnames are
	processed correctly.  At the end of each screen-painting, a
	keypress of 'n' will skip to the next file to be displayed.
	The key 'q' will return to DOS.  Hitting the return key will
	scroll down one line.  Any other keypress will cause the next
	screen of the current file to be displayed.  Can also be used
	as a filter.

-----
	(c) Chuck Allison, 1985
-----

*/

#include <stdio.h>
#include <ctype.h>
#define screen_size 25
#define ESC 27
#define MAXLINE 256
#define MAXFILES 150
extern char getkey();		/* ..get keystroke through BIOS.. */


main(argc,argv)
int argc;
char *argv[];
{
	FILE *fp;
	int maxarg = MAXFILES, i, xargc, rc;
	char *xargv[MAXFILES];

	if (argc == 1)
		more(stdin,"(stdin)");
	else
	{
		/* ..expand wildcards, if any.. */
		xargc = exargs("",argc,argv,xargv,maxarg);

		for (i = 0; i < xargc; ++i)
			if ((fp = fopen(xargv[i],"r")) == NULL)
				fprintf(stderr,"can't open: %s\n",xargv[i]);
			else
			{
				rc = more(fp,xargv[i]);
				fclose(fp);
				/* ..pause between files.. */
				if (rc == EOF && i < xargc-1)
					pause();
			}
	}
}


int more(f,name)
FILE *f;
char *name;
{
	int lc, key;
	char line[MAXLINE];

	/* ..print filename.. */
	filename(name);
	lc = 2;

	while (fgets(line,MAXLINE-1,f) != NULL)
		if (lc <= screen_size-1)
		{
			/* ..continue scroll.. */
			fputs(line,stderr);
			++lc;
		}
		else if ((key = tolower(pause())) == 'n')
			/* ..stop - get next file, if any.. */
			return key;
		else if (key == '\r')
			/* ..scroll 1 line only.. */
			fputs(line,stderr);
		else
		{
			/* ..start next screen.. */
			filename(name);
			fputs(line,stderr);
			lc = 3;
		}

	return EOF;
}


filename(s)		/* ..clear screen and show current file name.. */
char *s;
{
	fprintf(stderr,"%c[2J%c[7m%-20.20s%c[0m\n",ESC,ESC,s,ESC);
}

int pause()
{
	int key;

	/* ..position cursor.. */
	fprintf(stderr,"%c[25;1H",ESC);	

	/* ..print prompt.. */
	fprintf(stderr,"%c[7mmore...%c[0m",ESC,ESC);
	fflush(stderr);

	/* ..get keypress.. */
    key = tolower(getkey());

	/* ..wipe prompt.. */
	fprintf(stderr,"\r       \r");
	fflush(stderr);

	/* ..quit if requested.. */
	if (key == 'q')
		exit(0);

    return(key);
}
