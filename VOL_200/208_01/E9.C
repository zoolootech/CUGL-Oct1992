/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48b

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e9.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
        FUNCTIONS: initvm,freememslot,swapout,writepage,swappin,readpage,
                        fatalerror
*/

#include "e.h"
#include <setjmp.h>

extern jmp_buf mainmenu;

extern char *mktemp();
extern long lseek();

char pagemem[MAXMEMSLOTS*PAGESIZE];

initvm()        /*initialise virtual memory system*/
{
	register int slot;
	register char *base;

	slotsinmem=MAXMEMSLOTS;
	for(slot=0, base=pagemem;slot <slotsinmem;slot++, base += PAGESIZE) {
		usage[slot]= FREE;
		slotaddr[slot]= base;
	}
	clock=0;
	/*slot 0 for tp*/
	tp = (struct addr *)pagemem;
	tppages=1;
	usage[0]= NOTAVAIL;

	/*force balloc to find a new page to start*/
	pageloc[(newpage=0)]=FREE;
	allocp=PAGESIZE+1;

	/*paging file not yet created*/
	pagefd= NOFILE;
	strcpy(pagingfile,(*pagingdir ? pagingdir:curdir) );
	strcat(pagingfile,":");
	strcat(pagingfile,mktemp("eXXXXXX"));
}

freememslot()   /*returns the number of a free memory slot, possibly by swapping
                  out the least recently used page currently in memory*/
{
	register int use, u;
	register int i, slot;

	for (use=MAXINT, i=0; use && i < slotsinmem; i++)
		if ( (u=usage[i]) != NOTAVAIL && u < use)
			use=u, slot=i;
	if (use) /*no free slots*/ swapout(slot);
	return(slot);
}

swapout(slot)   /*swaps page currently in memory at 'slot' to disk,
                  updating pageloc to show new location*/
{
	register int *pl;

	/*find page number of page at 'slot'*/
	for (pl= pageloc; *pl != slot; pl++);
	*pl = -writepage(slot);/*update pageloc with disk slot written to*/
	/*pageloc[] contains positive int when in main memory, negative
	 *when in disk
	 */
}

writepage(slot) /*writes page currently in memory at 'slot' to disk;
                  returns disk slot where written*/
{
	register int loc;
	register long sekbytes;

	if (pagefd == NOFILE) { /*haven't opened paging file yet*/
		checkkey();
		if ((pagefd=creat(pagingfile,0644)) == FAIL)
			fatalerror("Can't create a buffer file");
		for (loc=0; loc < MAXSLOTS; loc++) dskslots[loc]=FREE;
	}
	for (loc=0; dskslots[loc] != FREE; loc++); /*find a free slot*/
	sekbytes= (long)loc*(long)PAGESIZE;
	if (lseek(pagefd,sekbytes,0) == FAIL)
		fatalerror("Bad seek in writing buffer");
	checkkey();
	if (write(pagefd,slotaddr[slot],PAGESIZE) != PAGESIZE)
		fatalerror("Can't write to buffer - disk full");
	dskslots[loc]= INUSE;
	usage[slot]= FREE;
	return(loc);
}

swappin(page)   /*get 'page', currently on disk, into memory and return slot
                  where placed*/
int page;
{
	register int slot;
	readpage( (slot=freememslot()), -pageloc[page]);
	usage[slot]= INUSE;
	return(pageloc[page]=slot);
}

readpage(memslot,dskslot)       /*read a page from disk into memory at 'memslot'*/
register int memslot, dskslot;
{
register long sekbytes;
	sekbytes= (long)dskslot*(long)PAGESIZE;
	if (lseek(pagefd,sekbytes,0) == FAIL)
		fatalerror("Bad seek in reading buffer");
	checkkey();
	if (read(pagefd,slotaddr[memslot],PAGESIZE) != PAGESIZE)
		fatalerror("Can't read buffer");
	dskslots[dskslot]= FREE;
}

fatalerror(message)     /*displays message, returns to a known good screen*/
char *message;
{
	error(message);
	moveline(goodline-cline);
	longjmp(mainmenu,1);
}
