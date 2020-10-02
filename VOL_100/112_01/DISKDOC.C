/*  TAB s5,4
 *
 *  diskdoc
 *  a utility for diskette maintainance
 *
 *  by egil kvaleberg
 *     studpost 111
 *     n7034 trondheim-nth
 *     norway
 *
 *  diskdoc is intended for use and distribution
 *  among amateur computer users only. please respect this.
 *
 *  compile using the Ron Cain small C compiler as distributed
 *  by The Code Works. if you use the version of the compiler shown
 *  in the Dr. Dobbs May 1980 issue, you must ensure that the stack
 *  is properly initialized. if you use another C compiler, you'll
 *  have to revise (parts of) the file ddocsys.c. good luck.
 */

#define VERSION "ver c - 1 nov 81"
#define WHOMADEIT "by egil kvaleberg"

/*
 *  constants, change as required
 */

#define SECLEN 128	/* sector length, 128 or 256 is ok */

#define VOID 229	/* character used when zeroing diskette */

#define DHOME 26	/* default home character */
#define ESC 27		/* lead-in character */

#define CR 13		/* various ascii equates */
#define LF 10
#define BS 8
#define QOT 39

/*
 *  globals
 */

int tracks,		/* number of tracks and sectors */
    sectors,firstsector;

int dfltrk,dflsec;	/* default values */
char dfldrv;

char secbuf[SECLEN];	/* buffer for patch */
char vfybuf[SECLEN];	/* buffer for write verify */

int home;		/* character to move cursor home */

/*
 *  begin
 */

main()
{   char cmd;
    dfldrv='a'; dfltrk=0; dflsec=0;
    home=DHOME;
    if (sysok()==0) exit();
    nl(); puts("welcome to the world of diskdoc "); 
    nl(); puts(VERSION); nl(); puts(WHOMADEIT);
    nl(); nl(); puts("kindly enter your request");
    cmd=0;
    while (cmd!='e') {
	nl();
	puts("adapt, backup, compare, exit, patch, scan, test or zero ?");
	cmd=getcmd("abcepstz",'e');
	if (cmd=='a') adapt();
	else if (cmd=='b') backup();
	else if (cmd=='c') compare();
	else if (cmd=='p') patch();
	else if (cmd=='s') scan();
	else if (cmd=='t') test();
	else if (cmd=='z') zero();
	else {
	    nl(); puts("re-insert the system diskette and type <cr> ");
	    if (conlower()!=CR) cmd=0;
	    nl();
	}
    }
    nl(); puts("thanks for having consulted diskdoc");
    exit();
}

/*
 *  adapt to terminal
 */

adapt()
{   nl(); nl(); puts("type character to move cursor home..");
    putbyte(home=conin());
    if (home==ESC) {
	puts(".."); putbyte(home=conin());
	home=home+128;
    }
    nl();
}

/*
 *  backup, copy entire diskette
 */

backup()
{   int trk,ftrk,ltrk;
    char sdrive,ddrive; char *adr;
    if (seltwo(&sdrive,&ddrive,
	       "source diskette in drive (a-h) ?",
	       "destination diskette in drive (a-h) ?")==0) return;
    if (cont()==0) return;
    trk=-1; 
    while (nextt(&trk)) {
	adr=botmem();
	ftrk=trk;
	if (baksel(sdrive,ddrive,"source")==0) return;
	while (1) {
	    if (readtrk(trk,adr)==0) return;
	    ltrk=trk;
	    adr=adr+(SECLEN*sectors);
	    if ((adr+(SECLEN*sectors))>=topmem()) break;
	    if (nextt(&trk)==0) break;
	}
	adr=botmem(); trk=ftrk;
	if (baksel(ddrive,sdrive,"destination")==0) return;
	while (1) {
	    if (writetrk(trk,adr)==0) {
		puts("backup aborted"); nl();
		return;
	    }
	    if (trk>=ltrk) break;
	    nextt(&trk);
	    adr=adr+(SECLEN*sectors);
	}
    }
    nl(); puts("backup finished"); nl();
}

/*
 *  compare contents of two diskettes
 */

