/*
>>:yam7.c 9-8-81
 * File open and close stuff
 * This file assumes operation on a CP/M system
 */
#include "yam.h"


struct fcb {	/* CP/M Version 2 fcb AS SEEN BY THE USER */
	char	dr;		/* drive number */
	char	fname[8];	/* fname[1] used by TAG2 */
	char	ftype[3];	/* corresponds to extension in rt-11 */
	char	ex;		/* file extent normally 0 */
	char s1;		/* reserved for bdos's benefit */
	char s2;		/* likewise, =0 on call to open,make,search */
	char	rc;		/* record count for extent[ex]  0...128 */
	char dmap[16];
	char cr;		/* current record, initialized to 0 by usr */
	unsigned recn;		/* highest record number */
	char recovf;		/* overflow of above */
};

#define CONST 2			/* bios console char ready */
#define CONIN 3			/* bios cons char input */
#define CONOUT 4		/* bios cons char output */
#define SRCH 17 		/*bdos search for file pattern*/
#define SRCHNXT 18		/* search for next occurrence */
#define SETDMA 26		/* set address for read, write, etc. */
#define SETATTRIB 30		/* update file attributes */
#define SETGETUSER 32		/* set or get user number */
#define COMPFILSIZ 35		/* compute file size into recn and recovf */
#define UFNSIZE 15		/* a:foobar12.urk\0 is 15 chars */

openrx(name)
char *name;
{

#ifdef RESTRICTED
	char *s;
	if(s=cisubstr(name, ".com"))	/* upload .com files as .obj */
		strcpy(s, ".OBJ");
	if(cisubstr(name, "$$$"))
		return ERROR;		/* don't allow upload of $$$.sub */
#endif
#ifdef LOGFILE
	logfile(name, 'r');		/* record file xmsn */
#endif
	unspace(name);
	lprintf("'%s' ", name);		/* show the name right away */
	if(!Creamfile && fopen(name, fout) != ERROR) {
		fclose(fout);
		printf("Exists ", name);
#ifdef XMODEM
		return ERROR;
#else
		printf("Replace it (y/n)??");
		if(tolower(getchar())!= 'y')
			return ERROR;
#endif
	}
	if(fcreat(name, fout)==ERROR){
		printf("Can't create %s\n", name);
		return ERROR;
	}
	Rfile= TRUE;
	strcpy(Rname, name);
	Dumping= !Squelch;
	lprintf("Created%s\n", Dumping? "" : " recording OFF");
	return OK;
}

closerx(pad)
{
	if(Rfile) {
		if(!Quiet)
			lpstat("Closing %s", Rname);
#ifdef BDSC
		if(pad)
			do
				putc(CPMEOF, fout);
				while(fout._nleft % SECSIZ);
#endif
		fflush(fout);
		fclose(fout);
		Rfile=FALSE;
	}
}

opentx(name)
char *name;
{
	struct fcb *fp, *fcbaddr();
	lprintf("'%s' ", name);
#ifdef LOGFILE
	logfile(name, 's');		/* record file xmsn */
#endif
	unspace(name);
	if(fopen(name, fin)==ERROR){
		printf("Can't open %s\n", name);
		return ERROR;
	}
#ifdef RESTRICTED
	if(cisubstr(name, ".bad")
	 || (fp=fcbaddr(fin._fd))==ERROR
	 || (fp->fname[1] & 0200)
	 ) {
	 	fclose(fin); printf("\n'%s' Not for Distribution\n", name);
		return ERROR;
	}
#endif
	Tfile= TRUE;
	strcpy(Tname, name);
	lprintf("Open\n");
	return OK;
}

closetx()
{
	if(Tfile) {
		fclose(fin);
		if(!Quiet)
			lpstat("%s closed", Tname);
		Tfile=FALSE;
	}
}
/* search the phone file for name */
getphone(name, buffer)
char *name, *buffer;
{
	closetx();

	if(fopen(PHONES, fin)==ERROR) {
		printf("Cannot open %s\n", PHONES);
		return ERROR;
	} else {
		while(fgets(buffer, fin))
			if(cmdeq(buffer, name)) {
				fclose(fin);
				return OK;
			}
	}
	printf("Can't find data for %s\n", name);
	fclose(fin);
	return ERROR;
}

