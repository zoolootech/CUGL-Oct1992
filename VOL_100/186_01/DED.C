
/************************************************
*						*
*   d/ed.c					*
*		version 1.5 12/15/84		*
*		by Collin Brendemuehl		*
*						*
*************************************************
*	An inexpensive disk editor		*
*	   for use with cp/m 2.2 computers	*
*	This program source code was compiled	*
*	   on the C/80 C compiler vers. 3.1	*
*	   from The Software Toolworks		*
*************************************************
*	NOTE:	set Cmode in your library for	*
*		character i/o			*
*************************************************
*						*
*	As of this date, this program is	*
*	hereby donated to the public domain.	*
*	It is donated on the freeware concept.	*
*	If you would like to make a donation	*
*	for the program, please send what you	*
*	feel an appropriate value to:		*
*		Collin Brendemuehl		*
*		721 S. Littler Place		*
*		Edmond OK 73034			*
*						*
*************************************************
*						*
*	Its purpose is to make system use	*
*	for budding hackers (in the tradi-	*
*	tional sense) possible.  It should	*
*	be of more functional use than DU.COM	*
*						*
************************************************/

#define NOFLOAT
#include fprintf.c

unsigned drive,track,sector,block;
unsigned blksz,dsm,allocated,blksec;

int maxtrk,maxsec;
int *dpb1,*dpb2;

unsigned otrack,osector;
int found;
int list,console;
int bell;

long dsksz;

char *dpb;
char dma[128];
char fstring[32];
char hi[3];
char fi[3];
char buffers[128*8];

main()
	{
	display();		/* signon */
	setup();		/* setup variables & screen */
	while(1)		/* infinite loop */
		{
		screen();	/* display data	*/
		command();	/* get command	*/
		}
	}

display()
	{
	putc(26,0);
	locate(8,38);
	fprintf(console,"D/ED");
	locate(10,33);
	fprintf(console,"A Disk Editor");
	locate(12,39);
	fprintf(console,"by");
	locate(14,31);
	fprintf(console,"Collin Brendemuehl");
	locate(23,0);
	fprintf(console,"(c) 1984 by Collin Brendemuehl");
	for(maxtrk=0;maxtrk<=1000;maxtrk++)
		{ /* timing loop here */
		time();
		}
	}

setup()
	{
	int i;
	bell = 7;
	list=fopen("LST:","w");

	sector=0;		/* assign sector */
	drive=bdos(25,0);	/* get drive */
	dpb = bdos(31,0);	/* find dpb */
	dpb1 = dpb;		/* setup 2 other pointers */
	dpb2 = dpb+5;

	blksec = dpb[3]+1;	/* sectors/block */
	blksz = blksec*128;	/* determine block size */
	maxsec = dpb1[0]-1;	/* determine max sector count */
	dsm = *dpb2;		/* determine max # of blocks */
	maxtrk = dpb2[4]+((dsm*blksec)/(maxsec+1));
				/* calculate # of tracks */
	track = dpb2[4];	/* track # is beginning of dir */
	allocated = track;	/* allocated tracks */
	dsksz = maxtrk+1;	/* calculate 'K' of disk space */
	dsksz *= (maxsec+1) * 128;

	putc(26,0);	/* televideo clear screen / home */
	hi[0] = 27;	/* tvi half-intensity */
	hi[1] = 41;
	hi[2] = 0;
	fi[0] = 27;	/* tvi full-intensity */
	fi[1] = 40;
	fi[2] = 0;

	putc(30,0);	/* home the cursor */

	sdata();	/* print disk information */

	procsec();	/* set dma,trk,sec;read sec */
	}

