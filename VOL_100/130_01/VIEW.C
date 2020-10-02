/*
	VIEW - Disk Block Viewer for CP/M v2

	written for BDS "C" v1.44 by

	Rick Hollinbeck
	Box C
	Norwood, CO  81423

	v1.0 - 10/19/81
Note:
	This program depends on:
	 1. v2.x CP/M
	 2. a modified 'bios' function which leaves HL untouched
	 	for the seldsk function (9), but is otherwise compatible
		with the standard library version.
	 3. A CRT with addressable cursor & clear screen capabilities.
	 4. A 'bdscio.h' with the strings CLEARS (clear screen) and
	 	CURSADR (cursor addressing prefix to row, column) defined.
	 5. The direct bios disk i/o utilities (diskio.c & diskio.dcl)
*/

#include <a:bdscio.h>
#include <diskio.dcl>

#define ALLXOFF 5	/* Home Row for Allocation Map */
#define ALLYOFF 1	/* Home Column for Allocation Map */
#define MXBITPOS 63	/* Last bit position to display per row */
#define ALLYMAX (ALLYOFF + (MXBITPOS+1)/8 + MXBITPOS - 1)
			/* Max column for alloc. map */
#define MXBLOCK 0x4000	/* Maximum Block Size */
#define MAXROW TLENGTH	/* CRT Maximum row */
#define MAXCOL TWIDTH	/* CRT Maximum column */

/* CP/M 2.x Disk Parameter Tables */

struct dpb {	/* Disk Parameter Block */
	unsigned spt;	/* Sectors/track */
	char bsh;	/* block shift (3 = 1k, 4 = 2k, 5 = 4k, etc.) */
	char blm;	/* a bit mask 'bsh' bits wide */
	char exm;	/* extent mask (depends on dsm, 0 for 1k allocation) */
	unsigned dsm;	/* maximum block number for drive */
	unsigned drm;	/* maximum directory entry index */
	char al[2];	/* Directory Allocation Bitmap */
	unsigned cks;	/* size of checksum vector */
	unsigned off;	/* Number of reserved tracks */
	} *dpbp;	/* from dphp->hdpbp */

struct dph {	/* Disk Parameter Header */
	char *xltp;	/* pointer to sector translation table */
	int dscr[3];	/* BDOS scratch */
	char *dirbp;	/* Pointer to directory buffer */
	struct dpb *hdpbp; /* Disk Parameter Block pointer */
	char *csvp;	/* Pointer to checksum vector */
	char *alvp;	/* Pointer to Allocation Bitmap */
	} *dphp;	/* returned by seldsk */

unsigned bls;		/* block size (bytes) */
char curx, cury, bitpos, maxxpos, lstmxbit;
char scurx, scury;
char svcurx, svcury;
char i, j, *dbufp, *bufp1, lineno;
char *xltbp;
unsigned usedblks, block, ii, jj, maxtrk, maxsec;
struct dskadr dadr;
char c, cdisk, abort;
char ans[132];
char dbuf[MXBLOCK];		/* Block Buffer */