/* channge default disk and optionally, user number */
chdir(p)
char *p;
{
	unsigned newuser;

	newuser=user; *p=toupper(*p);
	if(index(*p, DISKS)) {
		defdisk= *p - 'A';
		bdos(14, defdisk);
#ifdef CDOS
		return;
#else
		if(!isdigit(p[1]))
			return;
		if((newuser=atoi(p+1)) <= MAXUSER) {
			bdos(SETGETUSER, newuser);
			user=newuser;
			return;
		}
#endif
	}
	printf("Disk %c and/or User %d Illegal\n", *p, newuser);
}

/* fetch default disk and user number */
initdd()
{
	Secpblk=SECPBLK;
	defdisk= bdos(25,0);
#ifdef CDOS
	user=0;
#else
	user=bdos(SETGETUSER, 0377);
#endif
}

/*
 * Z19 gets to use it's 25th line. pstat starts at 48th char
 * note that a call to lpstat will erase what pstat displays
 */
/*VARARGS*/
pstat(a,b,c)
char *a, *b, *c;
{
#ifdef Z19
	lprintf("\033x1\033j\033Y8P");
#endif
	lprintf(a,b,c);
#ifdef Z19
	lprintf("\033K\033k");
#else
	lprintf("\n");
#endif
}

/*
 * Z19 gets to use it's 25th line. lpstat starts at col 1
 * Rest of line is erased
 */
/*VARARGS*/
lpstat(a,b,c)
char *a, *b, *c;
{
#ifdef Z19
	lprintf("\033x1\033j\033Y8 ");
#endif
	lprintf(a,b,c);
#ifdef Z19
	lprintf("\033K\033k");
#else
	lprintf("\n");
#endif
}
char getcty()
{
	return bios(CONIN,0);
}
char putcty(c)
char c;
{
	bios(CONOUT, c);
	if(bios(CONST,0)) {
		if((c=bios(CONIN,0))==XOFF)
			bios(CONIN,0);
		else
			return c;
	}
	return FALSE;
}

dolist(argc, argp)
char **argp;
{
	int listfile();

#ifdef XMODEM

	printf("^S pauses, ^K skips to next file, ^X terminates\n");

#endif

	expand(listfile, argc, argp);
}

listfile(name)
char *name;
{
	int c;

#ifdef XMODEM
	printf("\nListing '%s'\n\022", name);
#endif
	closetx();
	if(opentx(name)==ERROR)
		return ERROR;
	else {
		while((c=getc(fin))!=EOF && c != CPMEOF) {
			if( !(c=putcty(c)))
				continue;
			if(c==003 || c==CAN || c==013)
				break;
		}
		closetx();
#ifdef XMODEM
		sendline(024);	/* squelch in case user downloading */
#endif		
	}
	/* cancel rest of files if ^C ^X */
	if(c==003 || c==CAN)
		return ERROR;
	else
		return OK;
}

/* fill buf with count chars padding with ^Z for CPM */
filbuf(buf, count)
char *buf;
{
	register c, m;
	m=count;
	while((c=getc(fin))!=EOF) {
		*buf++ =c;
		if(--m == 0)
			break;
	}
	if(m==count)
		return 0;
	else
		while(--m>=0)
			*buf++ = 032;
	return count;
}

dodir(argc, argp)
char **argp;
{
/*	printf("Directory\n");*/
	int pdirent();
	cfast=0;		/* counter for 4 across format */
	expand(pdirent, argc, argp);
}
pdirent(name)
{
	printf("%-14s%c", name, (++cfast&03)?' ':'\n');
}
#ifndef CDOS
/* docomp does a directory listing showing sectors for each matched file
 * and computes total transmission time of matched files in batch mode
 * time is sum of:
 * 	number of files * open/close time (assumed 5 seconds)
 *	time to xmit and ACK each sector assuming no path delay or error
 *	disk i/o time at each end, not dependent on baud rate
 */
