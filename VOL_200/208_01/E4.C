/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48+

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e4.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	FUNCTIONS: findorrep,dofindrep,find
	PURPOSE: perform find, alter and repeat commands
*/

# include "e.h"

findorrep()
{
	int  toend, global, i, count;
	char c, *p;

	putmess("Find? ");
	global=nocheck=toend=nocase=NO;
	findir=1;
	count=0;
	c=scans(patt,FLIM);
	if (replace) {
		if (c == ESCKEY) return;
		putmess("Alter to? ");

		c=scans(changeto,FLIM);
	}
	else if (!patt[0]) return;
	if (c == CR) {
		if (replace)
			putmess("B|ackwards/|G|lobally/|T|o end (start)/|I|gnore case/|W|ithout asking/number|? ");
		else putmess("B|ackwards/|I|gnore case/number of times|? ");
		if (scans(opts,5) == ESCKEY) return;
		for (i=0; (c=opts[i]); i++) {
			switch(tolower(c)) {
			case 'g' :
				global=YES;
			case 't' :
				toend=YES;
				break;
			case 'b' :
				findir= -1;
				break;
			case 'w' :
				nocheck=YES;
				break;
			case 'i' :
				nocase=YES;
				for (p=patt; *p; p++)
					*p=tolower(*p);
				break;
			}
			if (c >= '0' && c <= '9') count=count*10+c-'0';
		}
		if (!replace) {
			global=NO;
			toend=NO;
		}
		if (count == 0) {
			if (toend) count=MAXINT;
			else count=1;
		}
		if (global) {
			findir=1;
			moveline(1-cline);
			sync(0);
		}
	}
	else count=1;
	dofindrep(count);
}

dofindrep(count)
int count;
{
	int cp, i, len;
	char c;

	puttext();
	do {
		count--;
		if (find() == FAIL) count=0;
		else if (replace) {
			if (nocheck) c='y';
			else {
				gotoxy(EMPOS,0);
				putstr("     Replace |{|Y|/|N|}|? ");
				do {
					gotoxy(REPPOS,0);
					for (i=0; i < 10*CURSORWAIT; i++);
					resetcursor();
					for (i=0; i < 10*CURSORWAIT; i++);
				}
				while ((c=testlow()) != 'y' && c != 'n'
				    && c != ESCKEY);
			}
			deleteline(EMPOS,0);
			switch(c) {
			case 'y' :
				if (strlen(text)+(len=strlen(patt)) >= LLIM) {
					error("Line would be too long");
					return;
				}
				for (cp=charn; (text[cp]=text[cp+len]); cp++);
				for (cp=strlen(text), len=strlen(changeto);
					cp >= charn; cp--)
					    text[cp+len]=text[cp];
				for (i=0; (c=changeto[i]); i++) text[charn++]=c;
				altered=YES;
				puttext();
				rewrite(++cp,cursorx);
				sync(charn);
				break;
			case ESCKEY:
				count=0;
				error("Search stopped");
			case 'n' :
				movechar(findir);
				break;
			}
		}
	}
	while(count);
	inbufp=0;
}

/*find 'patt', searching back (findir== -1) or forwards (1) from
 *	  current line.  Return FAIL or YES, and set current line to
 *	  that containing pattern
 */
find()
{
	int fline, oldcharn, newcharn, interupt;
	register int linecount, pos;
	register char *s, pattch1, *p, *t;
	char lcline[LLIM];

	if (!replace || repeat) movechar(findir);
	fline=cline;
	oldcharn=charn;
	interupt=NO;
	linecount= cline%100;
	pattch1=patt[0];
	gotoxy(WAITPOS,0);
	if (findir == 1)
		while (fline <= lastl) {
			if (linecount++ == 100) {
				linecount=1;
				ptlineno(fline);
				gotoxy(WAITPOS,0);
				if (testkey() == ESCKEY) {
					interupt=YES;
					goto interrupted;
				}
			}
			s=getline(fline)+charn;
			if (nocase) {
				for (t=lcline; (*t=tolower(*s & NOPARITY));
							s++, t++);
				    s=lcline;
			}
			if ( (pos=indexx(s,patt)) != FAIL) {
				charn+=pos;
				goto foundit;
			}
			fline++;
			charn=0;
		}
	else
		while (fline >= 1) {
			if (linecount-- == 0) {
				linecount=99;
				ptlineno(fline);
				gotoxy(WAITPOS,0);
				if (testkey() == ESCKEY) {
					interupt=YES;
					goto interrupted;
				}
			}
			s=getline(fline);
			if (nocase) {
				for (t=lcline; (*t=tolower(*s)); s++, t++);
				s=lcline;
			}
			for (; charn >= 0; charn--)
				if (*(p= &s[charn]) == pattch1) {
					for (t=patt+1,p++; *t && *p == *t; p++, t++);
					if (!*t) goto foundit;
				}
			charn=strlen(getline(--fline))-1;
		}
interrupted:
	charn=oldcharn;
	if (!replace || repeat) movechar(-findir);
	if (interupt) {
		error("Search aborted");
		ptlineno(cline);
	}
	else error("       Search fails");
	return FAIL;

foundit:
	newcharn=charn;
	moveline(fline-cline);
	sync(charn=newcharn);
	return YES;
}

static
indexx(str,substr)
char *str, *substr;
/*
 * Returns index of substr in str, or -1 if not found.
 */
{
	register char *p, *q, *s;
	register int c;
	c = *substr;
	p = str -1;

	for(;;){
		do {
			if( *++p == 0)	/* end of str?	*/
				return (-1);	/* not found.	*/
		}
		while (c != *p);

		q = p;		/* first char matched	*/
		s = substr;
		do {
			q++;
			if(*++s == 0)	/* end of substr?	*/
				return (int)(p - str);
		}
		while (*s == *q);	/* current char match?	*/
	}
}
