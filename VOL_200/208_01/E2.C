/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48+

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e2.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	FUNCTIONS: movechar,moveline, movepage, moveword, insertchar,
			deletechar,crdelete,deleteword,crinsert,adjustc,
			sync, pointtobrack.
	PURPOSE: perform text changing commands
*/

# include "e.h"

movechar(move)	/*move cursor by 'move' columns to the right
		  return YES unless going off text*/
int move;
{
	int cp, len, result;

	cp=charn+move;
	result=YES;
	if (cp < 0) {
		result=moveline(-1);

		if (result) {
			adjustc(LLIM);
			movechar(cp+1);
		}
		else sync(0);
	}
	else if (cp > (len=strlen(text))) {
		result=moveline(1);
		if (result) {
			sync(0);
			movechar(cp-len-1);
		}
		else adjustc(LLIM);
	}
	else sync(cp);
	return(result);
}

moveline(move)	/*move cursor by 'move' lines down
		 return YES if Ok, NO if going off text*/
register int move;
{
	register int line;

	puttext();
	if ( (move<0?-move:move) > SHEIGHT) gotoxy(WAITPOS,0);
	line=cline;
	if ((cline=loc(cline,move)) != line) {
		strcpy(text,getline(cline));
		if (cline < pfirst || cline > plast) {
			if (move == 1 || move == -1) scroll(move);
			else putpage();
		}
		else {
			if (hilight) putline(line,cursory);
			cursory+=cline-line;
			adjustc(cursorx);
			if (hilight) putline(cline,cursory);
		}
		return(YES);
	}
	else return(NO);
}

scroll(move)	/*scroll up (move==1) or down 1 line*/
int move;
{
	if (move == 1) {
		linedelete(topline);
		putline(cline-1,SHEIGHT-1);
	}
	else {
		insertline();
		putline(cline+1,topline+1);
	}
	adjustc(cursorx);
	putline(cline,cursory);
	if (plast-pfirst == (SHEIGHT-topline)) plast+=move;
	pfirst+=move;
}

jumpline(move)	/*move current line by move lines down, checking for
			interrupt from user (if interrupted, do nothing,
			and return NO) */
int move;
{
	register int line, dest;

	puttext();
	dest=cline+move;
	dest-=dest%100;
	if (dest > lastread) {
		gotoxy(WAITPOS,0);
		line=cline;
		while (line < dest && loc(line,100) != line) {
			line+=100;
			if (testkey() == ESCKEY) {
				error("Interrupted");
				return(NO);
			}
		}
	}
	moveline(move);
	return(YES);
}

movepage(dir)	/*move current line by a page down (dir==0) or up (-1)*/
int dir;
{
	int move, line;

	puttext();
	move=(SHEIGHT-topline)/2 - PAGEOVERLAP;
	if (dir) move=pfirst-cline-move;
	else move=plast-cline+move;
	line = cline;
	if ( (cline=loc(cline,move)) != line) {
		strcpy(text,getline(cline));
		putpage();
	}
}

moveword(move)	/*move 1 word to the right (move -ve: left)*/
int move;
{
	if (charn+move < 0) {
		moveline(-1);
		charn=strlen(text);
	}
	else if (charn+move >= strlen(text)) {
		moveline(1);
		sync(0);
		if (inword(text[0])) return;
	}

	while ((move<0 || text[charn]) && inword(text[charn]) && (charn+=move));
	while ((move<0 || text[charn]) && !inword(text[charn]) && (charn+=move));
	if (move < 0 && charn) {
		while(inword(text[charn]) && --charn);
		if (charn || !inword(text[charn]) ) charn++;
	}
	sync(charn);
}

insertchar(c)	/*inserts 'c' at charn, moves cursor up  one */
char c;
{
	int cp;

	if ( (cp=strlen(text)+1) >= (LLIM-1))
		error("Line too long");
	else {
		for (; cp >= charn; cp--) text[cp+1]=text[cp];
		text[charn]=c;
		altered=YES;
		rewrite(charn,cursorx);
		if (c == ')' || c == '}' || c == ']') pointtobrack(c);
		sync(charn+1);
	}
}

pointtobrack(closebr)	/*momentarily put cursor on matching opening
			bracket */
char closebr;
{
	int chn, chna, i, l, brackline, brackcount, cx, openbr, c;
	char *t;

	if (closebr == ')') openbr='(';
	else if (closebr == '}') openbr='{';
	else openbr='[';

	for (chn=charn, t=text, l=cline, brackcount=0, brackline=cursory;
		l >= pfirst; brackline--) {
		    for (; chn >= 0; chn--) {
			if ((c= *(t+chn)) == closebr) brackcount++;
			if (c == openbr) brackcount--;
			if (brackcount == 0) {
				for (chna=0, cx=0; chna < chn; chna++, cx++)
					if (*(t+chna) == '\t') cx+= tabwidth-1-(cx%tabwidth);
				if (l == cline || blockscroll) cx+= (lastoff>0)-lastoff;
				if (cx < 0|| cx >= SWIDTH) return;
				gotoxy(cx,brackline);
				for (i=0; i < (8 * CURSORWAIT); i++);
				return;
			}
		}
		chn=strlen( (t=getline(--l) ))-1;
	}
}