screen()
	{
	int i,j;

	putc(30,0);
	putc('\n',console);
	putc('\n',console);
	putc('\n',console);
			/* print changing information */
	fprintf(console,"Track           : %s%3d%s          ",hi,track,fi);
	fprintf(console,"Sector          : %s%3d%s   \n",hi,sector,fi);
	fprintf(console,"Block           : %s%3d%s          ",hi,block,fi);
	fprintf(console,"Drive           : %s%c%s   \n",hi,drive+65,fi);
			/* print the sector contents */
	for(i=0;i<=127;i++)	/* 128 bytes */
		{
		j=i/4;		/* space every 4 bytes */
		if(j*4==i) putc(' ',console);
		j=i/16;
		if(j*16==i)	/* contents every 16 bytes */
			{
			if(i)	/* if after a line is printed only */
				{
				for(j=i-16;j<i;j++)
					{
					if(dma[j]<32||dma[j]>128) putc('.',console);
					else putc(dma[j],console);
					}
				}
			/* byte count of line */
			fprintf(console,"\n%3d\t",i);
			}
		if(dma[i]<0x10) putc('0',console);
		fprintf(console,"%x",dma[i]);
		}
	putc(' ',console);
			/* last 16 bytes contents */
	for(j=112;j<=127;j++)
		{
		if(dma[j]<32||dma[j]>128) putc('.',console);
		else putc(dma[j],console);
		}
	/* ********** commands ************ */
	putc('\n',console);
	fprintf(console,"Commands: \tQ%suit%s\n",hi,fi);
	fprintf(console,"\tSector: +%sNext  %s-%sPrev%s",hi,fi,hi,fi);
	fprintf(console,"   0%s %s2%s %sZ%sap%s\n",hi,fi,hi,fi,hi,fi);
	fprintf(console,"\t        W%srite  %sE%sdit%s",hi,fi,hi,fi);
	fprintf(console,"C%sall    %sR%secall%s\n",hi,fi,hi,fi);
	fprintf(console,"\tTrack : B%sack   %sA%shead  %sH%some%s\n",hi,fi,hi,fi,hi,fi);
	fprintf(console,"\tSelect: T%srack  %sS%sector%s\n",hi,fi,hi,fi);
	fprintf(console,"\tDisk  : D%srive Logged%s\n",hi,fi);
	fprintf(console,"\tSearch: L%socate %s*%sAgain%s\n",hi,fi,hi,fi);
	fprintf(console,"\tOther : P%srint   %sF%sile%s\n",hi,fi,hi,fi);

	nline();
	fprintf(console,"\t\t\t\tOption: ");
	}

command()
	{
	int comm;

	comm=getc(0);		/* get a command character */
	switch(toupper(comm))
		{
		default:	/* any other key */
			putc(8,0);
			command();
			break;
		case '*':	/* locate again */
			star();
			break;
		case '2':	/* goto middle of sector */
			midsec();
			procsec();
			break;
		case '0':	/* goto beginning of sector */
			begsec();
			procsec();
			break;
		case '+':	/* next sector */
			nxtsec();
			procsec();
			break;
		case '-':	/* previous sector */
			prvsec();
			procsec();
			break;
		case 'A':	/* ahead track */
			aheadt();
			procsec();
			break;
		case 'B':	/* back track */
			backt();
			procsec();
			break;
		case 'C':	/* store sector in a buffer */
			storsec();
			break;    
		case 'D':	/* change logged drive */
			logdrv();
			procsec();
			break;
		case 'E':	/* edit sector contents */
			edit();
			break;
		case 'F':	/* disk files ts0.dat to ts9.dat */
			fileit();
			break;
		case 'H':	/* home the disk head */
			homehd();
			procsec();
			break;
		case 'L':	/* locate string/data */
			locdata();
			break;    
		case 'P':	/* print to the printer */
			printit();
			break;
		case 'Q':	/* quit */
			exit();
			break;
		case 'R':	/* recall sector from buffer */
			recsec(); 
			break;    
		case 'S':	/* specific sector */
			sect();
			procsec();
			break;
		case 'T':
			trak();	/* specific track */
			procsec();
			break;
		case 'W':	/* write sector to disk */
			wrsec();         
			break;
		case 'Z':	/* zap the sector */
			zapsec();
			break;
		}
	}

aheadt()	/* go ahead a track */
	{
	if(track==maxtrk) track=0;
	else track++;
	}

backt()		/* go back a track */
	{
	if(track==0) track=maxtrk;
	else track--;
	}

begsec()	/* goto beginning of sector */
	{
	sector = 0;
	}

logdrv()	/* change logged drive */
	{
	char drv;

	nline();
	fprintf(console,"\t\t\t\t  Drive :  \b");
	drv=getc(0);
	drv=toupper(drv);
	if(drv<'A'||drv>'P') return;	/* out of range */
	drv -= 'A';
	drive = drv;	/* show logged drive */
	chngdr(drv);	/* do the change */
	setup();	/* recalculate for the drive */
	}

