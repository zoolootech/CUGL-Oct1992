/*
	DISK UTILITY PROGRAM

	Written by Richard Damon
	Modified by Alexander von Obert
	Buergweg 13, D-8500 Nuernberg, W. Germany
	Tel. (0)911/57 13 74 or (0)761/4011-371

	Version 2.0, December 1984

    This program allows the operator to examine and modify
a CPM diskette. All information needed about the specific
format is fetched from the CP/M 2.X BIOS. 
Compile with: CC disk3 -e2500 when using L2.COM

    The commands available in this package are :

Tn  set current track to n 
Sn  set current sector to n
Dn  set current disk number to n 
Bn  set current track and sector to point to block n 
N   set current track and sector to next sector/block
	next sector if last set command was for track or sector
	next block if last set command was for block
I   turn off sector scewing, if used by this installation.
	Turned on again by nxt Dn command
R   read sector/block into buffer
W   write sector/block from buffer
P   print out contents of buffer, along with 
	track/sector/block information
Ea n n n n
    edit buffer starting from location a filling 
	with values n n n.
Fn  Fill buffer with value n
X   exit program
M   print disk allocation map

Notes:
    1)  Multiple commands may be specified on a line except for X
	which must be the only command on the line followed by return.
    2)  Commands may be in upper or lower case letters
    3)  Spaces are ignored except in the E command where they are used
	as separaters for the numbers

Typical commands:
d0t0s1rp	read in the track 0 sector 1 of disk 0 (Drive A) 
		and print it
e1A 4F 20 11	set buffer location 1A to 4F, 1B to 20, and 1C to 11.
e0a 00w		set buffer location 0a to 0 and write buffer
			Note no space after last data byte
nrp		get next buffer and print it

Disk Allocation Map
    The M command is designed to allow the directory blocks 
(the first blocks after th reserved tracks) to be printed out in a 
convinient format. The directory is print out in the following format:

 Section 1:
	The top half of the directory listing is a listing of the name
    inforamtion of the directory entries.  Each line corresponds to 
    1 sector of the directory.  A typical entry would be:
	f=DISKTESTCOM/1 4c
    The first letter is a code letter used to referance into section 2.
    The equal sign indicats that the file exists, a star here indicates
      that this entry is a deleted file.
    Next comes the filename and extension.
    The following /n is printed if this is other then the first extent 
      (extent 0) of a file where n is the extent number of this entry.
    The following number is the hex record count for this extent.

 Section 2:
	The bottom half of the directory listing is a disk allocation 
    map showing which blocks are in use and by which file. Free blocks 
    are indicated by a dot while used blocks are marked by the file 
    control etter asigned to a file in section 1.  This listing has been 
    blocked off in groups of 8 and 16 to ease reading.

If a particular disk format uses more than 64 directory entries, more
maps may be printed. All blocks used in previously shown maps are marked
in section 2 by '@'.

CPM FILE STRUCTURE

    To help the user of this program the following is a brief description
of the format used in CPM.  The first 2 or so tracks of a disk are 
reserved for the bootstrap and the copy of the CPM operating system or to 
divide a large (Winchester-)drive inseveral logical units. The following 
tracks, according to the capacity given in the disk prameter block (DPB), 
store the data of this logical drive.  To speed up disk access, CPM allows 
to store consecutive data in not consecutive sectors.  Instead the designer 
of the bios may choose to skip e.g. 5 physical sectors between logically
following sectors. This allows to read, compute and read again more
than one sector per turn of the disk. The designer puts a translation
table in the BIOS. For a skip factor of 6 this list is:
 1 7 13 19 25 5 11 17 23 3 9 15 21 2 8 14 20 26 6 12 18 24 4 10 16 22
This interleaving is taken care of when reading in multiple sectors
or when incrementing the disk address with the N command.
     To simplify the disk allocation scheme the sectors are the collected 
into groups of 8, 16 or more, called blocks. These blocks are numbered 
from 0 starting at the beginning of the dirctory. 
    The directory is organized to use the first block(s) of storage to
store information on the file extensions.  A file extension is a part of 
a file.  The directory entry for a file extension is as follows:

byte  0   : file code : 0 if file exists, E5 if file is deleted
byte  1- 8: file name : ascii representation of file name
byte  9-11: file type : ascii representation of file type
byte 12   : file ext  : binary number for extent number
byte 13,14: unused
byte 15   : rec count : count of number of sectors in extent
byte 16-31: map       : list of block numbers used by this extent

*/
/* Two data structures are extracted from the bios:
*/
int	*dph;		/* disk parameter header, contains
			   following information:			*/
