/*
	Disk Utility Program for CP/M version 2.x

	Original May, 1980	by Richard Damon
	Modified Sept 9, 1981	by Robert Ward
	Modified July, 1982	by Edward K. Ream

	Please send all reports of bugs to:

		Edward K. Ream
		1850 Summit Ave.
		Madison, WI 53705
		(608) 231 - 2952

	See the file anydisk.doc for documentation.

*/

#define VERSION "\nJuly 27, 1982\n\n"


/*
	Define the data structures pointed to by
	the BIOS select disk function.
*/


/* DPH -- Disk Parameter Header.  One for each disk */

struct DPH {

	/* XLT: address of translation table.	*/
	/* one table for each TYPE of disk	*/
	char *	d_XLT;

	/* scratchpad area for BDOS use only	*/
	int	d_TEMP1;
	int	d_TEMP2;
	int	d_TEMP3;

	/* DIRBUF: address of 128-byte directory buffer	*/
	/* all disks use the same buffer		*/
	char *	d_DIRBUF;

	/* DPB: address of DPB. One for each type of drive */
	char *	d_DPB;

	/* CSV:	address of changed-disk check buffer	*/
	/* one for each drive				*/
	/* size of buffer is DPB -> CSK bytes		*/
	char *	d_CSV;

	/* ALV: address of allocation buffer		*/
	/* one for each drive				*/
	/* size of buffer is (DPB -> DSM / 8) + 1 bytes	*/
	char *	d_ALV;

};


/*
	DPB -- Disk Parameter Block
	One for each TYPE of disk
*/

struct DPB {

	/* sector per track */
	int	d_SPT;

	/* BSH: block shift factor		*/
	/* BLM = (2 ** BSH) - 1			*/

	/* BLS: block size. Note size VARIES	*/
	/* BLS = (BLM + 1) * 128		*/
	char	d_BSH;
	char	d_BLM;

	/* EXM: extent mask */
	/* if DSM > = 256 then block numbers take two bytes */
	/* Thus, each extent can hold only 8 block numbers */
	char	d_EXM;

	/* DSM maximum disk BLOCK number		*/
	/* BLOCKS numbered starting at zero		*/
	/* (DSM + 1) * BLS = # of bytes per drive	*/
	int	d_DSM;

	/* DRM:	directory max. directory entries 0 -- DRM */
	int	d_DRM;

	/* AL0, AL1:  allocation vector for directory	*/
	/* each bit reserves one BLOCK			*/
	char	d_AL0;	/* high bit = block 0 */
	char	d_AL1;	/* high bit = block 8 */

	/* CKS: size of directory check vector (or 0)	*/
	/* fixed media:		CKS = 0			*/
	/* removeable media:	CKS = (DRM + 1)/4	*/
	int	d_CKS;

	/* OFF: track offset.  number of skipped tracks */
	int	d_OFF;
};


/* Define BIOS entry points. */

#define SELDSK	9
#define SETTRK	10
#define SETSEC	11
#define SETDMA	12
#define RDSEC	13
#define WTSEC	14
#define	SECTRAN	16	/* BE = logical sec # */
			/* DE = address of translation table */


/* Set this constant to the number of drives you have. */

#define NDRVS 5

/*
	Set BBUFSIZ to the size of the largest block
	that any of your disks use.

	Set MBUFSIZ to the largest number of directory
	entries that are placed on any of your disks.

	There is code (in the select routine) which
	makes sure that these constants are, in fact,
	large enough.  If not,  a message is issued and
	the program aborts, so you shouldn't be able to
	bomb the program (or your disks).
*/

#define BBUFSIZ 4096
#define MBUFSIZ 4096

/* Do not change ANY of the following constants. */

#define PHYS 0
#define LOG 1
#define YES 1
#define NO  0
#define SECSIZE	128
#define table TXTABLE


/* Define global constants set by the select() routine. */

struct DPH * d_header;	/* pointer to DPH		*/
struct DPB * d_disk;	/* pointer to DPB		*/

