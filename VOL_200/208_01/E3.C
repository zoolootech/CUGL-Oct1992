/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48+

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e3.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	FUNCTIONS: putline,putoffset,putstatusline,ptlineno,,putpage,
			putmess,unmess,putstr,rewrite,calcoffset,resetcursor
	PURPOSE: write text to screen
*/

# include "e.h"

putline(line,y)
int line, y;
{
	register char *p;
	register int cp, lastcol, off, x, nblank;
	int	bright;

	deleteline(0,y);
	if (line <= lastl) {
		bright= !hilight || line == cline;
		if (blocking && !bright)
			bright = to ? (line >= from && line <= to):
				(line >= (from > cline ? cline : from) &&
				    line <= (from > cline ? from  : cline));
		lastcol=SWIDTH;
		off=x=0;
		p=getline(line);
		if ( (line == cline && calcoffset()) || (blockscroll && offset) ) {
			lastcol+=offset-1;
			off=offset;
			x=1;
			standout();
			putch('<');
			standend();
		}

		if (!bright) standout(), isdim = YES;

		for (cp=nblank=0; *p && cp < lastcol; p++) {
			if (*p == '\t')
			do {
				if (cp >= off && cp < lastcol)
					nblank++;
			}
			while (++cp % tabwidth);
			else {
				if (cp++ >= off) {
					if (*p == ' ') nblank++;
					else {
						if (nblank) {
							x+=nblank;
							if (nblank < GOTOLEN)
								for (; nblank; nblank--)
									putch(' ');
							else {
								gotoxy(x,y);
								nblank=0;
							}
						}
						dispch(*p);
						x++;
					}
				}
			}
		}
		if (*p && y < SHEIGHT) {
			makeother();
			gotoxy(SWIDTH,y);
			putch('>');
		}
		standend();
		isdim = NO;
	}
}

putoffset()
{
	register int i;

	gotoxy(0,0);
	if (offset) for(i=3-uspr(offset); i>0; i--) putch(' ');
	else putstr("   ");
	gotoxy(0,cursory);
}

putstatusline(line)
int line;
{
	deleteline(FNPOS,0);
	standout();
	putstr(filename);
	ptlineno(line);
	standend(); isdim = NO;
	if (errmess) {
		gotoxy(EMPOS,0);
		putstr(errmess);
	}
}

ptlineno(line)
int line;
{
	int i;

	if (!displaypos) return;
	gotoxy(LNPOS,0);
	standout();
	i=uspr(line);
	putch(':');
	for (i=8-i-uspr(cursorx+1); i > 0; i--) putch(' ');
	standend();
	isdim = NO;
}

putpage()	/*display page more or less centered about 'cline'*/
{
	register int y,line;

	pfirst= loc(cline,(topline-SHEIGHT)/2);
	plast = loc(pfirst,SHEIGHT-topline);
	putstatusline(cline);
	for (line=pfirst, y=topline; line <= plast; line++, y++) {
		if (cline == line) {
			cursory = y;
			adjustc(cursorx);
		}
		putline(line,  y);
	}
	if (y <= SHEIGHT) delpage(y);
}

putmess(message)
char *message;
{
	register int lines;

	if (blankedmess && topline > blankedmess) {
		gotoxy(3,blankedmess++);
		putstr(message);
		return;
	}

	lines = (topline == (helpon ? HELPLINES : 1)) ? 2:1;
	if (cursory < (topline+=lines)) {
		gotoxy(0,cursory);
		for ( ; cursory < topline; cursory++) {
			insertline();
			if (plast != lastl) plast--;
		}
	}
	else pfirst+= lines;
	deleteline(0,topline-2);
	deleteline(0,topline-1);
	gotoxy(3,topline-2);
	putstr(message);
}

unmess()
{
	register int l, i, newtop, diff;

	newtop= (helpon ? HELPLINES : 1);
	if (!(diff=topline-newtop)) return;
	if (diff < PAGEOVERLAP && pfirst-diff < 1) {
		if (blankedmess == newtop) return;
		for (l=newtop; l < topline; l++) deleteline(0,l);
		blankedmess=newtop;
		return;
	}
	blankedmess=NO;
	if ((pfirst-=diff) < 1) {
		topline=newtop;
		putpage();
	}
	else {
		for (l=newtop, i=0; l < topline; l++, i++) putline(pfirst+i,l);
		topline=newtop;
	}
}

putstr(s)		/*as usual, but turns text bright or dim as required*/
char *s;
{
	register int dim, c;

	dim=NO;
	while (c= *s++)
		switch(c) {
		case BRIDIM :
			(dim=!dim) ? standout() : standend();
			break;
			/*		case '\n':
						putch('\r');
			 */
		default  :
			putch(c);
		}
	if (dim) standend();
	isdim = NO;
}

rewrite(cp,x)	/*rewrites current line from char 'cp', col 'x', onwards*/
register int cp, x;
{
	register int i, begmark, c;

	begmark= (calcoffset() > 0);
	i= x-offset+begmark;
	deleteline((i>0 ? i : 0),cursory);
	if (!x && begmark) {
		standout();
		putch('<');
		standend();
		isdim = NO;
	}
	while (x < SWIDTH+offset-begmark && (c=text[cp++])) {
		if (c == '\t') {
			for (i=tabwidth-x%tabwidth; i>0 && x<SWIDTH+offset-begmark; x++, i--)
				if (x >= offset) putch(' ');
		}
		else if (x++ >= offset) dispch(c);
	}
	if (c && cursory < SHEIGHT) {
		standout();
		putch('>');
		standend();
		isdim = NO;
	}
}

calcoffset()		/*calculate horizontal screen offset required for
			cursor to be on screen*/
{
	for (offset=(!blockscroll || cursorx < lastoff ? 0 : lastoff);
		cursorx >= SWIDTH+offset-(offset>0); offset+=OFFWIDTH);
	    return(offset);
}

resetcursor()		/*put cursor on current character, coping with
			horizontal scroll */
{
	register int line, y;

	if (lastoff != calcoffset()) {
		if (blockscroll) {
			for (line=pfirst, y=topline; line <= plast; line++, y++)
				if (line != cline) putline(line,y);
		}
		putoffset();
		rewrite(0,0);
		lastoff=offset;
	}
	gotoxy(cursorx-offset+(offset>0),cursory);
}
