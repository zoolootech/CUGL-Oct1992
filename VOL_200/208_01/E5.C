/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48b

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e5.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	FUNCTIONS: readfile, writefile, exists, scans, retag
	PURPOSE: read and write files
*/

#include "e.h"

FILE *fp;

readfile(nname)	/* read file 'nname' and insert it after current line*/
char *nname;
{
	register int c, i;
	register int line;

	if ((fp= fopen(nname,"r")) == 0) {
		error("Can't find file");
		nname[0]='\0';
		return(FAIL);
	}
	puttext();
	line=cline;
	do {
		for (i=0; i < LLIM && (c= fgetc(fp)) != EOF && c != '\n'; )
			text[i++]=c;
		text[i]='\0';
		if (line%20 == 0) ptlineno(line);
	} while (inject(line++,text) != FAIL && c != EOF);
	fclose(fp);
	strcpy(text,getline(cline));
	return(0);
}

writefile(ffrom,tto,nname,nametoprint,exiting)
int ffrom, tto;
int exiting;
char *nname, *nametoprint;
{
	register int l;
	long copybytestart;
	bool copying;
	register char *t;
	register int c, d;
	extern long ftell();


	puttext();
	if (nname[0] <= ' ') {
		error("Bad name");
		return(FAIL);
	}
	if ((fp= fopen(nname,"w")) == 0) {
		error("cannot write file");
		return(FAIL);
	}
	putmess("Saving ");
	putstr(nametoprint);
	copying=NO;
	if (exiting) tto=lastread;
	else if (tto == lastl) tto=loc(lastl,0);
	for (l=ffrom; l <= tto; ) {
		t=getline(l++);
		/* if(fputs(getline(l++),fp)== EOF) goto diskfull;
		 * is not feasible because it could return EOF
		 * when the line is empty. -- a bug in V7 C.
		 */
		while (*t) if (fputc(*t++,fp) == EOF) goto diskfull;
		if (fputc('\n',fp) == EOF) goto diskfull;
	}
	if (exiting && (lastl == UNKNOWN|| !goteof)) {
		copying=YES;
		copybytestart= ftell(textfp);
		d = '\n';
		while ( (c= fgetc(textfp)) != EOF)
			if ((d = fputc(c,fp)) == EOF) goto diskfull;
		if (d != '\n')
			if (fputc('\n',fp) == EOF) goto diskfull;
	}
	if (fclose(fp) == EOF) { /*does flushing as well*/
		error("Can't close file");
		goto reposition;
	}
	if (copying) fclose(textfp);
	return(YES);
diskfull:
	error("Disk full");
reposition:
	if (copying) {
		fclose(fp);
		unlink(nname);
		fseek(textfp,copybytestart,0);
	}
	return(FAIL);
}

exists(nname)
char *nname;
{
	char c;

	c='y';
	if (checkexists(nname)) {
		putmess("OK to replace ");
		putstr(nname);
		putstr(" ? ");
		putch((c=getlow()));
		putret();
	}
	return(c == 'y');
}

checkexists(nname)	/*return YES if file 'nname' exists, else NO */
char *nname;
{
	return ((access(nname,4)== 0)? YES : NO);
}

scans(answer,maxlen)
char *answer;
int maxlen;
{
	int c, n;

	maxlen--;
	n=0;
	while (n < maxlen) {
		switch((c=getscankey())) {
		case LEFTKEY	:
		case DELLEFT	:
			if (n) {
				putch(BACKSP);
				putch(' ');
				putch(BACKSP);
				n--;
				answer--;
			}
			break;
		case CR		:
		case ESCKEY	:
			n=maxlen;
			break;
		case RETRIEVE	:
			if (n == 0) {

				while (*answer) {
					dispch(*answer++);
					n++;
				}
				break;
			}
		default		:
			dispch(c);
			*answer++ =c;
			n++;
			break;
		}
	}
	*answer='\0';
	putret();
	return (c & ~PARBIT) ;
}

retag(nname,tag)	/*puts a new suffix on a file name*/
register char *nname, *tag;
{
	for (; *nname && *nname != '.'; nname++);
	if (!*nname)*nname='.';
	for (++nname; (*nname= *tag); nname++, tag++);
}