int	TPD;		/* tracks  per disk		*/
int	SPT;		/* sectors per track		*/
int	BPD;		/* blocks  per disk		*/
int	SPB;		/* sectors per block		*/
char *	TXTABLE;	/* pointer to translate table	*/
int	DIRBLKS;	/* blocks in the directory	*/
int	DIRSIZE;	/* entries in the directory	*/
int	OFFSET;		/* offset of first track	*/


main()
{
	char buffer [BBUFSIZ];
	char buff[80], *bufp, c;
	char *temp;
	int mode, nsects, disk, b, i, j;
	int block, track, sector;

	/* Sign on. */
	printf("Welcome to ddndisk version 3:  ");
	printf(VERSION);

	/* Initialize */
	track	= 0;
	sector	= 0;
	block	= 0;
	mode	= PHYS;
	nsects	= 1;

	/* The default drive is drive A: */
	disk  = 0;
	select(disk);

	for (;;) {
		bufp = gets(buff);

		/* The 'X' request must be the only thing on a line. */
		if (tolower(*bufp) == 'x' && *(bufp + 1) == '\0') {
			break;
		}

		while (c = *bufp++) switch (toupper(c)) {

	case 'B':
		mode   = LOG;
		nsects = SPB;

		/* Get block number from the user. */
		block = getnum(&bufp, 0, BPD-1 ,16);

		/*
			Convert block number to track/sector.
			A loop is used to avoid overflow.
		*/
		track = sector = 0;
		for(b = block; b; b--) {
			sector += SPB;
			if (sector >= SPT) {
				track++;
				sector -= SPT;
			}
		}
		break;

	case 'C':
		/* Fill the buffer with a signed constant. */
		temp = getnum(&bufp, -32765, 32765, 16);
		for(i = 0; i < nsects * SECSIZE; i++) {
			buffer[i] = temp[i];
		}
		break;

	case 'D':
		/* Select the disk and print disk info. */
		disk = getnum(&bufp, 0, NDRVS-1, 10);
		select(disk);
		info();
		break;

	case 'E':
		/* Patch the buffer with a list of chars. */
		i = getnum(&bufp, 0, nsects*SECSIZE-1, 16);
		while(*bufp ==' ') {	
			buffer [i++] = getnum(&bufp,0,255,16);
			if(i >= nsects*SECSIZE) {
				break;
			}
		}
		break;

	case 'F':
		/* Fill the buffer with an unsigned constant */
		i = getnum(&bufp, 0, 255, 16);
		for(j = 0; j < nsects*SECSIZE; j++) {
			buffer [j] = i;
		}
		break;

	case 'H':
		/* Print a help message. */
		help();
		break;

	case 'I':
		/* Enter track/sector mode. */
		if (mode != PHYS) {
			mode = PHYS;
			nsects = 1;

			/* Convert block to track/sector. */
			track = sector = 0;
			for (b = block; b; b--) {
				sector += SPB;
				if (sector >= SPT) {
					track++;
					sector -= SPT;
				}
			}
			track += OFFSET;
		}
		break;

	case 'M':
		/* Print the directory and allocation map. */
		ptmap(disk, table);
		break;

	case 'N':
		/* Go to next sector or block. */
		if (mode == LOG) {
			block++;
			sector += SPB;
		}
		else {
			sector++;
		}

		if (sector >= SPT) {
			track++;
			sector -= SPT;
			if (track >= TPD) {
				/* Stop the scan. */
				*bufp = '\0';
				printf("No next sector.\n");
			}
		}
		break;

	case 'P':
		/* Print the buffer. */
		dmpbuff	(buffer, nsects, mode, disk,
		block, track, sector, table);
		break;

	case 'R':
		/* Read a block or sector into the buffer. */
		rdbuff	(buffer, nsects, mode, disk,
		block, track, sector, table);
		break;

	case 'S':
		/*
			Set LOGICAL sector #.
			Enter track/sector mode.
		*/
		if (mode == LOG) {
			mode = PHYS;
			track = OFFSET;
		}
		nsects = 1;
		sector = getnum(&bufp, 0, SPT - 1, 10);
		break;

	case 'T':
		if (mode == LOG) {
			mode = PHYS;
			sector = 0;
		}
		nsects = 1;
		track = getnum(&bufp, 0, TPD - 1, 10);
		break;

	case 'W':
		/* Write the buffer to the disk. */
		wrbuff	(buffer, nsects, mode, disk,
		block, track, sector, table);
		break;

	case ' ':
		/* Ignore spaces. */
		break;

	default:
		/* Unknown request. */
		printf("%c ?????\n",c);
		*bufp = '\0';
		break;

	} /* end switch */

	if(kbhit()) getchar();
	}
}