docomp(argc,argp)
char **argp;
{
	unsigned compsecs();
	unsigned spm;	/* sectors per minute-baud */
	unsigned dminutes;	/* tenths of minutes */
	cfast=Numsecs=Numblks=0;
/*	printf("Directory\n");*/
	expand(compsecs, argc, argp);

	spm= Baudrate/23;	/* (Baudrate*60)/(10 bits each char * 136 chars) */
	dminutes= (Numfiles/2)+((10*(Numfiles+Numsecs))/spm)+(Numsecs/25);
	printf("\n%u Files %u Blocks %u K\n",
	  Numfiles, Numblks, (Numblks*(Secpblk/8)));
	printf("%u Sectors %u.%u Minutes Xmsn Time at %u Baud\n",
	  Numsecs, dminutes/10, dminutes%10, Baudrate);

}
/* add file length (in CP/M 128 byte records) to Numsecs */
unsigned compsecs(ufn)
char *ufn;
{
	struct fcb fstat;
	printf("%-14s", ufn);
	unspace(ufn);
	setfcb( &fstat, ufn);
	bdos(COMPFILSIZ, &fstat);
	Numsecs += fstat.recn;
	Numblks += (fstat.recn+Secpblk)/Secpblk;
	printf("%4u%c",fstat.recn, (++cfast&03)?' ':'\n');
}
#endif

expand(fnx, argc, argp)
int (*fnx)();
char **argp;
{
	char name[PATHLEN], *s;
	Numfiles=0;

	if(argc<=0)
		return e1xpand(fnx, "*.*");
	else
		while(--argc>=0) {
			/* change b: to b:*.*     */
			strcpy(name, *argp++);
			if((s=index(':', name)) && *++s == 0)
				strcpy(s, "*.*");
			if(e1xpand(fnx, name)==ERROR)
				return ERROR;
		}
}

/*
 * e1xpand expands ambiguous pathname afnp
 * calling fnx for each.
 * Modified from: Parameter list builder by Richard Greenlaw
 *                251 Colony Ct. Gahanna, Ohio 43230
 */
e1xpand(fnx, afnp)
int (*fnx)();
char *afnp;	/* possible ambiguous file name*/
{
	struct fcb sfcb, *pfcb;
	char *p, *q, i, byteaddr;
	int filecount, m;
	char tbuf[SECSIZ];
	struct {
		char xYxx[UFNSIZE];		/* unambiguous file name */
	} *fp;
	int strcmp();


	/* build CPM fcb   */
	unspace(afnp);
	if(setfcb(&sfcb, afnp) == ERROR) {
		printf("%s is bad pattern\n", afnp);
		return ERROR;
	}

	if(Wrapped || (bufend-bufcq)<2048) {
		dumprxbuff();		/* I need the space for building the pathlist */
		clearbuff();		/* so the printer won't try to list dir's */
		bufmark=bufst;
	} else
		bufmark=bufcq;

	/* Search disk directory for all ufns which match afn*/
	for(fp=bufmark,filecount=0;; fp++,filecount++) {
tryanother:
		bdos(SETDMA, tbuf);
		if((byteaddr=bdos(filecount? SRCHNXT:SRCH, &sfcb))==255)
			break;
		/* calculate pointer to filename fcb returned by bdos */
		pfcb = (tbuf + 32 * (byteaddr % 4));
#ifdef RESTRICTED
		/* check for tag bit on 2nd byte of filename (TAG2) */
		if(pfcb->fname[1]&0200)
			goto tryanother;
#endif
		Numfiles++;
		p = fp;
		if(fp>bufend) {	/* Note: assumes some slop after bufend! */
			printf("Out of Memory for pathname expansion\n");
			return ERROR;
		}
		if(*(afnp+1) == ':') {
			/* Drive spec.*/
			*p++ = *afnp;
			*p++ = ':';
		}

		/*Copy filename from directory*/
		q = pfcb;
		for(i =8; i; --i)
			*p++ = (0177& *++q);
		*p++ = '.' ;

		/*Copy file extent*/
		for(i = 3; i; --i)
			*p++ = (0177& *++q);
		*p = '\0' ;

	}

	qsort(bufmark, filecount, UFNSIZE, strcmp);

	for(fp=bufmark; --filecount>=0;) {
		p=fp++;
		/* execute desired function with real pathname */
		if((*fnx)(p)==ERROR)
			return ERROR;
	}
}