compare()
{   int trk,sec;
    char drive1,drive2;
    char *adr1,*adr2;
    if (seltwo(&drive1,&drive2,
	       "compare diskette in drive (a-h) ?",
	       "to diskette in drive (a-h) ?")==0) return;
    if (cont()==0) return;
    trk=-1; 
    while (nextt(&trk)) {
	adr1=botmem();
	adr2=adr1+(SECLEN*sectors);
	if (baksel(drive1,drive2,"first")==0) return;
	if (readtrk(trk,adr1)==0) return;
	if (baksel(drive2,drive1,"second")==0) return;
	if (readtrk(trk,adr2)==0) return;
	sec=firstsector-1;
	while (nexts(&sec)) {
	    if (eqsec(adr1,adr2)==0) {
		dfltrk=trk; dflsec=qskew(sec);
		nl(); puts("compare error in "); putsec(dfltrk,dflsec);
	    }
	    adr1=adr1+SECLEN; adr2=adr2+SECLEN;
	}
    }
    nl(); puts("compare finished"); nl();
}

/*
 *  zero diskette contents
 */

zero()
{   int trk,trkmax,n,m;
    char *adr;
    nl(); nl(); puts("zero diskette in drive (a-h) ?");
    if (getsel()==0) return;
    nl(); puts("zero how many tracks (0-"); putnum(tracks-1); puts(") ?");
    trkmax=getnum(tracks-1,0,tracks-1);
    if (cont()==0) return;
    n=0; adr=botmem(); m=SECLEN*sectors;
    while (n<m) adr[n++]=VOID;
    trk=0;
    while (trk<trkmax) {
	if (writetrk(trk,botmem())==0) return;
	nextt(&trk);
    }
    nl();
}

/*
 *  test a diskette by writing a pattern first,
 *  and then reading it back
 */

test()
{   int trk,sec,n;
    char *adr,*p;
    nl(); nl(); puts("diskette to test in drive (a-h) ?");
    if (getsel()==0) return;
    nl(); puts("the test will destroy the contents of the diskette");
    if (cont()==0) return;
    p="**** diskdoc **** test pattern *"; /* length is 32 */
    n=0;	    /* fill pattern to compare to */
    while (n<SECLEN) {
	secbuf[n]=p[n&31];
	++n;
    }
    puts("does diskette already contain test pattern (y-n) ?");
    if (getcmd("yn",'n')=='n') {
	nl(); puts("writing test pattern...");
	trk=0; sec=firstsector-1;
	while (next(&trk,&sec)) {
	    secbuf[0]=trk; secbuf[1]=qskew(sec);
	    if (tstbrk()) return 0;
	    if (write(trk,qskew(sec),secbuf)==0) {
		nl(); puts("error writing "); putsec(trk,qskew(sec)); nl();
		return 0;
	    }
	}
    }
    trk=-1;	    /* read test */
    nl(); puts("reading test pattern...");
    while (nextt(&trk)) {
	if (readtrk(trk,botmem())==0) return;
	sec=firstsector-1; adr=botmem();
	while (nexts(&sec)) {
	    secbuf[0]=trk; secbuf[1]=qskew(sec);
	    if (eqsec(adr,secbuf)==0) {
		dfltrk=trk; dflsec=qskew(sec);
		nl(); puts("bad test pattern in "); putsec(dfltrk,dflsec);
	    }
	    adr=adr+SECLEN;
	}
    }
    nl(); puts("test completed"); nl();
}

/*
 *  scan: check all sectors for crc errors etc.
 */

scan()
{   int trk,sec;
    nl(); nl(); puts("scan diskette in drive (a-h) ?");
    if (getsel()==0) return;
    if (cont()) {
	nl();
	trk=0; sec=firstsector-1;
	while (next(&trk,&sec)) {
	    if (tstbrk()) return;
	    if (sec==firstsector) {
		puts("track "); putnum(trk); conout(CR);
	    }
	    if (read(trk,qskew(sec),vfybuf)==0) {
		puts("the dubious quality of ");
		putsec(trk,qskew(sec)); puts(" has been detected"); nl();
		dfltrk=trk; dflsec=qskew(sec);
	    }
	}
	puts("scan finished"); nl();
    }
}

/*
 *  patch
 */

patch()
{   int pos;
    nl(); nl(); puts("patch diskette in drive (a-h) ?");
    if (getsel()==0) return;
    if (dflsec<firstsector) dflsec=firstsector;
    getsec(&dfltrk,&dflsec);
    putnls(60); gohome();
    puts("<sp>=next-byte, <bs>=previous-byte, <cr>=next-line, <hex>, ',"); 
    nl();
    puts("next, logical-next, trk-sec, re-read, shift-bit, write or quit ?");
    putnls(4);
    puts("    +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +a +b +c +d +e +f");
    puts("   0123456789abcdef");
    pos=pread();
    while ((pos=patchcmd(pos))>=0);
    putnls(60);
}

