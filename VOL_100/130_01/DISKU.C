/*
	DISK UTILITY PROGRAM

	Written by Richard Damon
	Version 1.0 May 1980

	Revised by Bob Pasky
	Version 2.0 July 1981
	Version 2.4 Oct. 1981

    This program allows the operator to examine and modify
a CPM diskette, either single density or Micromation's version of
double density.

    The commands available in this package are :

Tn  set current track to n (0-76)
Sn  set current sector to n (1-26/52)
Ln  set current logical sector to n (0-25/51)
Dn  set current disk number to n (0-3)
Bn  set current track and sector to point to block n (0-F2)
N   set current track and sector to next sector/block
	next sector if last set command was for track or sector
	next block if last set command was for block
I   increment sector, if sector>max set to 1 and increment track
R   read sector/block into buffer
W   write sector/block from buffer
P   print out contents of buffer, along with track/sector/block information
Ea n n n n
    edit buffer starting from location a filling with values n n n
Aa "ccc..."
    edit buffer starting from location a with ascii characters ccc...
Fn  fill buffer with value n
X   exit program
M   print disk allocation map
Cn  collate directory: n=0 orders by file type (from typtbl), n=1 by name
?   prints current track, sector, block info
/   repeats last command string

Notes:
    1)  Multiple commands may be specified on a line except for X and /
which must be the only command on the line followed by return.
    2)  Commands may be in upper or lower case letters
    3)  Spaces are ignored except in the E command where they are used
	as separaters for the numbers

Typical commands:
d0t0s1rp	read in the track 0 sector 1 of disk 0 (Drive A) and print it
e1A 4F 20 11	set buffer location 1A to 4F, 1B to 20, and 1C to 11.
e0a 00w		set buffer location 0a to 0 and write buffer
			Note no space after last data byte
nrp		get next buffer and print it

Disk Allocation Map
    The M command is designed to allow the directory blocks (blocks 0 and 1)
to be printed out in a convient format. The directory is print out in the
following format:

 Section 1:
	The top half of the directory listing is a listing of the name
    inforamtion of the directory entries.  Each line corresponds to 1 sector
    of the directory.  A typical entry would be f=DISKTESTCOM/1 4c
    The first letter is a code letter used to referance into section 2.
    The equal sign indicats that the file exists, a star here indicates
       that this entry is a deleted file.
    Next comes the filename and extension.
    The following /n is printed if this is other then the first extent 
       (extent 0) of a file where n is the extent number of this entry.
    The following number is the hex record count for this extent.

 Section 2:
	The bottom half of the directory listing is a disk allocation map
    showing which blocks are in use and by which file. Free blocks are
    indicated by a dot while used blocks are marked by the file control
    letter asigned to a file in section 1.  This listing has been blocked off
    in groups of 8 and 16 to ease reading.

DIRECTORY SORTING

    Collating (sorting) the directory only places the sorted directory
in the buffer. From there you can Write it back to the disk if desired,
producing a sorted directory on the disk. The track, sector and block size
are set up by Collate to their proper values for writing to the directory.

	Sorting can be performed alphabetically by the full name, type and
extent, or grouped by type using a table within this program to sort by
type -- within each type the files are ordered by name and extent. File
types not found in the table are placed at the end, arranged by name, type
and extent.

CPM FILE STRUCTURE

    To help the user of this program the following is a brief description
of the format used in CPM. (Double density values are shown in parentheses.)
The first 2 tracks of a disk are reserved for the bootstrap and the copy of
the CPM operating system.  Tracks 2 through 76 store the data.  To speed up
disk access CPM does not store consecutive data in consecutive sectors.
Instead it uses every (8th or) 6th sector for data. Thus to read logical
consecutive sectors you must read the sectors in the following order:

(single density only)
 1 7 13 19 25 5 11 17 23 3 9 15 21 2 8 14 20 26 6 12 18 24 4 10 16 22

This interleaving is taken care of when reading in multiple sectors
or when incrementing the disk address with the N command.  To simplify
the disk allocation scheme the sectors are the collected into groups of
(16 or) 8 sectors forming a (2k or) 1k block.  These blocks are numbered
from 0 starting with the beginning of the directory -- track 2 sector 1.
Block numbers range from 0 to F2 (hex).
    The directory is organized to use (1 or) 2 blocks of storage -- 16
sectors -- to store information on 64 file extensions.  A file extension
is a part of a file up to 16k bytes long.  The directory entry for a file 
extension is as follows:

byte  0   : file code : 0 if file exists, E5 if file is deleted
byte  1- 8: file name : ascii representation of file name
byte  9-11: file type : ascii representation of file type
byte 12   : file ext  : binary number for extent number (0 to ?)
byte 13,14: unused
byte 15   : rec count : count of number of sectors in extent
byte 16-31: map       : list of block numbers used by this extent

*/