#define	XLT	0	/* address of logical to physical
			   translation vector				*/
#define	DPB	5	/* address o disk parameter block		*/

char	*dpb;		/* disk parameter block, contains
			   following information:			*/
#define	SPT	0	/* int, number of sectors per track		*/
#define	BSH	2	/* byte, data allocation block shift factor	*/
#define	BLM	3	/* byte, data allocation block length mark	*/
#define	DSM	5	/* int, total storage capacity of drive		*/
#define	DRM	7	/* int, number of directory entries		*/
#define	OFF	13	/* int, number of reserved tracks		*/

#define	MAXL	4	/* maximum block length (kByte)			*/
#define MAXB	1000	/* maximum number of blocks			*/
#define	MAXE	192	/* maximum number of directory entries		*/
#define STAT	1	/* set true, if you want a STAT DSK: like
			   (but more detailled) display every time
			   you exit a Dn command			*/
#define	MINS	0	/* maybe you must change that to 1		*/
			/* for those withot a copy of the CP/M alternation 
			   guide: names of the BIOS enty points:	*/
#define SELDSK	9	/* next drive to access, C=0 --> A:		*/
#define SETTRK	10	/* BC = next track to access			*/
#define SETSEC	11	/* BC = next sector to access			*/
#define SETDMA	12	/* BC = start of 128 byte file buffer		*/
#define READ	13	/* read, on RET: A=0: ok, A=1: read error	*/
#define WRITE	14	/* wite, on RET: A=0: ok,A=1: write error	*/

int	maxtrk, maxsec, maxblk, blockl, restrk, entries, capac, longbn;
char	*physlog, buff[80], *bufp;