/* Call Bios.  Return a word, not just a byte. */

bioshl(function, bc, de)
int function, bc, de;
{
	int * p;
	int q;

	/* get address of BIOS jump table */
	p = 0x0001;
	q = *p + (3*function - 3);

	/* go to BIOS */
	return call(q,0,0,bc,de);
}


/* Dump out the buffer in hex and in ascii. */

dmpbuff(pntr, nsects, mode, disk, block, track, sector, table)
char *pntr;
int nsects, mode, disk, block, track, sector;
char *table;
{
	int i,j,lchar;

	prthdr(mode,nsects,disk,block,track,sector,table);

	for (i = 0; i < nsects * SECSIZE; i += 16) {

		if(i % SECSIZE == 0 && mode == LOG) {
			printf("\n\nRecord %d\n", i/SECSIZE);
		}
		printf("\n%04x ", i);

		for (j = 0; j < 16; j++){
			if(j%4 == 0) {
				printf(" ");
			}
			if (j%8 == 8) {
				printf(" ");
			}
			printf("%02x", pntr [i+j]);
		}

		printf("  ");
		for (j = 0; j < 16; j++) {
			lchar = pntr [i+j];
			if (lchar < 0x1f || lchar > 0x7d) {
				printf(".");
			}
			else {
				printf("%c",lchar);
			}
		}
		if (kbhit()) break;
	}
	printf("\n");
}


/* Get a number from the keyboard. */

getnum(pntr, low, high, base)
int low, high, base;
char **pntr;
{
	int number;
	char c, buffer[50], *bp;

	number = 0;
	while(**pntr == ' ') (*pntr)++;
	while((c = toupper(*(*pntr)++))>='0' && c<= '9' ||
	base ==16 && (c-=7) > '9' && c<= ('F'-7))
		number = base*number+c-'0';
	(*pntr)--;

	if (number < low || number > high) {
		if (base == 16) {
			printf("%x is out of range.\n",number);
			printf("Enter a hex number between ");
			printf("%x and %x:  ", low, high);
		}
		else {
			printf("%d is out of range.\n",number);
			printf("Enter a decimal number ");
			printf("between %d and %d:  ",
			low, high);
		}
		bp = gets(buffer);
		number = getnum(&bp, low, high, base);
	}
	return number;
}


/* Print a helpful message. */

help()
{
	printf("Commands are...\n\n");
	printf("Bn  enter block mode and set block n.\n");
	printf("Cn  fill buffer with short n.\n");
	printf("Dn  set disk to n and print disk info.\n");
	printf("Ea n1 n2 ...  edit buffer[a] with n1 n2 ...\n");
	printf("Fn  fill buffer with unsigned n.\n");
	printf("H   print this message.\n");
	printf("I   enter track/sector mode.\n");
	printf("M   print disk allocation map.\n");
	printf("N   go to next block or track/sector.\n");
	printf("P   print buffer.\n");
	printf("R   read sector or block into buffer.\n");
	printf("Sn  set current sector to n (first sector is 0).\n");
	printf("Tn  set current track  to n (first track  is 0).\n");
	printf("W   write sector or block from buffer.\n");
	printf("X   exit program (must be alone on line).\n");
}