/*
 *  one patch command
 *  return new position, -1 if quit
 */

patchcmd(pos)
    int pos;
{   char cmd,dig;
    cmd=conlower();
    if (cmd=='q') return -1;
    else if (cmd==' ') pos=(pos+1)%SECLEN;
    else if (cmd==BS)  pos=(pos+SECLEN-1)%SECLEN;
    else if (cmd==CR)  pos=(pos+16)%SECLEN;
    else if (cmd=='s') pos=pshift(pos);
    else if (cmd=='n') pos=pnsect();
    else if (cmd=='l') pos=plsect();
    else if (cmd=='t') pos=psetts();
    else if (cmd=='r') pos=pread();
    else if (cmd=='w') pos=pwrite();
    else if (cmd==QOT) {
	conout(QOT);
	cmd=conin();
	conout(BS);
	if (cmd==BS) tbyte(pos);
	else pos=patchpos(pos,cmd);
    }
    else if ((dig=makhex(cmd))>=0) {
	putdig(dig);
	while ((cmd=conlower())!=BS) if (makhex(cmd)>=0) break;
	conout(BS);
	if (cmd==BS) tbyte(pos);
	else pos=patchpos(pos,(dig<<4)+makhex(cmd));
    }
    return poscur(pos);
}

/*
 *  patch one byte, update screen
 *  and return new position
 */

patchpos(pos,new)
    int pos,new;
{   int tpos;
    secbuf[pos]=new;
    tpos=pos++; 
    if (pos>=SECLEN) pos=0;
    while (tbyte(tpos++)>0);
    return pos;
}

/*
 *  shift one bit at current position
 */

pshift(pos)
    int pos;
{   int tpos,byte,carry;
    tpos=pos;
    carry=0;
    while (1) {
	byte=secbuf[tpos]&255;
	secbuf[tpos]=(byte>>1)|(carry<<7);
	carry=byte&1;
	if (tbyte(tpos++)==0) break;
    }
    return poscur(pos);
}

/*
 *  next sector
 */

pnsect()
{   next(&dfltrk,&dflsec);
    return pread();
}

/*
 *  next logical sector
 *  useful only in systems with mapping between logical and physical
 *  sector numbers
 */

plsect()
{   lnext(&dfltrk,&dflsec);
    return pread();
}

/*
 *  set new track/sector address
 */

psetts()
{   gohome(); putnls(2); putsps(38);
    gohome(); nl();
    getsec(&dfltrk,&dflsec);
    return pread();
}

/*
 *  read and display one sector during patch
 */

 pread()
 {  char *p;
    int pos;
    pos=0;
    while (pos<SECLEN) secbuf[pos++]=0;
    gohome(); putnls(2);
    if (read(dfltrk,dflsec,secbuf)==0) puts("error reading ");
    putsec(dfltrk,dflsec);
    putsps(18); nl(); putsps(24);
    poscur(pos=0);
    while (tbyte(pos++));
    return poscur(0);
 }

/*
 *  write one sector during patch
 *  verify by reading back
 */

pwrite()
{   int c;
    gohome(); putnls(2); c=cont();
    gohome(); putnls(3);
    if (c) {
	if (write(dfltrk,dflsec,secbuf)==0) puts("error writing");
	else if (read(dfltrk,dflsec,vfybuf)==0) puts("error reading back");
	else if (eqsec(secbuf,vfybuf)==0) puts("error on read verify");
    }
    putsps(22);
    return poscur(0);
}

/*
 *  position cursor during patch
 *  position is returned
 */

poscur(pos)
    int pos;
{   int n;
    gohome(); putnls(6); puts("00  ");
    n=pos/16; while (n--) conout(LF);
    n=(pos/16)*16; while (n<pos) tbyte(n++);
    return pos;
}

/*
 *  print byte in patch table
 *  returns -1 if end of line
 *	     0 if end of page
 *	     1 otherwise
 */

tbyte(pos)
    int pos;
{   putbyte(secbuf[pos++]); /* print byte */
    puts(" "); 
    if (pos%16) return 1;
    puts(" '"); 	    /* print ascii equivalents */
    pos=pos-16;
    while (1) {
	conout(makep(secbuf[pos++]));
	if ((pos%16)==0) break;
    }
    puts("'"); nl();
    if (pos==SECLEN) return 0;
    putbyte(pos);	    /* print address too */
    puts("  ");
    return -1;
}

/*
 *  read one track
 *  assume that drive is already selected
 */