/*
 * cisubstr(string, token) searches for lower case token in string s
 * returns pointer to token within string if found, NULL otherwise
 */
char *cisubstr(s, t)
char *s,*t;
{
	char *ss,*tt;
	/* search for first char of token */
	for(ss=s; *s; s++)
		if(tolower(*s)==*t)
			/* compare token with substring */
			for(ss=s,tt=t; ;) {
				if(*tt==0)
					return s;
				if(tolower(*ss++) != *tt++)
					break;
			}
	return NULL;
}
#ifdef XMODEM

/*
 * lprintf is like regular printf but uses direct output to console
 * This prevents status printouts from disrupting file transfers, etc.
 */

lprintf(a,b,c,d,e,f)
char *a, *b, *c, *d, *e, *f;
{
	char lbuf[CMDLEN], *s;
	/* format data into lbuf */
	sprintf(lbuf, a,b,c,d,e,f);
	/* now send lbuf to console directly */
	for(s=lbuf; *s; ) {
		if(*s=='\n') {
			while(!COREADY)		/* expand \n to \r\n */
				;
			outp(CDATA, '\r');
		}
		while(!COREADY)
			;
		outp(CDATA, *s++);
	}
}
#endif

/* copy string s onto itself deleting spaces "hello there" > "hellothere" */
unspace(s)
char *s;
{
	char *p;
	for(p=s; *s; s++)
		if(*s != ' ')
			*p++ = *s;
	*p++ =0;
}

#ifdef LOGFILE
/*
 * logfile keeps a record of files transmitted.
 * Mode is 's' for send file, 'r' for receive file, 't' for type file
 * Lifted from xcmodem by J. Wierda,R. Hart, and W. Earnest
 */
#define RPB 4	/* log records per buffer */
#define LRL 32	/* logical rec length LRL*RPB == SECSIZ */
logfile(name, mode)
char *name;
char mode;
{
	struct fcb *fcbaddr(),*fp;
	int fd,rnum,bnum;
	char *i, *lrec, lbuf[SECSIZ+2];
	if(i=index(':', name))
		name= ++i;		/* discard possible drive spec */
	bdos(SETGETUSER, 0);	/* get it from user 0 */
	fd=open(LOGFILE,2);
	if(fd == ERROR) {	/* if file absent, create and initialize it */
		fd = creat(LOGFILE);
		if(fd == ERROR)
			return;
		else {
			rnum=1;
			fp=fcbaddr(fd);
			fp->fname[1] |= 0200;	/* set TAG2 bit */
			bdos(SETATTRIB, fp);	/* and update attributes */
		}
	}
	else {
		read(fd, lbuf, 1);
		rnum=atoi(lbuf)+1;
	}
	setmem(lbuf, SECSIZ, 0);
	sprintf(lbuf, "%d\r\n", rnum);
	seek(fd,0,0);
	write(fd, lbuf, 1);		/* update last record number */
	bnum=rnum/RPB;
	rnum=(rnum%RPB)*LRL;
	seek(fd, bnum, 0);
	if(rnum==0)
		setmem(lbuf, SECSIZ, 0);
	else
		read(fd, lbuf, 1);
	sprintf(&lbuf[rnum], "%s %c %u\r\n", name, mode, Baudrate);
	seek(fd, bnum, 0);
	write(fd,lbuf,1);
	close(fd);
	bdos(SETGETUSER, user);
}
#endif
/
logfile(name, mode)
char *name;
char mode;
{
	struct fc