#include <bdscio.h>

#define MAXTYP	14	/* how many file types to sort on */
#define SORTED	'~'	/* this may need changing for CP/M 2.x;
			   anything not seen in file_code is ok */
#define DELETED	0xe5	/* ...anything except E5, of course */
#define ESCAPE 0x1b
#define STRDLM '"'

int	DEBUG;
int	maxsecs, blocksecs, interval, typ, presdisk;
char	density, buffer[2048], dir[2048], map[256];
STATIC_char *typtbl[MAXTYP];

main(ac,av)
int ac;
char **av;
{
	int track, sector, disk, nsect, s, t, rs, rt, i, j, k, block;
	char buff[80], prevbuff[80], *bufp, c, d, mc;

	disk=0;
	track=rt=rs=0;
	sector=1;
	nsect=1;

	DEBUG = 0;
	if (ac > 1 && (*av[1] == 'D')) DEBUG = 1;
if(DEBUG)printf("\nDebug on...\n");

	presdisk = bdos(25,0);	/* find out which disk called from */

	typtbl[0]="COM";	/* change this table as desired */
	typtbl[1]="SUB";
	typtbl[2]="CRL";
	typtbl[3]="CCC";
	typtbl[4]="H  ";
	typtbl[5]="C  ";
	typtbl[6]="   ";
	typtbl[7]="HEX";
	typtbl[8]="DOC";
	typtbl[9]="TXT";
	typtbl[10]="BAK";
	typtbl[11]="BRL";
	typtbl[12]="TMP";
	typtbl[13]="BAD";	/* note that the typtbl vector contains
				   only pointers to the strings
				        ********/

	printf("\n\t\tDisk Utility v 2.4\n");

	printf("Density on %c is Double\n", presdisk+65);
	maxsecs = 52;
	blocksecs = 16;
	interval = 8;

	prevbuff[0] = '\0';	/* init last command string to nil */

	printf("Command? ");
	while(tolower(*(bufp=gets(buff))) != 'x' || *(bufp+1) != '\0'){
		if((*bufp == '/') && (*(bufp+1) == '\0'))
			strcpy(buff,prevbuff);

		strcpy(prevbuff,buff);	/* save last command string */

		while((c=*bufp++) != '\0')
		switch(toupper(c)){
			case 'T' : track=getnum(&bufp,0,76,10,'T',track);
				nsect=1;
				break;

			case 'S' : sector=getnum(&bufp,1,maxsecs,10,'S',
							sector);
				nsect=1;
				break;

			case 'L' : s=getnum(&bufp,0,maxsecs-1,10,'L',
							tolog(sector));
				sector = tophys(s);
				nsect=1;
				break;

			case 'D' : disk=getnum(&bufp,0,3,10,'D',presdisk);
				do {
				printf("\nDensity of %c? (s/d) ", disk+65);
				density = tolower(getchar());
				} while (density != 's' && density != 'd');
				if (density == 's') {
					printf("ingle\n");
					maxsecs = 26;
					blocksecs = 8;
					interval = 6;
					}
				else	{
					printf("ouble\n");
					maxsecs = 52;
					blocksecs = 16;
					interval = 8;
					}
				break;

			case 'B' : block=getnum(&bufp,0,0xf2,16,'B',block);
				nsect=blocksecs;
				track=2 + block*blocksecs / maxsecs;
				s = block*blocksecs % maxsecs;
				sector = tophys(s);
				break;

			case 'N' : for(i=0;i<nsect;i++){
					sector = bumpsector(sector);
					if (sector == 1) track++;
					}
				break;

			case 'I' : sector+=nsect;
				if(sector>maxsecs){
					sector-=maxsecs;
					track++;
					}
				break;

			case 'R' : bios(9,disk);
				rt = t = track;
				rs = s = sector;
				if  (t < 2 && s > 26) {
		printf("\n-- System tracks are not double density --");
					break;
					}
				for(i=0;i<nsect;i++){
					bios(10,t);
					bios(11,s);
					bios(12,&buffer[i*128]);
					bios(13);
					s = bumpsector(s);
					if(s==1)t++;
					}
				break;

			case 'W' : bios(9,disk);
				t=track;
				s=sector;
				if  (t < 2 && s > 26) {
		printf("\n-- System tracks are not double density --");
					break;
					}
				if ((rs!=s) || (rt!=t)) {
		printf("\n--Warning-- Track and/or sector have changed\
since last read.\nDo you wish to continue? (y) ");
				    if (tolower(getchar()) != 'y') {
					printf("...whew!");
					break;
					}
				    else printf ("...ok, here goes...");
				    }
				for(i=0;i<nsect;i++){
					bios(10,t);
					bios(11,s);
					bios(12,&buffer[i*128]);
					bios(14);
					s = bumpsector(s);
					if(s==1) t++;
					}
				break;

			case 'P' : /* display the values from read */
				if (!rs) {
					printf("\n -- empty --\n");
					break;
					}
				sector = rs; track = rt;
				block = (track-2)*maxsecs + tolog(sector);
				printf("track %d  sector %d ",track,sector);
				printf(" block %x.%d ",
					block/blocksecs,block%blocksecs);
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
			case 'E' : i=getnum(&bufp,0,nsect*128-1,16,'E',0);
				while(*bufp==' '){
				   printf("\n%02x: %02x --> ",i,buffer[i]);
				   buffer[i] = getnum(&bufp,0,255,16,'E',0);
				   printf("%02x",buffer[i++]);
				   if(i>=nsect*128) break;
				   }
				break;

			case 'A' : i=getnum(&bufp,0,nsect*128-1,16,'A',0);
				while (*bufp!=STRDLM && *bufp !='\0')
					bufp++;
				if (*bufp=='\0') {
					printf("\nno string");
					break;
					}
				bufp++;
				printf("\n%02x: <",i);
				while(*bufp!=STRDLM && *bufp!='\0') {
					buffer[i++] = *bufp;
					putchar(*bufp++);
					if(i>=nsect*128) {
					    while(*bufp!=STRDLM
						&& *bufp!='\0')
						    bufp++;
					    break;
					    }
					}
				putchar('>');
				if(*bufp==STRDLM) bufp++;
				break;

			case 'F' : i=getnum(&bufp,0,255,16,'F',0);
				for(j=0;j<nsect*128;j++) buffer[j]=i;
				break;

			case 'M' : bios(9,disk);
				bios(10,2);
				s=1;
				for(i=0;i<16;i++){
					bios(11,s);
					bios(12,&dir[i*128]);
					bios(13);
					s = bumpsector(s);
					}
				setmem(&map,256,0);
				for(i=0;i<64;i++){
					if(i%4==0) putchar('\n');
					j=32*i;
					c=(dir[j]==0) ? '=' : '*';
					d=dir[j+12];
					mc=mapchar(i);
					if(d==DELETED){
						printf("%c%18s",mc,"");
						continue;
						}
					else
						dir[j+12]=0;
					printf("%c%c%s",mc,c,&dir[j+1]);
					if(d != 0) printf("/%x",d%0x10);
						else printf("  ");
					printf(" %2x ",dir[j+15]);
					if(c=='*')mc+=128;
					for(k=16;k<32 && dir[j+k];k++){
						d=dir[j+k];
						if(mc<128) map[d]=mc;
						}
					}
				for(i=0;i<0xf3;i++){
					if(i%64==0) putchar('\n');
					else if(i%16==0) printf("  ");
					else if(i%8==0) printf(" ");
					putchar(map[i] ? map[i] : '.');
					}
				putchar('\n');
				break;

			case 'C' : bios(9,disk);
				bios(10,2);
				s=1;
				for(i=0;i<16;i++){
					bios(11,s);
					bios(12,&dir[i*128]);
					bios(13);
					s = bumpsector(s);
					}
				/* set for writing: */
				rs=sector=1; rt=track=2;
				nsect=16;

				typ=getnum(&bufp,0,1,10,'C',0);
				if (typ != 0) typ = MAXTYP;

				for (i = 0; i < 2048; i += 32) {
				   if((k = lowstring() * 32) >= 0) {
					putchar( i%128 ? ' ' : '\n');
					for (j = 0; j < 32; j++)
						buffer[i+j] = dir[k+j];
					dir[k] = SORTED;
				d = dir[k+12] + '0';
				dir[k+12] = 0; /* ensure name ends ok */
				printf("%2d=%s/%c  ",i/32, &dir[k+1], d);
					}
				   else { for ( j = 0; j < 32; j++)
						buffer[i+j] = DELETED;
					}
				   }
				printf("\nUse 'w' to re-write Directory\n");
				break;

			case ' ' : break;

			case '?' :
				block = (track-2)*maxsecs + tolog(sector);
				printf("track %d  sector %d ",track,sector);
				printf(" block %x.%d",block/blocksecs,
						block%blocksecs);
				break;

			default:
				printf("\nBad character <%c> ",c);
				printf("...remainder of line ignored.\n");
				*--bufp = '\0'; /* zap line here */
				strcpy(prevbuff,buff);	/* save zapped */

	printf("\nCommands are:\n");
	printf("\nTn	set current track to n (0-76)");
	printf("\nSn	set current sector to n (1-26/52)");
	printf("\nLn	set current logical sector to n (0-25/51)");
	printf("\nDn	set current disk number to n (0-3)");
	printf("\nBx	set track & sector to point to block n (0-F2)");
	printf("\nN	set track & sector to next sector/block");
	printf("\nI	increment sector");
	printf("\nR	read sector/block into buffer");
	printf("\nW	write sector/block from buffer");
	printf("\nP	print out contents of buffer");
	printf("\nEa x x x...	edit buffer starting from location a");
	printf("\nAa \"ccc...\"	edit buffer from a with character data");
	printf("\nFn	Fill buffer with value n");
	printf("\nM	print disk allocation map");
	printf("\nCn	collate directory: 0=by type/name, 1=by name/type");
	printf("\n?	print current track, sector, block numbers");
	printf("\n/	repeat last command string");
	printf("\nX	exit program");
	printf("\n");
				break;
			}
		printf("\nCommand? ");
		if(kbhit()) getchar();
		}
	bios(9,presdisk);	/* reset disk drive */
	exit();
}