readtrk(trk,adr)
    int trk;
    char *adr;
{   int sec;
    char cmd;
    sec=firstsector;
    while (1) {
	if (tstbrk()) return 0;
	if (read(trk,qskew(sec),adr)==0) {
	    nl(); puts("error reading "); putsec(trk,qskew(sec));
	    nl(); puts("continue, retry or quit ?");
	    cmd=getcmd("qcr",'r');
	    if (cmd=='r') {
		read(0,firstsector,vfybuf); /* reposition */
		continue;
	    }
	    dfltrk=trk; dflsec=qskew(sec);
	    if (cmd=='q') return 0;
	}
	adr=adr+SECLEN;
	if (nexts(&sec)==0) return 1;
    }
}

/*
 *  write one track
 */

writetrk(trk,adr)
    int trk;
    char *adr;
{   int sec;
    sec=firstsector;
    while (1) {
	if (tstbrk()) return 0;
	if (write(trk,qskew(sec),adr)==0) {
	    nl(); puts("error writing "); putsec(trk,qskew(sec)); nl();
	    return 0;
	}
	adr=adr+SECLEN;
	if (nexts(&sec)==0) return 1;
    }
}

/*
 *  get track and sector numbers
 */

getsec(t,s)
    int *t,*s;
{   nl(); puts("track (0-"); putnum(tracks-1); puts(") ?");
    *t=getnum(*t,0,tracks-1);
    nl(); puts("sector ("); putnum(firstsector); puts("-"); 
    putnum(sectors-1+firstsector); puts(") ?");
    *s=getnum(*s,firstsector,sectors-1+firstsector);
}

/*
 *  get drive name, select and reset
 *  0 if cannot select
 */

getsel()
{   if (seldrv((dfldrv=getdrv(dfldrv)),&tracks,&sectors,&firstsector)==0) {
	nl(); puts("drive not ready"); nl();
	return 0;
    }
    rstdrv();
    return 1;
}

/*
 *  select two drives
 *  parameters of drives selected must be equivalent
 */

seltwo(d1,d2,text1,text2)
    char *d1,*d2;
    char *text1,*text2;
{   int dtracks,dsectors,dfirstsector;
    nl(); nl(); puts(text1);
    *d1=dfldrv=getdrv(dfldrv);
    nl(); puts(text2);
    *d2=getdrv(dfldrv);
    seldrv(*d1,&tracks,&sectors,&firstsector);	   /* establish constants */
    rstdrv();
    seldrv(*d2,&dtracks,&dsectors,&dfirstsector);
    rstdrv();
    if ((dtracks!=tracks)|(dsectors!=sectors)|(dfirstsector!=firstsector)) {
	nl(); puts("drives are not compatible"); nl();
	return 0;
    }
    return 1;
}

/*
 *  increment track and sector, 0 if no more
 */

next(t,s)
    int *t,*s;
{   if (nexts(s)) return 1;
    return nextt(t);
}

/*
 *  increment sector, 0 if no more
 */

nexts(s)
    int *s;
{   if ((++*s)>(sectors-1+firstsector)) {
	*s=firstsector;
	return 0;
    }
    return 1;
}

/*
 *  increment track, 0 if no more
 */

nextt(t)
    int *t;
{   if ((++*t)>=tracks) return *t=0;
    return 1;
}

/*
 *  return skewed sector number, skew factor is two.
 *  this function is not equivalent to lnext().
 *  the only purpose of this function is to optimize
 *  scan and backup operations.
 *  hard disks will operate faster without a skew.
 *  hard disk is assumed if there are more than 100 tracks (!).
 */

qskew(sec)
    int sec;
{   if (tracks>100) return sec;
    sec=sec+sec-firstsector;
    if (sec>(sectors-1+firstsector))
	return sec-sectors+((sectors&1)==0);
    return sec;
}

/*
 *  compare two sectors
 *  false if not equal
 */

eqsec(sec1,sec2)
    char *sec1,*sec2;
{   int n;
    n=0;
    while (n<SECLEN) {
	if (sec1[n]!=sec2[n]) return 0;
	++n;
    }
    return 1;
}

/*
 *  select drive during backup
 *  return 0 if something fishy
 */

baksel(drv,otherdrv,name)
    char drv,otherdrv; char *name;
{   int dummy;
    if (drv==otherdrv) {
	nl(); puts("insert "); puts(name); puts(" diskette");
	if (cont()==0) return 0;
    }
    if (seldrv(drv,&dummy,&dummy,&dummy)==0) {
	nl(); puts(name); puts(" drive is not ready"); nl();
	return 0;
    }
    return 1;
}

