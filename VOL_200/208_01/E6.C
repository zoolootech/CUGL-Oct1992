/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48+

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e6.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	FUNCTIONS: blockpos,blockops,putpart,listfile
	PURPOSE: performs block commands
*/

#include "e.h"


blockpos(oldpos)
int oldpos;
{
	int c;

	if (oldpos) putstr("| or |P|rev.");
	do {
		ptlineno(cline);
		resetcursor();
		c = getkey();
		if(goabout(c) == -1)
			switch (c) {
			case 'p'	:
			case 'P'	:
				if (oldpos) return PREV;
				break;
			case ESCKEY	:
				return FAIL;
			default		:
				;
			}

	}
	while (c != CR);
	return(cline);
}

blockops()
{
	int oldcline, oldcharn, oldto, oldfrom, op;
	int l, ll, line, shifts, shiftx, cp, y, c;

	char  *txt, shift[LLIM];

	puttext();
	oldcline=cline;
	oldcharn=charn;
	oldfrom=from;
	oldto=to;
	putmess("W|rite to file, |P|rint, |S|hift, |M|ove, |C|opy, or |D|elete block ? ");
	while (YES) {
		switch ((op=getlow())) {
		case ESCKEY :
			return;
		case 'w':
			putstr("Write");
			break;
		case 'p':
			putstr("Print");
			break;
		case 's':
			putstr("Shift");
			break;
		case 'm':
			putstr("Move");
			break;
		case 'c':
			putstr("Copy");
			break;
		case 'd':
			putstr("Delete");
			break;
		default:
			continue;
		}
		break;
	}
	from=cline;
	to=0;
	blocking=YES;
	putmess("|Put cursor on line |end|ing block and press [return]");
	if ( (to=blockpos(oldto)) == FAIL) {
		to=cline;
		goto abort;
	}
	if (to == PREV) {
		moveline(oldfrom-cline);
		to=loc(oldto,0);
		for (l=from=cline, y=cursory, ll= (oldto < plast ? oldto : plast);
			l <= ll; l++, y++) putline(l,y);
	}
	if (to < from) {
		l=to;
		to=from;
		from=l;
	}
	switch (op) {
	case 'w':
		putmess("File to write to? ");
		if (scans(name,15) != ESCKEY)
			if (exists(name)) writefile(from,to,name,name,NO);
		break;
	case 'p':
		listfile(from,to);
		break;
	case 's':
		putmess("Delete/insert spaces/tabs| to shift line, and press [return]");
		moveline(from-cline);
		sync(0);
		resetcursor();
		for (shifts=0; (c=getkey()) != CR; ) {
			switch (c) {
			case DELRIGHT:
				if (text[0] == ' ' || text[0] == '\t')
					deletechar(0);
				break;
			case ' ':
				insertchar(' ');
				break;
			case TAB:
				insertchar('\t');
				break;
			case ESCKEY:
				goto abort;
			default:
				continue;	 /*ignore other characters*/
			}
			shift[shifts++]=c;
			sync(0);
			resetcursor();
		}
		puttext();
		for (l=from+1; l <= to; l++) {
			strcpy(text,getline(l));
			cline= l;
			for (shiftx=0; shiftx < shifts; shiftx++) {
				switch((c=shift[shiftx])) {
				case DELRIGHT:
					if (*(txt= &text[0]) == ' ' || *txt == '\t')
						while ( (*txt= *(txt+1))) txt++;
					break;
				case ' ':
				case TAB:
					if ((cp=strlen(text)) < (LLIM-1)) {
						for (; cp >= 0; cp--)
							text[cp+1]=text[cp];
						text[0]= (c == ' ' ? ' ':'\t');
					}
					break;
				}
			}
			altered=YES;
			puttext();
		}
		break;
	case 'd':
		for (l=from; l <= to; l++) delete(from);
		cline= loc(from,0);
		strcpy(text,getline(cline));
		adjustc(cursorx);
		from=to=0;
		break;
	case 'm':
	case 'c':
		putmess("|Put cursor on |line under which  block is to go |and press [return]");
		if ( (cline=line=blockpos(0)) == FAIL) {
			cline=oldcline;
			break;
		}
		for (l=from; l <= to; l++) {
			if ((line=inject(line,getline(l))) == FAIL) break;
			if (op == 'm') {
				delete( (l<line?l:l+1) );
				if (to < line) {
					to--;
					l--;
					line--;
					cline--;
				}
			}
			else {
				if (to >= line) to++;
				if (l >= line) l++;
				if (l == cline) l=line;
			}
		}
		from=cline+1;
		to=line;
		break;
	}
abort:
	blocking=NO;
	switch (op) {
	case 'w':
	case 'p':
	case 's':
		cline=oldcline;
		charn=oldcharn;
		putpart(from,to);
		break;
	case 'd':
		putpart(cline,cline+SHEIGHT);
		break;
	case 'm':
	case 'c':
		putpage();
		break;
	}
}

putpart(start,fin)
int start,fin;
{
	int l, y;

	if (start < pfirst) putpage();
	else
		for (l=start, y=topline+(start-pfirst); l <= fin && y <= SHEIGHT;
			l++, y++) {
			    if (l == cline) cursory=y;
			putline(l,y);
		}
	strcpy(text,getline(cline));
}

listfile(ffrom,tto)
int ffrom,tto;
{
	int l,cp,i;
	char *t, ch[2];
	int lst;

	puttext();
	lst= open("LST:", 1);

	for (l=ffrom; l<=tto; l++) {
		if (l%10 == 0) ptlineno(l);
		for (cp=0, t=getline(l); *t; t++)
			if (*t == '\t') for (i=tabwidth-cp%tabwidth; i>0 ; cp++, i--)
				write(lst, " ", 1);
			else {
				if (*t > CTRL) write(lst, t, 1);
				else {
					write(lst, "^", 1);
					ch[0] = *t+64, write(lst, ch, 1);
				}
				cp++;
			}
		write(lst, "\n", 1);
		if (testkey() == ESCKEY) {
			error("Listing aborted");
			return;
		}
	}
	close(lst);
}