/* Print information about the selected disk. */

info()
{
	printf("Disk Parameter Header  = %x (hex)\n", d_header);
	printf("Disk Parameter Block   = %x (hex)\n", d_disk);
	printf("Sector Translate Table = %x (hex)\n", TXTABLE);
	printf("Track offset:      %d\n", OFFSET);
	printf("Tracks  per disk:  %d\n", TPD);
	printf("Blocks  per disk:  %d, %x (hex)\n", BPD, BPD);
	printf("Sectors per track: %d\n", SPT);
	printf("Sectors per block: %d\n", SPB);
	printf("Chars   per block: %d\n", SPB * SECSIZE);
	printf("Directory blocks:  %d\n", DIRBLKS);
	printf("Directory entries: %d\n", DIRSIZE);
	printf("\n");
}


/* Print the header message of a dump. */

prthdr(mode, nsects, disk, block, track, sector, table)
int nsects, mode, disk, block, track, sector;
char *table;
{
	printf("\ndisk %c, ", disk + 'A');
	if (mode == LOG) {
		printf("block %d, ", block);
	}
	printf("track %d, ",
	track + (mode == LOG ? OFFSET : 0));
	printf("physical sector %d, ",
	bioshl(SECTRAN, sector, table));
	printf("logical sector %d", sector);
	printf("\n");
}


/* Print the directory and a disk allocation map. */

#define M1COL 3		/* Entries/line:  directory	*/
#define M2COL 16	/* Entries/line:  alloc map	*/

ptmap(disk, table)
int disk;
char *table;
{
	int count, i, j, k;
	int track, sector, d, ex;
	char dir[BBUFSIZ];	/* Current directory bloc. */
	int  map[MBUFSIZ];	/* The disk allocation map. */
	int *intdir, id;

	intdir = dir;

	/* Clear the map. */
	setmem(&map, 2*MBUFSIZ, 0);


	printf("The directory is...\n");

	track = sector = 0;
	for (count = i = 0; i < DIRSIZE; i++) {

		if (i%(4*SPB) == 0) {
			/* Read the next next block. */
			rdbuff	(dir, SPB, LOG, disk,
			0, track, sector, table);
			sector += SPB;
			if (sector >= SPT) {
				track++;
				sector -= SPT;
			}
			j = 0;
		}
		else {
			j += 32;
		}

		/* Skip never allocated entries completely. */
		if (dir [j+1] == 0xe5) {
			continue;
		}

		/* Print M1COL entries per line. */
		if (count%M1COL == 0) {
			putchar('\n');
		}
		count++;

		/* Print directory number. */
		printf("%3x", i);

		/* Print '=' for nondeleted extents */
		printf("%s",  dir[j]==0 ? " = " : " * ");

		/* Save extent number. */
		ex = dir [j + 12];

		/* Print the file name and extent. */
		dir [j + 12] = 0;
		printf("%s:%x  ", &dir [j+1], ex);

		/* Skip deleted blocks. */
		if (dir [j] == 0xe5) {
			continue;
		}

		/* See whether block # fits in 8 bits. */
		if (BPD <= 256){
			for(k = 16; k < 32 && dir [j+k]; k++) {
				d = dir [j + k];
				if (d) {
					map [d] = i;
				}
				else {
					break;
				}
			}
		}
		else {
			for(k = 8; k<16 && intdir[j/2 + k]; k++){
				id = intdir [j/2 + k];
				if (id) {
					map [id] = i;
				}
				else {
					break;
				}
			}
		}
	}

	printf("\n\nThe disk map is...\n\n");

	for(i = 0; i < BPD; i++) {

		if (i%M2COL == 0) {
			printf("%3x: ", i);
		}

		if (map [i]) {
			printf("%3x ", map [i]);
		}
		else {
			printf("... ");
		}

		if(i%M2COL == M2COL-1) {
			putchar('\n');
		}
	}
	putchar('\n');
}