bumpsector(s)
int s;
{
	if (density == 's') {
		if ((s = s+interval) > maxsecs) s -= maxsecs;
		if (s == 2) s = 1;
		else if (s == 1) s = 2;
		return (s);
		}
	else
		return (tophys(tolog(s) + 1));
}

tophys(s)
int s;
{
	while (s >= maxsecs) s -= maxsecs;
	s =  1 + s/13 + (s*interval) % maxsecs;
	if (DEBUG) printf("phys = %d",s);
	return (s);
}

tolog(s)
int s;
{
	int ss, mu;

	while (s > maxsecs) s -= maxsecs;

	/**** comments apply to double density only ****/
	/* adds 52 if s%8 is 5,6,7,8 to s then dividing by interval
	** giving number from 0 to 12.
	** sd: add 9 more if s%6 is 3 or 4; -- a wicked kludge
	*/
	ss = (((s-1)%interval / 4) * maxsecs + s) / interval;
	if (density == 's') ss += ( (s-1) % 6 % 4 / 2) * 9;

	/* to that add 13* s%4 giving number from 0 to 51	*/
	s = ss + (13 * ((s-1) % (maxsecs/13) ));
	while (s > maxsecs) s -=maxsecs;

if(DEBUG) printf("\nlog = %d,",s);

	return (s);
}
getnum(pntr,low,high,base,cmndtype, current)
int low, high, base, current;
char **pntr, cmndtype;
{
	int number;
	char c,buffer[50],*bp;
	char *f1,*f2;

	number = 0;
	while( **pntr== ' ') (*pntr)++ ;
	while( (c=toupper(*(*pntr)++))>='0' && c<= '9' ||
		base==16 && (c-=7) > '9' && c <= ('F'-7) )
			number=base*number+c-'0';
	(*pntr)--;
	if (number<low || number>high){
		if (base == 16) { f1 = "%c%x"; f2 = "%x and %x"; }
		else {		  f1 = "%c%d"; f2 = "%d and %d"; }
		printf(f1, cmndtype, number);
		printf(" is illegal value,\nEnter new number between ");
		printf(f2, low, high);
		printf(": ");
		bp = gets(buffer);
		if (bp[0] == ESCAPE) {
			if (low<=current && current<=high)
				number = current;
			else number = low;
			printf("Default used: ");
			printf(f1, cmndtype, number);
		}
		else number = getnum(&bp,low,high,base,cmndtype);
		}
	return (number);
}