edit()	/* edit sector contents */
	{
	int off,i,input;
	char buff[4];

	off=offset();
	for(i=off;i<=127;i++)
		{
		nline();
		fprintf(console,"\t\t\t\t  Data[%3x]: %x   ",i,dma[i]);
		input = valdata();
		switch(input)
			{
			case -1:
				break;
			case -2:
				nline();
				return ;
				break;
			default:
				dma[i]=input;
				screen();
				break;
			}							
		}
	}

fileit()
	{
	static int dfile;
	static char countr = 8;
	static char tens = 9;
	static char *fname=" :TS  .DAT";

	countr++;
	if(countr>9) countr= 0;

	if(countr==0) tens++;

	if(tens>9)
		{
		locate(22,0);
		printf("No more files available %c %c %c",bell,bell,bell);
		time();
		return;
		}

	fname[0] = drive+65;
	fname[5]=countr+0x30;
	fname[4]=tens+0x30;

	dfile=fopen(fname,"w");
	console=dfile;
	sdata();
	screen();
	putc(26,dfile);
	console=0;
	fclose(dfile);
	}

homehd()	/* goto track 0, sector 0 */
	{
	track=0;
	sector=0;
	}

star()		/* locate string a second time */
	{
	int i,f;

	f=0;
	otrack=track;
	osector=sector;
	nline();
	i = -1;
	while((i=substr(fstring))==-1)
		{
		if(track==maxtrk && sector==maxsec)
			break;
		nxtsec();
		procsec();
		}
	nline();
	if(i == -1)
		{
		fprintf(console,"String Not Found");
		track=otrack;
		sector=osector;
		}
	else	{
		fprintf(console,"String Found At Location %d",i);
		}
	procsec();
	oline();
	}

locdata()	/* locate data in sector */
	{
	int i;

	otrack=track;
	osector=sector;
	nline();
	fprintf(console,"\t\tString (ascii) to Locate . . .");
	putc('\n',0);
	getline(fstring,30);
	i = -1;
	while((i=substr(fstring))==-1)
		{
		if(track==maxtrk && sector==maxsec)
			break;
		nxtsec();
		procsec();
		}
	nline();
	if(i == -1)
		{
		fprintf(console,"String Not Found");
		track=otrack;
		sector=osector;
		}
	else	{
		fprintf(console,"String Found At Location %d",i);
		found=i;
		}
	procsec();
	oline();
	}

midsec()	/* goto middle of sector */
	{
	sector=maxsec/2;
	}

nxtsec()	/* goto next sector */
	{
	sector++;
	if(sector > maxsec)
		{
		sector=0;
		track++;
		}
	if(track>maxtrk) track=0;
	}

prvsec()	/* goto previous sector */
	{
	sector--;
	if(sector == -1)
		{
		sector=maxsec;
		track--;
		}
	if(track == -1) track=maxtrk;
	}

printit()
	{
	console=list;
	sdata();
	screen();
	list=0;
	}

storsec()	/* store sector in buffer */
	{
	int buff;
	int i;

	nline();
	fprintf(console,"\t\t\t\tBuffer: ");
	buff=getc(0);
	if(buff<'1' || buff>'8') return;
	buff = buff - 0x31;
	buff = buff * 0x80;
	for(i=buff;i<=buff+127;i++) buffers[i]=dma[i-buff];
	}

recsec()	/* recall sector from buffer */
	{
	int buff;
	int i;
	nline();
	fprintf(console,"\t\t\t\tBuffer: ");
	buff=getc(0);
	if(buff<'1' || buff>'8') return;
	buff = buff-49;
	buff *= 128;
	for(i=buff;i<=buff+127;i++) dma[i-buff]=buffers[i];
	}

sect()
	{
	int i;
	char seclin[8];

	nline();
	fprintf(console,"\t\t\t\tSector: ");
	getline(seclin,5);
	i=atoi(seclin);
	if(i>maxsec) sect();
	if(i<0) sect();
	sector=i;
	}

trak()
	{
	int i;
	char trklin[8];

	nline();
	fprintf(console,"\t\t\t\t Track: ");
	getline(trklin,5);
	i=atoi(trklin);
	if(i>maxtrk) trak();
	if(i<0) trak();
	track=i;
	}

