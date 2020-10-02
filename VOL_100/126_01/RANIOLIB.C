/*********************************************************************\
** .---------------------------------------------------------------. **
** |                                                               | **
** |                                                               | **
** |         Copyright (c) 1981, 1982, 1983 by Eric Martz.         | **
** |                                                               | **
** |                                                               | **
** |       Permission is hereby granted to use this source         | **
** |       code only for non-profit purposes. Publication of       | **
** |       all or any part of this source code, as well as         | **
** |       use for business purposes is forbidden without          | **
** |       written permission of the author and copyright          | **
** |       holder:                                                 | **
** |                                                               | **
** |                          Eric Martz                           | **
** |                         POWER  TOOLS                          | **
** |                    48 Hunter's Hill Circle                    | **
** |                      Amherst MA 01002 USA                     | **
** |                                                               | **
** |                                                               | **
** `---------------------------------------------------------------' **
\*********************************************************************/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

/*
#define DEBUG 1
*/

#include <bdscio.h>
#include "bdscio+.h"
#define BUF 512

/*	RANDOM I/O LIBRARY

		ranfget(buf,fp,offset,bytecnt)
		ranfput(
		ranrec(buf, rw, fp, record)
		seekend(fp,offset)
*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
/*
These functions must be used in conjunction with fio1.h, fio2.h, fio3.h,
and the renamed BDS library functions described therein.
See also a-open.c, getc.c, putc.c, rawgetc.c supplied on this disk.
*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
ranfget(buf,fp,offset,bytecnt)

/* THIS FUNCTION READS FROM THE FILE SPECIFIED BY fp A STRING
BEGINNING AT A RANDOM POSITION IN A FILE. THE OFFSET WHERE
READING BEGINS IS INDICATED BY offset, A POINTER TO A STRUCTURE
WHICH CONTAINS AN UNSIGNED seccnt AND AN INT charcnt (SEE
bdscio+.h). IF bytecnt IS 0, ranfget() STOPS AT THE NEXT NEWLINE
IN THE FILE, WHICH IS NOT PUT INTO BUF. IF bytecnt IS A POSITIVE
INTEGER, ranfget() READS THE SPECIFIED NUMBER OF
BYTES, */

