/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48b

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e8.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	FUNCTIONS: loc,getline,inject,delete,puttext,
			readtext,balloc,error,addhistory
	PURPOSE: get and put text lines into and out of storage
*/

#include "e.h"


loc(line,move)	/*returns line+move, adjusted to be within text*/
register int line;
register int move;
{
register int y;

	if ( (y=line+move) < 1) y=1;
	if (lastl == UNKNOWN && y > lastread) readtext(y);
	if (y > lastl) return(lastl);
	return(y);
}
/*
 * signed short as an index to tp[] will create a boundary error.
 *		11/June/1987 Y. Tsuji
 */
char *
getline(line)	/*returns address of text of line 'line' and updates
		  	  page usage*/
register int line;
{
	register int slot;

	line = loc(line,0);
	if ( (slot=pageloc[tp[(unsigned)line].page]) <= 0)
		slot=swappin(tp[(unsigned)line].page);
	if (usage[slot] >= INUSE) usage[slot]= ++clock;
	return(slotaddr[slot] + tp[(unsigned)line].moffset);
}

inject(line,txt)	/* inserts 'txt' after 'line' */
register int line;
char *txt;
{

	int p;

	addhistory(line,HISTINSERT);
	if ((char *)&(tp[(unsigned)lastread+2]) >= slotaddr[tppages]) {
		/*need another slot to store tp's in */
		if (tppages+2 >= slotsinmem) {
			error("Too many lines of text");
			return(FAIL);
		}
		if (usage[tppages] == NOTAVAIL) /*ie page is being alloc'ed into*/
			allocp= PAGESIZE+1; /*force balloc into new page*/
#ifdef DEBUG
		else if(usage[tppages] == FREE)
			putstr("\007\007");
#endif
		swapout(tppages);
		usage[tppages++]= NOTAVAIL;
	}

	/* CPM68K specific function blkmove
	 *		blkmov(to, from, bytes)
	 */

	if (++line < ++lastread)
		blkmove(&tp[(unsigned)line+1], &tp[(unsigned)line], (lastread-line)*sizeof(struct addr) );
	tp[(unsigned)line].moffset=p=balloc(1+strlen(txt));
	tp[(unsigned)line].page=newpage;
	strcpy(npaddr+ p,txt);
	if (lastl != UNKNOWN)lastl++;
	return(line);
}

delete(ffrom)	/*deletes a line by shifting pointers*/
register int ffrom;
{

	addhistory(ffrom,HISTDELETE);
	blkmove(&tp[(unsigned)ffrom], &tp[(unsigned)ffrom +1], (lastread-ffrom)*sizeof(struct addr) );
	if (lastl != UNKNOWN) lastl--;
	lastread--;
	if (lastl < 1)lastl=1;
	if (lastread < 1)lastread=1;
}

puttext()	/*replaces cline's text if it has been altered*/
{
register int p, cp;
	if (altered) {
		addhistory(cline,HISTREPLACE);
		altered=NO;
		if (!trail) {
			for(cp=strlen(text)-1;cp>=0 && isspace(text[cp]);
				cp--) text[cp]= '\0';
		}
		tp[(unsigned)cline].moffset=p=balloc(1+strlen(text));
		tp[(unsigned)cline].page=newpage;
		strcpy(npaddr+ p,text);
	}
}

readtext(line)	/*reads file being edited into virtual memory until 'line'
		  is read, or eof.  If eof, sets lastl to number of last line
		  read.  File is assumed to be already open*/
int line;
{
	char txt[LLIM];
	register char *t;
	register int i, c, l;

	storehist=NO;
	while (lastread < line) {
		for (i=0, t= &txt[0]; i < LLIM && ((c= fgetc(textfp)) >= ' ' ||
			c != '\n' && c != EOF); i++) {
			    if (c) *t++ =c;
			else i--;
		}
		*t='\0';
		if ((l=inject(lastread,txt)) == FAIL) {lastl=lastread;break;}
		else if ((lastread=l)%100 == 0) ptlineno(l);
		if (goteof= c == EOF) { fclose(textfp);lastl=lastread;
			break; }
		checkkey();
	}
	storehist=YES;
}

balloc(n)	/*allocate and return the offset in newpage of a vector size n*/
int n;
{
	register short slot;
	if (allocp  + n >= PAGESIZE) {
		/*no room in current newpage; get another*/
		if (pageloc[newpage] > 0)usage[pageloc[newpage]]=INUSE;
		pageloc[++newpage]=slot=freememslot();
		usage[slot]= NOTAVAIL;
		allocp=0;
		npaddr=slotaddr[slot];
	}
	allocp+=n;

	return(allocp-n);
}

error(message)
char *message;
{
	if (errmess) return;
	gotoxy(32,0);
	putstr(errmess=message);
	resetcursor();
	inbufp=0;
}

addhistory(l,type)
register int l;
short type;
{
	register struct histstep *step;

	if(!storehist) return;

	step= &history[histptr];
	if (++histptr == HISTLEN) histptr=0;
	if (histcnt < HISTLEN) histcnt++;

	step->histp.page=tp[(unsigned)l].page;
	step->histp.moffset=tp[(unsigned)l].moffset;
	step->histline=l;
	step->histtype=type;
	step->histcomm=ncommand;
}