main(){
	int track,sector,disk,nsect,t,s,i,j,k,l,block,d;
	char buffer[1024*MAXB],c,mc;
	char dir[32*MAXE],map[MAXB];

	puts ("\nDisk Utility Program V 2.0\n");
	disk=getdiskp(bdos(25));
	track=0;
	sector=MINS;
	nsect=1;
	puts("\nDUP>");
	while(tolower(*(bufp=gets(buff))) != 'x' || *(bufp+1) != '\0'){
		while((c=*bufp++) != '\0')
		switch(toupper(c)){
			case 'T' : track=getnum(&bufp,0,maxtrk-1,10);
				nsect=1;
				break;
			case 'S' : sector=getnum(&bufp,MINS,maxsec-1+MINS,10);
				nsect=1;
				break;
			case 'D' : disk=getdiskp(getnum(&bufp,0,15,10));
				break;
			case 'B' : block=getnum(&bufp,0,maxblk-1,16);
				nsect=8*blockl;
				track=restrk+(block*blockl*8)/maxsec;
				sector=(block*8*blockl)%maxsec;
				break;
			case 'N' : sector+=nsect;
				if(!update(&sector,&track))sector-=nsect;
				break;
			case 'I' : 
				break;
			case 'R' : bios(SELDSK,disk);
				t=track;
				s=sector;
				for(i=0;i<nsect;i++){
					bios(SETTRK,t);
					bios(SETSEC,transl(s));
					bios(SETDMA,&buffer[i*128]);
					if(bios(READ))puts("\nCan't read");
					if(i!=nsect-1){
						s++;
						if(!update(&s,&t)){
							s--;
							break;
						}
					}
				}
				break;
			case 'W' : bios(SELDSK,disk);
				t=track;
				s=sector;
				for(i=0;i<nsect;i++){
					bios(SETTRK,t);
					bios(SETSEC,transl(s));
					bios(SETDMA,&buffer[i*128]);
					if(bios(WRITE))puts("\NCan't write");
					bdos(13);
					if(i!=nsect-1){
						s++;
						if(!update(&s,&t)){
							s-=1;
							break;
						}
					}
				}
				break;
			case 'P' : block=sector+(track-restrk)*maxsec;
				printf("track %d  sector %d ",track,sector);
				printf(" block %x.%d ",
					block/(8*blockl),block%(8*blockl));
				for(i=0;i<128*nsect;i+=16){
					printf("\n %4x  ",i);
					for(j=0;j<16;j++){
						printf("%2x ",buffer[i+j]);
						if(j%4 == 3) printf(" ");
					}
					for(j=0;j<16;j++){
						c=buffer[i+j]&0x7f;
						c=c<' '||c==0x7f ? '.' : c;
						putchar(c);
					}
					if(kbhit()) break;
				}
				putchar('\n');
				break;
			case 'E' : i=getnum(&bufp,0,nsect*128-1,16);
				while(*bufp==' '){
					buffer[i++]=getnum(&bufp,0,255,16);
					if(i>=nsect*128) break;
				}
				break;
			case 'F' : i=getnum(&bufp,0,255,16);
				for(j=0;j<nsect*128;j++) buffer[j]=i;
				break;
			case 'M' : bios(SELDSK,disk);
				/* read directory from disk		*/
				copydir(dir);
				/* output allocation info		*/
				l=0;
				setmem(&map,maxblk,'.');
				while(l<=entries){
					/* output up to 64 dir. entries	*/
					j=(l+64<entries) ? l+64 : entries+1;	
					for(i=l;i<j;i++){
						if(i%4==0) putchar('\n');
						mc=putentry(dir+i*32,i%64);
						setmap(map,dir+i*32+16,mc);
					}
					puts("\n");
					/* output block map		*/
					for(i=0;i<maxblk;i++){
						bldblk(i);/* format	*/
						putchar(map[i]);
						if(map[i]!='.')map[i]='@';
					}
					putchar('\n');
					l+=64;
					if(l<entries){
						puts("\nNext part of map? (Y/N)");
						if(tolower(getchar())=='n'){
							puts("\n");
							break;
						}
					}
				}
				break;
			case ' ' : break;
			default : printf("%c ??????\n",c);
				*bufp='\0';
				break;
		}
	if(kbhit()) getchar();
	puts("\nDUP>");
	}
}
int getnum(pntr,low,high,base)
int low,high,base;
char **pntr;
{
	int number;
	char c,buffer[50],*bp;
	number=0;
	while( **pntr== ' ') (*pntr)++ ;
	while( (c=toupper(*(*pntr)++))>='0' && c<= '9' ||
		base==16 && (c-=7) > '9' && c <= ('F'-7) )
			number=base*number+c-'0';
	(*pntr)--;
	if (number<low || number>high){
		printf("bad number %d ",number);
		bp=gets(buffer);
		number=getnum(&bp,low,high,base);
	}
	return (number);
}
mapchar(i)
char i;
{	if(i<10) return(i+'0');
	if(i<36) return(i-10+'a');
	return(i-36+'A');
}
putchar(c)
char c;
{
	putch(c&127);
}
int update(sector,track)
char *sector,*track;
{	char ttrack,tsector;
	ttrack=*track;
	tsector=*sector;
	if(tsector>(maxsec-1+MINS)){
		tsector-=maxsec;
		ttrack++;
	}
	if((ttrack==maxtrk)&&(tsector==maxsec)){
		puts("\n*** end of disk ***\n");
		return(0);
	}
	*track=ttrack;
	*sector=tsector;
	return(1);
}
int getdiskp(disk)
int disk;
{	do{	dph=biosh(SELDSK,disk,0);	/* SELDISK returns adr. of DPH */
		if (dph==0){
			puts("\nThis drive does not exist, 0 selected!\n");
			return(0);
		}
	}while (dph==0);

	physlog=dph[XLT];
	dpb=dph[DPB];
	maxsec=ctoi(&dpb[SPT]);
	blockl=bshblm(dpb[BSH],dpb[BLM]);
	restrk=ctoi(&dpb[OFF]);
	maxblk=ctoi(&dpb[DSM])+1;
	maxtrk=restrk+maxblk*blockl*8/maxsec+1;
	entries=ctoi(&dpb[DRM]);
	capac=blockl*maxblk;
	if(maxblk>255)longbn=1;
	else longbn=0;
#if STAT
	printf("\nDisk parameter header at %X",dph);
	printf("\nDisk parameter block at %X",dpb);
	if(physlog)printf("\nSector translation table at %X",physlog);
	else puts("\nNo sector translation");
	printf("\N\N%4d 128 Byte Record Capacity", 8*blockl*(capac+1));
	printf("\n%4d Kilobyte drive capacity", capac);
	printf("\n%4d 32 Byte Directory Entries",entries+1);
	printf("\n%4d Records/ Extent", 8*8*blockl*(2-longbn));
	printf("\n%4d Records/ Block",8*blockl);
	printf("\n%4d sectors/ track",maxsec);
	printf("\n%4d Reserved Tracks",restrk);
	printf("\n%4d tracks on disk",maxtrk);
	printf("\n%4d blocks on disk",maxblk);
	puts("\n");
#endif
	if (entries<=MAXE)
		if(maxblk<=MAXB)
			if(blockl<=MAXL) return(disk);
	puts("\nYour disk would bomb this program.");
	puts("\nSo please set MAXE, MAXB and MAXL constants to");
	puts("\nappropriate values and compile this program again!");
	exit();
}
int transl(logsec)
int logsec;
{	if (physlog==0)return(logsec);
	else return(physlog[logsec]);
}
int bshblm(bsh,blm)
int bsh,blm;
{	int blockl;
	for (blockl=-2;bsh--;blockl++){
		if(blm==0){
			puts("\nBLM too small.\n");
			blockl=0;
			break;
		}
		blm>>=1;
	}
	if(blm!=0)puts("\nBLM too big.\n");
	return(blockl);
}
copydir(dir)
char *dir;
{    int sector, track, i;
     track=restrk;
     sector=0;
     for(i=0;i<(entries+1)/4;i++){
		bios(SETTRK,track);
		bios(SETSEC,transl(sector));
		bios(SETDMA,dir+i*128);
		if(bios(READ))puts("\nCan't read");
		sector++;
		update(&sector,&track);
	}
}
char putentry(dir,i)
char *dir;
int  i;
{	char c,d,mc;
	c=(*dir==0) ? '=' : '*';
	d=dir[12];
	mc=mapchar(i);
	if(d==0xe5)		/* entry not used before */
		printf("%c%18s",mc,"");
	else{			/* output filename 	*/
		dir[12]=0;
		printf("%c%c%s",mc,c,dir+1);
				/* output extension number */
		if(d != 0) printf("/%x",d%0x10);
		else printf("  ");
				/* output record count	*/
		printf(" %2x ",dir[15]);
	}
	return (c=='=') ? mc : mc + 128;	
}
setmap(map,blnum,mc)
char *map, *blnum, mc;
{	int k,l;
	if(longbn){
		for(k=0;k<16&&(l=ctoi(blnum+k));k+=2){
			if(mc<128) map[l]=mc;
		}
	}
	else{
		for(k=0;k<16&&(l=blnum[k]);k++){
			if(mc<128) map[l]=mc;
		}
	}
}
int ctoi(integer)
char *integer;
{
	return(*integer+(*(integer+1)<<8));
}
bldblk(i)
int i;
{	if(i%64==0) putchar('\n');
	else if(i%16==0) printf("  ");
	else if(i%8==0) printf(" ");
}