/*
 *  get drive name, enter with default drive
 */

getdrv(dfl)
    char dfl;
{   return getcmd("abcdefgh",dfl);
}

/*
 *  ask for continue or quit, true if continue
 */

cont()
{   char cmd;
    nl(); puts("continue or quit ?");
    cmd=getcmd("cq",'c'); nl();
    return cmd=='c';
}

/*
 *  get single character command,
 *  enter with possible commands and default command
 */

getcmd(cmds,dfl)
    char *cmds,dfl;
{   char cmd,ch;
    while (1) {
	if ((cmd=conlower())==CR) cmd=dfl;
	if (member(cmd,cmds)==0) continue;
	conout(cmd);
	ch=0;
	while (ch!=BS) if ((ch=conlower())==CR) return cmd;
	unput();
    }
}

/*
 *  see if character can be found in a string
 */

member(ch,str)
    char ch,*str;
{   while (*str) if (*str++==ch) return 1;
    return 0;
}

/*
 *  get a unsigned decimal number
 *  enter with default, minimun and maximum values
 */

getnum(dfl,min,max)
    int dfl,min,max;
{   int n,digits,newn; char ch;
    if (dfl>max) dfl=max;
    n=digits=0;
    while (1) {
	if ((ch=conlower())==CR) {
	    if (digits) {
		if (n>=min) break;
	    } else digits=putnum(n=dfl);
	} else if (ch==BS) {
	    if (digits) {
		--digits; n=n/10;
		unput();
	    }
	} else if (((makdec(ch))>=0) & ((newn=n*10+makdec(ch))<=max)) {
	    if ((newn>0)|(digits==0)) {
		++digits; n=newn;
		conout(ch);
	    }
	}
    }
    return n;
}

/*
 *  make character printing
 */

makep(ch)
    char ch;
{   ch=ch&127;
    if ((ch>=' ')&(ch<127)) return ch;
    return ' ';
}

/*
 *  put track and sector numbers
 */

putsec(t,s)
    int t,s;
{   puts("track "); putnum(t);
    puts(" sector "); putnum(s);
}

/*
 *  put decimal number,
 *  return number of digits printed
 */

putnum(n)
    int n;
{   int d;
    if (n>9) d=putnum(n/10);
    else d=0;
    putdig(n%10);
    return d+1;
}

/*
 *  put hex byte
 */

putbyte(n)
    int n;
{   putdig(n>>4);
    putdig(n);
}

/*
 *  put digit
 */

putdig(d)
    int d;
{   if ((d=d&15)>9) conout(d-10+'a');
    else conout(d+'0');
}

/*
 *  put string
 */

puts(str)
    char *str;
{   while (*str) conout(*str++);
}

/*
 *  put spaces
 */

putsps(n)
    int n;
{   while (n--) conout(' ');
}

/*
 *  new line
 */

nl()
{   conout(CR); conout(LF);
}

/*
 *  put newlines
 */

putnls(n)
    int n;
{   while (n--) nl();
}

/*
 *  erase previous character
 */

unput()
{   conout(BS); conout(' '); conout(BS);
}

/*
 *  move cursor to home position
 */

gohome()
{   if (home>=128) conout(ESC);
    conout(home & 127);
}

/*
 *  convert decimal digit, -1 if error
 */

makdec(ch)
    char ch;
{   if ((ch=ch-'0')<0) return -1;
    if (ch<=9) return ch;
    return -1;
}

/*
 *  convert hex digit, -1 if error
 */

makhex(ch)
    char ch;
{   if (ch<'0') return -1;
    if (ch<='9') return ch-'0';
    if ((ch=ch-'a'+10)<10) return -1;
    if (ch<=15) return ch;
    return -1;
}

/*
 *  test for break, true if yes
 */

tstbrk()
{   if (const()) {
	nl(); puts("break");
	if (cont()==0) return 1;
    }
    return 0;
}

/*
 *  get character, no echo
 *  upper case is converted to lower
 */

conlower()
{   char ch;
    ch=conin();
    if ((ch>='A')&(ch<='Z')) ch=ch+'a'-'A';
    return ch;
}

/*
 *  system dependant functions
 */

#include ddocsys.c
 
/*
 *  the small C runtime library (as shown in the Dr. Dobbs
 *  September 1980 issue and as distributed by The Code
 *  Works) must be named ddoclib and edited
 *  so that it contains the primitive functions only:
 *  ccgchar,ccgint,ccpchar,...,ccmult,ccdiv
 */

#include ddoclib