#undef M1COL
#undef M2COL


/* Read a block or sector into the buffer. */

rdbuff(pntr, nsects, mode, disk, block, track, sector, table)
char *pntr;
int nsects, mode, disk, block, track, sector;
char *table;
{
	int i;
	int s;

	bios(SELDSK, disk);
	for (i = 0; i < nsects; i++, sector++){
		if (sector >= SPT) {
			track++;
			sector = 0;
		}
		if (track >= TPD) {
			printf("Read truncated!\n");
			return;
		}
		bios(SETTRK, track + (mode==LOG ? OFFSET : 0));
		s = bioshl(SECTRAN, sector, table);
		bios(SETSEC, s);
		bios(SETDMA, pntr + (i * SECSIZE));
		bios(RDSEC);
	}	
}


/*
	Select a drive for all future disk operations.
	drive is 0 for drive A:, 1 for drive B:, etc.
*/

select (drive)
char drive;
{
	int alloc, i, nsectors;

	printf("select drive = %c\n", drive + 'A');

	/* Point at the DPH and DPB. */
	d_header = bioshl(SELDSK, drive, 0);
	if (d_header == 0) {
		printf("Select failed!\n");
		exit();
	}
	d_disk = d_header -> d_DPB;

	/* calculate global constants for this drive */
	TXTABLE = d_header -> d_XLT;
	SPB = d_disk   -> d_BLM + 1;
	BPD = d_disk   -> d_DSM + 1;
	SPT	 = d_disk   -> d_SPT;
	OFFSET  = d_disk   -> d_OFF;

	/*
		Compute number of logical tracks on the disk.
		For hard disks nsectors can overflow.
		thus, we must use a for loop instead of:
	 */

	/* comment out -----
		nsectors = SPB * BPD;
		ntracks  = nsectors / SPT;
		if (nsectors % SPT != 0) {
			TPD++;
		}
	----- end comment out */

	nsectors = 0;
	TPD  = 1;
	for (i = 0; i < BPD; i++) {
		nsectors += SPB;
		if (nsectors > SPT) {
			nsectors -= SPT;
			TPD++;
		}
	}

	/* Add the number of hidden tracks. */
	TPD += OFFSET;

	/* Compute the number of blocks in the directory. */
	alloc = ((d_disk -> d_AL0) << 8) | (d_disk -> d_AL1);
	DIRBLKS = 0;
	for (i = 0; i < 16; i++) {
		if ((alloc & 1) != 0) {
			DIRBLKS++;
		}
		alloc = alloc >> 1;
	}

	/*
		Compute the number of entries in the directory.
		There are 4 entries in each 128-byte sector.
	*/

	DIRSIZE = DIRBLKS * SPB * 4;

	/* Make sure the directory buffer is big enough. */
	if (SPB * SECSIZE > BBUFSIZ) {
		printf("Block buffer is too small.\n");
		exit();
	}

	/* Make sure the map buffer is big enough. */
	if (DIRSIZE > MBUFSIZ) {
		printf("Map buffer is too small.\n");
		exit();
	}
}


/* Write the buffer to the disk. */

wrbuff(pntr, nsects, mode, disk, block, track, sector, table)
char *pntr;
int nsects, mode, disk, block, track, sector;
char *table;
{
	int i;
	int s;

	bios (SELDSK, disk);
	for (i = 0; i < nsects; i++, sector++) {
		if (sector >= SPT) {
			sector = 0;
			track++;
		}
		if (track >= TPD) {
			printf("Write truncated!\n");
			return;
		}
		bios(SETTRK, track + (mode==LOG ? OFFSET : 0));
		s = bioshl(SECTRAN, sector, table);
		bios(SETSEC, s);
		bios(SETDMA, pntr + (i * SECSIZE));

		/* Force immediate write. */
		bios(WTSEC, 1);
	}
}
able);
		bios(SETSEC, s);
		bios(SETDMA, pntr + (i * SECSIZE));

		/* Force immediate write. *