main()
{
strt:
	clearscrn();
	cursor(MAXROW/2, 0);
	printf("Disk Block Viewer");
	printf(" v1.0\n");

getdsk:
	printf("Enter Drive (A:, B:, etc.): ");
	gets(ans);
	bdos(14, c = (cdisk = toupper(ans[0])) - 'A');	/* Log in Drive */
	if (!(dphp = bios(SELDSK, c))) {
		printf("\n??? Select Error on %c:\n", cdisk);
		goto getdsk;
		}

	maxtrk = maxsec = (-1);	/* Disable diskio parameter checks */
	bitpos = 0;
	scurx = ALLXOFF;
	scury = ALLYOFF;
cloop:
	abort = FALSE;
	diskinfo();		/* Print Disk Characteristics */
	printbm();		/* Print allocation map */

  /* Block select loop */

	cursor(curx + 2, 3);
	printf("F - Fwd / B - Back / P - Up / N - Down / X - Examine");
	printf(" / S - Select Disk");
	printst();		/* Print sector translation table */
	cursor(scurx, scury);
	c = ' ';
	while (c != ('C'-0x40)) {	/* ^C terminates */

		block = (curx - ALLXOFF) * (MXBITPOS + 1) + bitpos;
		cursave();
		cursor(MAXROW - 1, MAXCOL - 30);
		printf("Block %u (dec) %x (hex) ", block, block);
		curback();

		switch (c = toupper(getch())) {

		case ('F'-0x40): /* ^F */
		case 'F':
			if (block < dpbp->dsm)
			 if (bitpos < MXBITPOS) {
				if ((bitpos & 7) == 7) ++cury;
				cursor(curx, ++cury);
				++bitpos;
				}
			 else { /* wrap to next line */
				cursor(++curx, ALLYOFF);
				bitpos = 0;
				}
			break;

		case ('B'-0x40): /* ^B */
		case 'B':
			if (bitpos) {
				if (!(bitpos & 7)) --cury;
				cursor(curx, --cury);
				--bitpos;
				}
			else { /* back up to previous line */
				if (curx > ALLXOFF) {
					cursor(--curx, ALLYMAX);
					bitpos = MXBITPOS;
				}}
			break;

		case ('N'-0x40): /* ^N */
		case 'N':
			if (curx < maxxpos) {
				cursor(++curx, cury);
				}
			break;

		case ('P'-0x40): /* ^P */
		case 'P':
			if (curx > ALLXOFF) {
				cursor(--curx, cury);
				}
			break;
			
		case ('X'-0x40): /* ^X */
		case 'X':
			/* Get the block & Display it */
			scurx = curx;
			scury = cury;
			cursor(MAXROW - 3, 0);
			readblk(block);
			for (dbufp = dbuf; dbufp < &dbuf[bls]; ++dbufp) {
				if ((c = *dbufp) == CPMEOF) break;
				if (c == '\r') continue;
				if (c == '\n') continue;
				if (c == '\t') continue;
				if (!printable(c)) {
					cursor(curx + 2, 0);
					printf("Looks binary, interested? ");
					if (!yes()) goto cloop;
					dmphex();
					goto cxdone;
					}
				}
			cursor(curx + 2, 0);
			printf("Looks like ASCII, Do you want hex display anyway? ");
			if (yes()) dmphex();
			else dmpascii();
		 cxdone:
			if (!abort) {
				newline();
				pause();
				}
			goto cloop;

		 case ('S'-0x40):
		 case 'S':
			goto strt;
		}
	}

	cursor (MAXROW - 2, 0);
	exit();
	}

diskinfo()
{

  /* Print Disk Info */

	dpbp = dphp->hdpbp;
	printf("%sDrive %c Characteristics:\n", CLEARS, cdisk);
	printf("Sectors per Track: %u  ", dpbp->spt);
	/* Calc block size */
	printf("Block Size: %u  ",
	bls = SECSIZ << dpbp->bsh);
	printf("Max Block: %u  ", dpbp->dsm);
	printf("Disk Size: %uk\n",
		dpbp->dsm << (dpbp->bsh - 3));
	printf("Reserved Tracks: %u", dpbp->off);
	}

readblk(blk)
unsigned blk;
{
	/* Determine the block's disk address */
	dadr.track = (jj = (ii = bls/SECSIZ) * blk) / dpbp->spt + dpbp->off;
	jj = jj % dpbp->spt;
	dadr.sector = physec(jj);
	clearscrn();
	printf("Reading Track - Sector\n");
	dbufp = dbuf;
	while (ii--) {
		cursor(++curx,10); printf("%u", dadr.track);
		cursor(curx, 18); printf("%u", dadr.sector);
		diskio(READ, &dadr, dbufp, 1, maxtrk, maxsec);
		dbufp += SECSIZ;
		if (++jj == dpbp->spt) {
			jj = 0;
			++dadr.track;
			}
		dadr.sector = physec(jj);
		}
	}

unsigned physec(logsec)
unsigned logsec;
{
	return dphp->xltp ? *(dphp->xltp + logsec) : (logsec + 1);
	}