lowstring()
{
	int i,j;
	char *dp;

	if (typ < MAXTYP) do {
	    for (i=0; i<64; i++)
		if ((dir[i*32] == '\0') && (typcmp(&dir[i*32],typ) == 0))
			break;
	   } while ((i == 64) && (++typ < MAXTYP));
	if (typ == MAXTYP) {
		for (i=0; i<64; i++)
			if (dir[i*32] == '\0') break;
		}

if(DEBUG)printf("\ntype: %d", typ);

	if (i == 64) return(-1);
	if (i == 63) return(63);
	for (j=i+1; j<64; j++) {
	    dp = &dir[j*32];
	    if ((*dp != DELETED) && (*dp != SORTED)) {
		if ((typ < MAXTYP) && (typcmp(dp,typ) == 0))
			if (namecmp(dp, &dir[i*32]) < 0 ) i = j;
		if (typ == MAXTYP)
			if (namecmp(dp, &dir[i*32]) < 0 ) i = j;
		}
	    } /* end for */
	return (i);
}

namecmp(s1,s2)
char *s1,*s2;
{
int i,j,k;
char *t;

if(DEBUG)printf("\ns1 <%s>  s2 <%s>", s1+1, s2+1);
	for (i=1; (s1[i] == s2[i]) && (i < 13); i++) ;
	if (s1[i] == s2[i]) return 0;
	if (s1[i] < s2[i]) return -1;
	else return 1;
}

typcmp(s1,typ)
char *s1;
int typ;
{
	int i;
	char *t;

	t = typtbl[typ];
/*if(DEBUG)printf("\ntypcmp: <%s> <%s>", &s1[9], t);*/
	for (i=9; i<12; i++) if (s1[i] != *t++) break;
	if (i==12) return 0;
	else return -1;
}

mapchar(i)
char i;
{	if(i<10) return (i+'0');
	if(i<36) return (i-10+'a');
	return (i-36+'A');
}

putchar(c)
char c;
{
	putch(c);
}

/* the end */