deletechar(dir)	/*deletes char before (dir= -1) or at (dir=0) cursor */
{
	register int cp;

	cp=charn+dir;
	if (cp < 0) {
		if (cline > 1) crdelete(-1);
	}
	else if (text[cp] == '\0') {
		if (cline < lastl) crdelete(0);
	}
	else {
		while(text[cp]=text[cp+1])
			cp++;
		altered=YES;
		sync(charn+dir);
		if (calcoffset() != lastoff)rewrite(0,0);
		else rewrite(charn,cursorx);
	}
}

crdelete(dir)	/*delete a [CR] before (dir== -1)or at (dir==0) cursor */
int dir;
{
	int delline, len;
	char textb[LLIM];

	altered=YES;
	if (dir == 0) {
		delline=cline+1;
		strcpy(textb,getline(delline));
		cursory++;
	}
	else {
		delline=cline;
		strcpy(textb,text);
		if (cline > 1)
			strcpy(text,getline(--cline));
		else puttext();
	}
	sync(len=strlen(text));
	if (len+strlen(textb) >= LLIM) {
		textb[LLIM-len]='\0';
		error("Line too long - cut short");
	}
	strcat(text,textb);
	puttext();
	delete(delline);
	if (delline > plast || delline <= pfirst) putpage();
	else {
		linedelete(cursory--);
		rewrite(0,0);
		if (plast <= lastl && lastl-pfirst > SHEIGHT - topline)
			putline(plast, SHEIGHT);
		else plast=lastl;
	}
}

deleteword()	/*delete rest of word if in word, else up to next word */
{
	int pend, cp, in, c;

	for (in=inword(text[pend=charn]);
		(c=text[pend]) && (in ? inword(c): !inword(c)); pend++);
	    for (cp=charn; (text[cp]=text[pend]); pend++, cp++);
	rewrite(charn,cursorx);
	altered=YES;
}

crinsert(dir)   /*insert a [CR] behind (dir==0) or in front of (-1) cursor*/
{
	char textb[LLIM], c;
	int charnb = 0;

	if (autoin && !isspace(text[charn]))
		while(isspace(c=text[charnb]) ) textb[charnb++]=c;
	strcpy(&textb[charnb],&text[charn]);
	text[charn]='\0';
	altered=YES;
	/* begin Haefner code */
	if (dir == 0) puttext();
	if ((cline=inject(cline,textb)+dir) == FAIL) return;
	if (dir == 0) {
		strcpy(text,getline(cline));
		sync(charnb);
	}
	/* end Haefner code */
	/*
		if (dir == 0) {
			puttext();
			strcpy(text,textb);
			altered=YES;
			sync(charnb);
			if ((cline=inject(cline,textb)) == FAIL) return;
			}
		else if (inject(cline,textb) == FAIL) return;
	*/
	if (cursory >= SHEIGHT) {
		puttext();
		putpage();
	}
	else {
		/* haefner code */
		gotoxy(0,cursory+1);
		/* haefner code */
		insertline();
		if (dir == 0) putline(cline-1,cursory++);
		else putline(cline+1,cursory+1);
		if (plast-pfirst < SHEIGHT-topline)plast++;
		rewrite(0,0);
		/*
		                rewrite(0,0);
		                if (plast-pfirst < SHEIGHT-topline)plast++;
		*/
	}
}

undo()	/*undoes edits on current line and then from history*/
{

	struct histstep *step;
	short onpage;
	int l, slot;
	char textb[LLIM];

	if (altered) {
		if (cline <= lastl) strcpy(text,getline(cline));
		else text[0]='\0';
		altered=NO;
		adjustc(cursorx);
		rewrite(0,0);
	}
	else {
		if (histcnt == 0) {
			error("Nothing to undo");
			return;
		}
		onpage=NO;
		do {
			if (--histptr < 0) histptr= (HISTLEN-1);
			histcnt--;
			step= &history[histptr];
			l=step->histline;
			onpage= onpage || (l >= pfirst && l <= plast);
			storehist=NO;
			switch (step->histtype) {
			case HISTINSERT:
				delete(l+1);
				break;
			case HISTDELETE:
				if ((slot=pageloc[step->histp.page]) <= 0)
					slot=swappin(step->histp.page);
				strcpy(textb,slotaddr[slot] + step->histp.moffset);
				inject(l-1,textb);
				break;
			case HISTREPLACE:
				tp[l].page=step->histp.page;
				tp[l].moffset=step->histp.moffset;
				break;
			}
		}
		while (histcnt && step->histcomm ==
		    history[(histptr-1 < 0 ? HISTLEN-1 : histptr-1)].histcomm);
		if (onpage) putpage();
	}
}

adjustc(x)	/*set cursorx to col. cursor nearest to col. 'x' so that
		  cursor isn't in the middle of a tab or off the end of the
		  current line*/
{
	int c;

	for(charn=0, cursorx=0; cursorx < x && (c=text[charn]);
						charn++, cursorx++)
		    if (c == '\t') cursorx+=tabwidth-1-(cursorx%tabwidth);
}

sync(cp)	/*put cursorx and charn onto character 'cp' of current line*/
int cp;
{
	for(charn=0, cursorx=0; charn < cp; cursorx++, charn++)
		if (text[charn] == '\t') cursorx=cursorx+tabwidth-1-(cursorx%tabwidth);
}