printbm()
{
	char abyt, bmsk, bit, *almapp;
	unsigned bcnt;

	cursor(ALLXOFF - 2, (MAXCOL/2 - 12));
	printf("Allocation Bit Map\n");
	cursor(maxxpos = (ALLXOFF - 1), ALLYOFF);
	printf("0      7 8     15 16    23 24    31 32    39 40    47");
	printf(" 48    55 56    63");
	almapp = dphp->alvp;	/* point to allocation map */
	usedblks = 0;
	for (bcnt = 0; bcnt <= dpbp->dsm; ++bcnt) {
		if (bcnt & 7) { /* shift to next bit */
			bit = ((abyt & (bmsk >>= 1)) != 0);
			}
		else { /* get next byte from vector */
			bit = (((abyt = *almapp++) & (bmsk = 0x80)) != 0);
			if (!(bcnt & 0x3f)) {
				++maxxpos;
				lstmxbit = 0;
				cursor(++curx, ALLYOFF - 1);
				}
			printf(" ");
			}
		++lstmxbit;
		printf("%u",bit);
		usedblks += bit;
		}
	--lstmxbit;

	/* Print total allocated storage */
	cursave();
	cursor(ALLXOFF - 3, MAXCOL - 22);
	printf("Blks Used: %u", usedblks);
	curback();
	}

printst()
{
	cursor(curx + 2, (MAXCOL/2 - 12));
	printf("Sector Translation");
	cursor(++curx, 0);
	xltbp = dphp->xltp;
	for (i = j = 0; i < dpbp->spt; ++i) {
		if (j++ == (MAXCOL/8)) {
			cursor(++curx, 0);
			j = 1;
			}
		printf("%u:%u ", i+1, *xltbp++);
		cursor(curx, cury + 8);
		}
	}

dmphex()
{
  clearscrn();
  ii = 0;
  for (dbufp = dbuf; dbufp < &dbuf[bls];) {
    newline();
    if (abort) return;
    printf("%c%c%c0: ", hexd(ii>>8),
    			hexd(ii>>4), hexd(ii));
    ++ii;
    bufp1 = dbufp;
    for (j=0; j<16; ++j)
    {
     printf("%c%c ", hexd(*dbufp>>4),
                     hexd(*dbufp));
     ++dbufp;
    }
    printf(" |");
    dbufp = bufp1;
    for (j=0; j<16; ++j) {
	putchar( printable(*dbufp) ? *dbufp : '_');
	++dbufp;
    }
    printf("|");
   }
}

dmpascii()
{
	clearscrn();
	for (dbufp = dbuf; dbufp < &dbuf[bls]; ++dbufp) {
		if ((c = *dbufp) == CPMEOF) break;
		if (c == '\n') newline();
		else putchar(c);
		if (abort) return;
		}
	}

char hexd(dbyte)
char dbyte;
{
  return
    ( ((dbyte = dbyte & 0x0f) > 9) ? (dbyte + ('A'-10)) :
                                   (dbyte + '0') );
}

char yes()
{
	return tolower(getchar()) == 'y';
	}

char printable(chr)
char chr;
{
 if (chr & 0x80) return 0;
 if (chr < ' ') return 0;
 if (chr == 0x7f) return 0;
 return 1;
}

clearscrn()
{
	curx = cury = lineno = 0;
	printf("%s", CLEARS);
	}

newline()
{
	printf("\n");
	if (++lineno > (MAXROW-2)) pause();
	}

pause()
{
	printf("Hit a key to continue (^G cancels dump) ...");
	if (getch() == ('G'-0x40)) abort = TRUE;
	clearscrn();
	}

cursor(x,y)
char x,y;
{
	printf("%s%c%c", CURSADR, (curx = x) + ' ', (cury = y) + ' ');
	}

cursave()
{
	svcurx = curx; svcury = cury;
	}

curback()
{
	cursor(svcurx, svcury);
	}

char getch()	/* no echo input */
{
	return bios(CONIN);
	}
bufp) {
		if ((c = *dbufp) == CPMEOF) break;
		if (c == '\n') newline();
		