wrsec()		/* bios call to write sector */
	{
	#asm
	MVI	C,0
	LXI	D,39
	LHLD	0001
	DAD	D
	PCHL
	#endasm
	}

zapsec()
	{
	int i;

	for(i=0;i<=127;i++) dma[i] = 0xe5;
	}

procsec()
	{
	bdos(26,dma);	/* set dma address */
	settrk(track);	/* set the track */
	setsec(sector);	/* set the sector */
	readsec();	/* read the sector */
		/* calculate block number if in data area */
	if(track>=allocated) block=(((track-allocated)*(maxsec+1))+sector)/blksec;
	else block = 0;
	}

locate(row,col)		/* column,row positioning (televideo) */
	int row,col;
	{
	fprintf(console,"%c%c%c%c",0x1b,'=',row+32,col+32);
	}

offset()		/* returns byte offset for sector editing */
	{
	char offst[4];

	nline();
	fprintf(console,"\t\t\t\t Offset: ");
	getline(offst,4);
	return(atoi(offst));
	}

valdata()		/* gets valid data input for sector */
	{
	char secdat[5];

	getline(secdat,4);
	if(strlen(secdat)==0) return -1;
	if(secdat[0]=='.') return -2;
	if(strlen(secdat)>2) valdata();
	if(secdat[0] == '#')
		return secdat[1];
	else
		return(htoi(secdat));
	}

sdata()
	{
	fprintf(console,"Tracks/disk     : %s%3d%s          ",hi,maxtrk+1,fi);
	fprintf(console,"Block Size      : %s%4d%s  \n",hi,blksz,fi);
	fprintf(console,"Sectors/track   : %s%3d%s          ",hi,maxsec+1,fi);
	fprintf(console,"Blocks/disk     : %s%4d%s  \n",hi,dsm,fi);
	fprintf(console,"Allocated tracks: %s%3d%s          ",hi,dpb2[4],fi);
	fprintf(console,"Space on disk   : %s%ld%s  \n",hi,dsksz,fi);
	}

substr(findit)
	char *findit;
	{
	int i,j,k;

	for(i=0;i<127-strlen(findit);i++)
		{
		if(findit[0]==dma[i])
			{
			k=1;
			j=0;
			while(findit[j])
				{
				if(findit[j]!=dma[i+j])
					{k=0;}
				j++;
				}
			if(k && i!=found && sector!=osector) return i;
			}
		}
	return -1;
	}

htoi(strng)
	char *strng;
	{
	int value,i,mult;

	value = 0;
	mult = 1;
	i=strlen(strng)-1;
	while(i > -1)
		{
		if(isalpha(strng[i]))
			value += (toupper(strng[i]) - 0x37) * mult;
		else value += (strng[i] - 0x30) * mult;
		mult *= 16;
		i--;
		}
	return value;
	}

spaces(dist)
	int dist;
	{
	while(dist--)
		{
		putc(' ',0);
		}
	}

nline()		/* new, clear line */
	{
	locate(22,0);
	spaces(70);
	putc(13,0);
	}

oline()		/* new, clear line */
	{
	locate(23,0);
	spaces(70);
	putc(13,0);
	}

chngdr()	/* bdos call to change logged drive */
	{
	#asm
	POP	H
	POP	D
	PUSH	D
	PUSH	H
	MVI	C,14
	JMP	5
	#endasm
	}

time()
	{
#asm
	PUSH	PSW
	MVI	A,0FFH
SAMPLE:	DCR	A
	CPI	0
	JNZ	SAMPLE
	POP	PSW
#endasm
	}

settrk()	/* bios call to set track number */
	{
	#asm
	POP	H
	POP	B
	PUSH	B
	PUSH	H
	LXI	D,27
	LHLD	0001
	DAD	D
	PCHL
	#endasm
	}

setsec()	/* bios call to set sector number */
	{
	#asm
	POP	H
	POP	B
	PUSH	B
	PUSH	H
	LXI	D,30
	LHLD	0001
	DAD	D
	PCHL
	#endasm
	}

readsec()	/* bios call to read sector */
	{
	#asm
	LXI	D,36
	LHLD	0001
	DAD	D
	PCHL
	#endasm
	}

#include stdlib.c