/* ASSUMES INPUT FILE IS OPEN */

	char *buf;
	FILE *fp;
	struct foffset *offset;
	int bytecnt;
	{
	int check, i;
	seek(fp->_fd,offset->record,0);
	check = read(fp->_fd,fp->_buff,NSECTS);
	if (check == ERROR) {
		printf("DISK READ ERROR (ranfget)\n");
		exit(0);
	}
	fp->_nextp = fp->_buff + offset->byte;
	fp->_nleft = (check * SECSIZ) - offset->byte;
	if (!bytecnt) {
		check = fnnlgetl(buf,fp,BUF);
		if (!check) goto bad;
	}
	else {
		for (i=0; i<=bytecnt-1; i++) {
			check = rawgetc(fp);

/* rawgetc() IS EQUIVALENT TO BDSC getc() WHICH PASSES ALL
CHARACTERS THROUGH. I HAVE MODIFIED getc() TO RETURN EOF (-1) WHEN IT GETS
A ^Z (CPMEOF), IN ACCORD WITH STANDARD K&R C. */

			if (check EQ EOF) {
bad:
				fprintf(STDERR,
					"ATTEMPT TO READ PAST EOF (ranfget)\n");
				exit(0);
			}
			buf[i] = check;
		}
		/* NO NULL APPENDED TO buf HERE ON ASSUMPTION CALLING PROGRAM
		DOES NOT REQUIRE A NULL TERMINATED STRING */
	}
	return(0);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
ranfput(buf, fp, offset, bytecount)

/* PUTS bytecount BYTES (OR THE NULL-TERMINATED STRING IN buf IF
bytecount IS 0; NULL IS NOT WRITTEN INTO FILE) INTO FILE
BEGINNING AT THE RANDOM RECORD/BYTE SPECIFIED BY offset.

ASSUMES FILE IS OPEN, AND WILL BE CLOSED BY THE CALLING PROGRAM
AFTER ranfput RETURNS.
*/
	char *buf;
	FILE *fp;
	struct foffset *offset;
	int bytecount;
	{
	char recbuf[129];
	int i, recpos, atrec;

#ifdef DEBUG
printf("RANFPUT bytes %d:\n",bytecount);
for (i=0;i<=bytecount-1;i++)dispchar(buf[i]);
printf("\n");
#endif

	atrec = offset->record;
	ranrec(recbuf,'r',fp,atrec); /* read current record */

#ifdef DEBUG
printf("OLD RECORD FROM FILE:\n",bytecount);
for (i=0;i<RECSIZ;i++)dispchar(recbuf[i]);
printf("\n");
#endif

	recpos = offset->byte;
	if (bytecount) {
		for (i=0; i<bytecount; i++, recpos++) {
			if (recpos EQ 128) { /* this byte is first in next record */
				ranrec(recbuf,'w',fp,atrec);
				ranrec(recbuf,'r',fp,++atrec);
				recpos = 0;
			}
			recbuf[recpos] = buf[i];
		}
	}


	else {
		i = 0;
		while (buf[i]) {
			if (recpos EQ 128) { /* this byte is first in next record */
				ranrec(recbuf,'w',fp,atrec);
				ranrec(recbuf,'r',fp,++atrec);
				recpos = 0;
			}
			recbuf[recpos] = buf[i++];
			recpos++;
		}
	}
	ranrec(recbuf,'w',fp,atrec); /* flush final record */
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
ranrec(buf, rw, fp, record)

/* READS OR WRITES A RANDOMLY POSITIONED RECORD (128 BYTES
REGARDLESS OF DISK FORMAT) USING CP/M 2.2 BDOS CALLS.

rw MUST BE 'r' OR 'w'; NO ERROR CHECKING IS DONE. FILE MUST BE
OPEN BEFORE CALL AND MUST BE CLOSED AFTER ranrec WRITE CALLS.
*/
	char *buf, rw;
	FILE *fp;
	int record;
	{
	int *pi, c;
	char *pfcb; /* POINTER TO CP/M FILE CONTROL BLOCK */

	/* SET RANDOM RECORD NUMBER IN FCB */
	pfcb = fcbaddr(fp->_fd);
	pi = 33 + pfcb;
	*pi = record;

	/* SET SO-CALLED DMA ADDRESS */
	/* n.b.: if this is done before the above call to fcbaddr the
	function fails -- ?? */
	bdos(26,buf);

	if (rw EQ 'r') bdos(33,pfcb); /* READ */
	else bdos(34,pfcb);			/* OR WRITE */

	putchar('\0'); /* required to fix a bug in CP/M! */
	
	bdos(26,0x80); /* RESTORE SO-CALLED DMA ADDRESS TO ITS USUAL
				VALUE*/
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
seekend(fp,offset)

/* THE FILE MUST HAVE BEEN OPENED FOR APPEND (fopen(filename,
"a")) PRIOR TO CALLING seekend(). seekend() POSITIONS THE
POINTERS IN THE FILE BUFFER _buf SO THAT THE NEXT CHARACTER
WRITTEN WILL BE APPENDED TO EXISTING CONTENTS OF THE FILE, I.E.
WILL OVERWRITE THE PREVIOUS ^Z.

IN SOME APPLICATIONS, IT IS LATER DESIRED TO READ THE SAME STRING
RANDOMLY FROM THE FILE, SO THE OFFSET IS NEEDED. FOR THIS
PURPOSE, seekend() PROVIDES IN offset (SEE bdscio+.h) THE NEEDED
INFORMATION. ranfget() CAN LATER BE CALLED WITH THIS offset. */


	FILE *fp;
	struct foffset *offset;
	{
	char *fcb, *fcbaddr(), bdos();
	int i, c, getc(), *pi;
	fcb = fcbaddr(fp->_fd); /* GET POINTER TO FILE CONTROL BLOCK */

#ifdef DEBUG
showfcb(fcb);
#endif

	bdos(35,fcb); /* INSTALL RECORD COUNT IN FILE CONTROL BLOCK */

#ifdef DEBUG
showfcb(fcb);
#endif

	if (*(fcb+35) == 1) {
		printf("RECORD COUNT IS 65536;\n");
		printf("TOO LARGE FOR SEEKEND DURING OPEN FOR APPEND\n");
		exit(0);
	}

	/* GET COUNT OF FILLED SECTORS WRITTEN = TOTAL SECTORS - 1 */
	pi = fcb+33;
	offset->record = (*pi)-1;

	/* SEEK TO BEGINNING OF LAST SECTOR */
	seek(fp->_fd,offset->record,0);

	/* GET COUNT OF CHARACTERS IN LAST SECTOR BY LOOKING FOR CTRL-Z */
	fp->_nleft = 0; /* forces reading of new sector with getc() */
	for (i=1; i <= (SECSIZ) && ((c=getc(fp)) != EOF); i++);

	/* IN MY LIBRARY, getc() HAS BEEN MODIFIED TO RETURN EOF WHEN A CPMEOF
	(^Z) IS READ, SO AS TO BE CONSISTENT WITH K&R C */

	if (c == EOF) {
		ungetc(CPMEOF,fp); /* NOT c ! ALSO, SINCE THIS ASSIGNS CPMEOF
						TO *_nextp, GETC MUST INCREMENT _nextp
						EVEN ON EOF! */
		i -= 1;
	}
	offset->byte = i;

	/* ABOVE CALL TO getc() READS ONLY ONE SECTOR AND SETS *_nleft=128--;
	SINCE A BUFFER FOR WRITING ALWAYS HAS 1024 AVAILABLE (AND THIS IS
	ASSUMED BY fflush()) WE NEXT CORRECT NLEFT TO INDICATE TRUE ROOM
	REMAINING IN THE BUFFER */
	fp->_nleft += (NSECTS - 1) * SECSIZ;

	/* SEEK AGAIN SO THAT THE NEXT WRITE (BY THE PROGRAM WHICH CALLED
	seekend() WILL REWRITE THE PRESENT LAST SECTOR */
	seek(fp->_fd,offset->record,0);

#ifdef DEBUG
printf("fp=%u, offset=%u, record=%u, byte=%d\n",
fp, offset, offset->record, offset->byte);
#endif
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
/* END OF RANDOM I/O LIBRARY */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 
checkbig(m,s)
/* ROUTINE TO WRITE TO STDERR STRINGS TOO BIG FOR fprintf() */
	char *m, *s;
	{
	/* m IS FOR A MESSAGE */
	while (*m) putc(*m++,STDERR);
	if (*s) {
		putc('<',STDERR);
		while (*s) putc(*s++,STDERR);
		if (*(s-1) NE NEWLINE)
			fputs(">\n",STDERR);
	}
}
 ABOV