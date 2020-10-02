/********************************************************************
	ry.c - library of file handlers
*/

/* changes */

/*
	following applies to rx package, father of ry:

	7/25/81- put this file on seperate development disk, ALL 
	development to this package to be done here only. crl files
	may be spread around as needed.

	7/7/81-	made ropen() free allocated buffer space before
	returning on ERROR. Don't close files that return ERROR
	from ropen().

	7/7/81- made rclose do primitive close even if buffer not
	flushed correctly, insuring that fd is always freed.

	8/18/81- corrected to be grammatically identical to unix
	call syntax (almost, still additional arg to ropen and rcreat
	to explicitly set file buffer size).

	2/14/82- add stderr, virtual console, virtual modem, and
	vcon & vmodem to rgetc() and rputc().

	3/16/82- remove virtual stuff.

	5/30/82 - remove 'byte' and 'string' defines from code

	5/30/82 - change all 'FILE' to 'RFILE' for ver 1.46

	6/7/82 - fix ropen() setting of fp->_bufbase to '= fp + 1'

	7/14/82 - fix rclose to free memory AFTER using fd to free file

	9/9/82 - add bs (binary search file) code to package.  got it from
				bs.c, not fron cnlib, will save cnlib for a while...
			no, maybe I did get it from cnlib....

	end of comments/changes on rx stuff

*/

/*
	following applies to ry package:

	9/11/82 - change over to ry disk, rename rx files to ry for starters, etc.
				change fseek() and ftell() to work with 'long' int
				rename them to lseek() and ltell(), respectively.
*/

#include "a:std.h"		/* bdscio.h + my stuff */
#include <ry.h>			/* stuff for this package */

/*
	purge any old copies, creat and open 'filename', buffer
size equal to secs * size of one sector, mode of write (from
beginning of file), append (write at end of file), or direct
(read or write, starting at beginning of file, random access
to any part of file). read not allowed as you can't read a
new file.
*/

RFILE *		/* returns pointer to RFILE struct, or ERROR on error */
rcreat(filename, mode, secs)
char *filename;
char mode;
int secs;
{
	int fd;
	char dummy_h[SECSIZ];
	RFILE *ropen();

	setmem(dummy_h, SECSIZ, 0);								/* nulls */
	switch (mode) {
	case 'r':	return ERROR;						/* can't read empty file */
	case 'w':
	case 'a':
	case 'd':
		if ((fd = creat(filename)) == ERROR ) return ERROR;
		if (write(fd, dummy_h, 1) != 1) {
			fabort(fd);
			return ERROR;									/* dummy header */
		}
		if (close(fd) == ERROR) return ERROR;				/* free up fd */
		return ropen(filename, mode, secs);					/* open */
	default:
		return ERROR;
	}
}
/*
	open 'filename' buffer in space provided by alloc(), setup
fp, buffer of size SECSIZ * secs
*/

RFILE *		/* returns pointer to RFILE or ERROR on error */
ropen(filename, mode, secs)
char *filename;
char mode;
int secs;
{
	RFILE *fp;
	int fd;
	char *alloc();
	unsigned rcfsiz(), rsrec();

	switch (mode) {
	case 'r':							/* read mode */
/*		fd = open(filename, 0);			/* open read */
		break;
*/
	case 'w':							/* write mode */
/*	case 'a':							/* append mode */ not supported yet */
	case 'd':							/* direct or random */
		fd = open(filename, 2);			/* open r/w */
		break;
	default: return ERROR;
	}
	if (fd == ERROR) return ERROR;		/* open error */

	if ((fp = alloc(((secs + 1) * SECSIZ) + sizeof(*fp))) == 0) return ERROR;
									/* no room for buffer */
	fp -> _rfd = fd;							/* file descriptor */
	fp -> _bufbase = fp + 1;					/* start of file buffer */
	fp -> _bufbase += SECSIZ;					/* room for header */
	fp -> _secs = secs;							/* secs. in buffer */
	fp -> _nxtbyt = fp -> _bufbase;	  			/* start at begin of buf */
	fp -> _pastbuf = fp -> _bufbase + (secs * SECSIZ); /* endbuf */
	fp -> _mode = mode;
/** 'a' not supported yet...
	fp -> _frstsec = (mode == 'a') ?    		/* if 'a' start at end */
		rcfsiz(fp->_rfd) : rsrec(fp->_rfd); 	/* else at 0 */
**/
	if (read(fd, fp + 1, 1) < 0) {				/* get file header into core */
		free(fp);
		return ERROR;
	}
	fp -> _frstsec = rsrec(fp->_rfd);			/* start at sector 1 */
	if (_fillbuf(fp) == ERROR ) {				/* fill the buffer */
		free(fp);
		return ERROR;
	}
	return fp;
}

/*
	flushes, then closes the random file at fp
*/

rclose(fp)			/* return -1 on error */
RFILE *fp;
{
	int err_flag;

	err_flag = _rflush(fp);									/* flush buffer */
	if (seek(fp->_rfd, 0, 0) == ERROR) err_flag = ERROR;	/* get to sec 0 */
	if (write(fp->_rfd, fp+1, 1) != 1) err_flag = ERROR;	/* update header */
	if (close(fp->_rfd) == ERROR) err_flag = ERROR;			/* close it */
	free(fp);											/* free buffer space */
	return err_flag;									/* did it flush ok? */
}
/*
	fills buffer pointed at by fp->_bufbase, with file fp->_rfd,
with fp->_secs sectors, starting with cp/m random record field
lseek calls this after updating cp/m's rrf and fp's pointers (if
necessary because seek is out of buffer area)
*/

_fillbuf(fp)			/* return -1 on error, 0 if OK */
RFILE *fp;
{
	int fd, secs, got;
	char *base;
	unsigned rseek();

	fd = fp -> _rfd;
	secs = fp -> _secs;
	base = fp -> _bufbase;

/* call rread for sectors till buffer full or ERROR */
	/* try to read all of buffer */
	while  (secs -= (got = rread(fd, base, secs))) {
		if ((got > 1000) OR (got == -1)) return ERROR;
								/* unwritten random sector in buffer area */
		setmem((base += (got * SECSIZ)), SECSIZ, 0);
											/* fill this sector with 0's */
		base += SECSIZ;						/* inc. buffer ptr */
		rseek(fd, 1, 1);					/* inc. file ptr */
		--secs;								/* this one filled with 0 */
	}										/* try filling rest of buffer */
	fp -> _update = NO;						/* clear update flag */
	return OK;
}
/*
	flushes the buffer fp->_bufbase if open for writing AND
fp->_update shows the buffer has indeed been written to since
last _fillbuf call. ignores flushes of i/o devices
*/

_rflush(fp)			/* return -1 on error, 0 if OK */
RFILE *fp;
{
	unsigned rseek();

	if ((fp < 4) OR (fp->_mode == 'r') OR (fp->_update == NO))
		return OK;											/* don't bother */
	if (rseek(fp -> _rfd,fp -> _frstsec, 0) == ERROR) return ERROR;
										/* seek first bufferd sec and ... */
	if (rwrite(fp->_rfd, fp->_bufbase, fp->_secs) != fp->_secs)
		return ERROR;					/* ...write entire buffer to disk */
	fp -> _update = NO;					/* clear update flag */
	return OK;
}
/*
	sets '_nxtbyt' of 'fp' to 'offset' (a long int) from
'origin' , returns offset.
*/

unsigned    /* returns (long int) offset, -1 on error */
lseek(fp, offset, origin)
RFILE *fp;
char *offset;
int origin;
{
	unsigned sector, byt;
	char longint[4], os[4], os2[4], lsecsiz[4];
	unsigned rseek(), rcfsiz();
	char *ltell();

	itol(lsecsiz, SECSIZ);					/* set logical sector size */

	if (origin == 0) {							/* from beginning of file */
		movmem(offset, os, 4);					/* make local copy */
	}
	else if (origin == 1) {
		ladd(os, offset, ltell(fp, longint));	/* pres pos + offset to os */
	}
	else if (origin == 3) {						/* ascii offset from start */
		atol(os, offset);
	}
	else if (origin == 4) {						/* ascii from pres pos */
		atol(os2, offset);
		ladd(os, os2, ltell(fp, longint));		/* pres pos + offset to os */
	}
	else return ERROR;							/* only legal orgs now */
	if (os[3] | os[2]) return ERROR;			/* within legal range? */
	ltou(&sector, ldiv(longint, os, lsecsiz));	/* byte / secsiz = sectors */
	ltou(&byt, lmod(longint, os, lsecsiz));		/* remainder = bytes */
	++sector;									/* reserve sec 0 for future */

	if ((sector >= fp->_frstsec) AND				/* if in buffer */
		(sector < (fp->_frstsec + fp->_secs))) {
		fp->_nxtbyt = (sector - fp->_frstsec)*SECSIZ
			+ byt + fp->_bufbase;					/* point to it */
		return offset;								/* and return */
	}
	if (_rflush(fp) == ERROR) return ERROR;			/* else flush buf */
	if (rseek(fp->_rfd, sector, 0) == ERROR) return ERROR;
	fp->_frstsec = sector;							/* seek sec in file, */
	fp->_nxtbyt = byt + fp->_bufbase;				/* and init ptrs */
	if (_fillbuf(fp) == ERROR) return ERROR;		/* fill buf */
	return offset;									/* and return */
}
/*
	returns ptr to long int that addresses current file pos.,
or -1 on error.

*/

char *    /* returns ptr to long int */
ltell(fp, longint)
RFILE *fp;
char *longint;
{
	char lb1[4], lb2[4], lfrstsec[4], lsecsiz[4];

	itol(lsecsiz, SECSIZ);					/* set logical sector size */

	utol(lfrstsec, (fp->_frstsec - 1));		/* sub for header */
	lmul(lb1, lfrstsec, lsecsiz);			/* times size of sector */
	utol(lb2, fp->_nxtbyt - fp->_bufbase);	/* bytes in buffer to _nxtbyt */
	return ladd(longint, lb1, lb2);			/* add the two */
}

/*
	seek a current byte that equals the block size * block #,
	return the current block number or ERROR
	max block # is 65534, large block sizes might cause
	arithmatic overflow before this value is reached
*/

unsigned		/* returns cur. addressed block, -1 on error */
bseek(fp, block, origin)
RFILE *fp;
unsigned block;
int origin;
{
	unsigned sector;
	char byt;
	int r1, r2, blocksiz;
	char *lseek();
	char longint[4], lblock[4], lbsize[4];

	switch (origin) {
	case 0:	break;							/* from begin */
	case 1:	block += fp->_curblk;			/* from curr pos */
		break;
	case 2:									/* from end not supported yet */
	default:
		return ERROR;
	}
	if (block < 0) return ERROR;			/* test under range */
	utol(lblock, block);					/* make it long int */
	itol(lbsize, fp->_blksiz);
	lmul(longint, lblock, lbsize);
/**
	blocksiz = fp->_blksiz;
	sector = (blocksiz/128)*block + (r1=(blocksiz%128))*(block/128)
		    + (r1*(r2=(block%128)))/128; /* need LONG INT */
	byt = r1 * r2 % 128;		/* hope this is right */
**/
	if (lseek(fp, longint, 0) == ERROR) return ERROR;
	return (fp->_curblk = block);
}
/*
	returns the current randomly addressed block
	that is, the last random block sought by
	bseek(), may not reflect _cursec and _curbyt
	if reads or writes have been done since bseek()
*/
unsigned				/* returns current log block */
btell(fp)
RFILE *fp;
{
	return (fp->_curblk);
}

/*
	returns a char from random buffered file 'fp'
*/

int			/* returns -1 on error */
rgetc(fp)
RFILE *fp;
{
	if (fp == 0) return getchar();				/* from con */
	if (fp == 3) return bdos(3);				/* from reader */
/*
	if (fp == 5) return cgetchar();				/* to virtual console */
	if (fp == 6) return mgetchar();				/* to virtual modem */
	if (fp == 7) return dgetchar();				/* to both vc & vm */
*/
	if (fp->_pastbuf - fp->_nxtbyt)  return *fp->_nxtbyt++;
												/* in buffer */
	if (lseek(fp, "0", 4) == ERROR) return ERROR;
				/* else seek one byte beyond buf, forcing flush and fill */
	return *fp->_nxtbyt++;						/* now get it from buffer */
}

/*
	random buffered 'unget' a char. Only one call between
	'rgetc()' calls
*/

int			/* returns OK, -1 on error */
rungetc(c, fp)
char c;
RFILE *fp;
{
	if (fp == 0) return ungetch(c);			/* con */
	lseek(fp, "-1", 4);						/* back up 1 char */
	rputc(c, fp);							/* unget the arg */
	lseek(fp, "-1", 4);						/* prepare to get it */
/**
	if (fp->_nxtbyt <= fp->_bufbase) return ERROR;
											/* can't push past begin */
	*--fp->_nxtbyt = c;						/* do it */
**/
	fp->_update = YES;
	return OK;
}

/*
	puts the char 'c' into random buffered file 'fp'
*/

int			/* returns -1 on error */
rputc(c, fp)
char c;
RFILE *fp;
{
	if (fp->_mode == 'r') return ERROR;			/* cant put in read file */
	if (fp == 1) return putchar(c);				/* to con */
	if (fp == 2) return bdos(5,c);				/* to list */
	if (fp == 3) return bdos(4,c);				/* to punch */
	if (fp == 4) {
		if (c == '\n') bdos(2,'\r');
		return bdos(2,c);
	 }	
/*
	if (fp == 5) return cputchar(c);			/* to virtual console */
	if (fp == 6) return mputchar(c);			/* to virtual modem */
	if (fp == 7) return dputchar(c);			/* to both vc & vm */
*/
	if (fp->_pastbuf - fp->_nxtbyt) {			/* in buffer */
		fp->_update = YES;		  				/* set update */
		return *fp->_nxtbyt++ = c;				/* write it */
	}
	if (lseek(fp, "0", 4) == ERROR) return ERROR; /* flush & fill */
	fp->_update = YES;			   				/* set update */
	return *fp->_nxtbyt++ = c;					/* write it */
}
/*
	returns a 16 bit word from file 'fp'
*/

int			/* returns -1 on error */
rgetw(fp)
RFILE *fp;
{
	int a,b;	
	if (((a=rgetc(fp)) >= 0) && ((b= rgetc(fp)) >=0))
			return 256*b+a;
	return ERROR;
}
/*
	writes the 16 bit word 'w' to file 'fp'
*/

int			/* returns -1 on error */
rputw(w, fp)
int w;
RFILE *fp;
{
	if ((rputc(w % 256,fp) >=0 ) && (rputc(w / 256,fp) >= 0))
				return w;
	return ERROR;
}

/*
	gets a TRUE c string (null term.) from file 'fp'
into 'str', returns ptr to 'str'

*/

char *					/* returns -1 on error */
rgets(str, fp)
char *str;
RFILE *fp;
{
	int count, c;
	char *cptr;
	count = MAXLINE;
	cptr = str;
	while ((count-- - 1) && (c=rgetc(fp)) != EOF && c ) {
		*cptr++ = c;				/* stuff it */
	}
	if (c == EOF) return ERROR;
	*cptr = '\0';					/* null */
	return str;
}

/*
	puts the null terminated c 'str' into 'fp', including null
*/

rputs(str, fp)			/* returns OK, -1 on error */
char *str;
RFILE *fp;
{
	do {
		if (rputc(*str, fp) == ERROR) return ERROR;
	} while (*str++);
	return OK;
}

/*
	gets the LF terminated text from 'fp' into 'line',
stripping off CRs from CR-LF combos

*/

char *			/* returns NULL on error */
rgetl(line, fp)
char *line;
RFILE *fp;
{
	int count, c;
	char *cptr;
	count = MAXLINE;
	cptr = line;
	if ((c = rgetc(fp)) == CPMEOF || c == EOF) return NULL;

	do {
		if ((*cptr++ = c) == '\n') {
		  if (cptr>line+1 && *(cptr-2) == '\r')
			*(--cptr - 1) = '\n';
		  break;
		}
	 } while (count-- && (c=rgetc(fp)) != EOF && c != CPMEOF);

	if (c == CPMEOF) rungetc(c, fp);	/* push back control-Z */
	*cptr = '\0';
	return line;
}

/*
	puts string at 'line' into 'fp', converting LFs to CR-LFs
if no LF exists no terminator (crlf) is written (ala fputs)

*/

rputl(line, fp)			/* returns OK, -1 on error */
char *line;
RFILE *fp;
{
	char c;
	while (c = *line++) {
		if (c == '\n') {
			if (rputc('\r', fp) == ERROR) return ERROR;
		}
		if (rputc(c, fp) == ERROR) return ERROR;
	}
	return OK;
}

/*
	gets 'nbyts' into memory starting at 'destination' from
file in 'fp'
*/

char *				/* returns -1 on error */
rget(fp, destination, nbyt)
RFILE *fp;
char *destination;
unsigned nbyt;
{
	char b, *ptr;

	ptr = destination;
	while (nbyt--) {
		if ((b = rgetc(fp)) == ERROR) return ERROR;
		*ptr++ = b;
	}
	return destination;
}
/*
	puts 'nbyts' into file 'fp' from 'source'
*/

rput(fp, source, nbyt)			/* returns OK, -1 on error */
RFILE *fp;
char *source;
unsigned nbyt;
{
	while (nbyt--) {
		if (rputc(*source++, fp) == ERROR) return ERROR;
	}
	return OK;
}

/*
    gets struct 'structsiz' long into memory starting at 'struct_add'
from file in 'fp'
*/

char *					/* returns -1 on error */
rgetstruct(fp, struct_add, structsiz)
RFILE *fp;
char *struct_add;
unsigned structsiz;
{
	char b, *ptr;

	ptr = struct_add;
	while (structsiz--) {
		if ((b = rgetc(fp)) == ERROR) return ERROR;
		*ptr++ = b;
	}
	return struct_add;
}
/*
	puts struct 'structsiz' long into file 'fp' from 'struct_add'
*/

rputstruct(fp, struct_add, structsiz)	/* returns OK, -1 on error */
RFILE *fp;
char *struct_add;
unsigned structsiz;
{
	while (structsiz--) {
		if (rputc(*struct_add++, fp) == ERROR) return ERROR;
	}
	return OK;
}
/*
	printf to random buffered file
*/

int			/* returns OK, -1 on error */
rprintf(fp, format)
RFILE *fp;
char *format;
{
	char text[MAXLINE];
	_spr(text,&format);
	return rputl(text,fp);
}

/*
	scanf to random buffered file
*/

int			/* returns # of successful matches, 0 on EOF */
rscanf(fp, format)
RFILE *fp;
char *format;
{
	char text[MAXLINE];
	if (!rgetl(text,fp)) return 0;
	return _scn(text,&format);
}

/* end of the random buffered file code.... */


/* start of bs code... */

bsmake(data_file, ptr_file, data_size, max_data)
char *data_file;
char *ptr_file;
int data_size;
int max_data;
{
	RFILE *fp;

	if (data_size < 8) return ERROR;				/* (to hold four ints) */
	if ((fp = rcreat(data_file, 'd', 1)) == ERROR)
		return ERROR;								/* creat a data file */
	rputw(0, fp);								/* 0 data elements in file */
	rputw(0, fp);								/* empty free list */
	rputw(data_size, fp);						/* block size of data file */
	rputw(max_data, fp);						/* max elements in data file */
		/* note: core must be able to hold twice this number of bytes. kinda
			limited but for files with a few thousand elements i suppose it
			will do.
		*/

	rclose(fp);										/* close datafile */
	if ((fp = rcreat(ptr_file, 'd', 1)) == ERROR)		/* creat a ptr file */
		return ERROR;
	rputw(0, fp);						/* write 0 to ptr file, not sure why */
	rclose(fp);							/* close ptr file */
}

bsopen(data_file, ptr_file, key_comp, secs)
char *data_file;
char *ptr_file;
int (*key_comp)();
int secs;
{
	BSFILE *bsp;
	RFILE *fp;
	int array_size;

	if ((fp = ropen(data_file, 'd', secs)) == ERROR)	/* open data file */
		return ERROR;
	lseek(fp, "6", 3);							/* find fourth int in file */
	array_size = rgetw(fp);						/* max elements in data file */
	array_size = (((array_size * 2) + (SECSIZ - 1)) / SECSIZ); /* sectors */
	if ((bsp = alloc(array_size * SECSIZ + sizeof(*bsp))) == 0) {
		rclose(fp);
		return ERROR;
	}
							/* init struct bs */
	bsp->_dfp = fp;						/* fp to 'data_file' */
	strcpy(bsp->_pfname, ptr_file);		/* save the name for closing */
	bsp->_key_comp = key_comp;			/* pointer to appropriate comp funct */
	lseek(fp, "0", 3);					/* go to beginning of file */
	bsp->_data_num = rgetw(fp);			/* number of data items in file */
	rgetw(fp);							/* skip free list */
	fp->_blksiz = bsp->_data_size = rgetw(fp);
						/* size in bytes of a data item, set up block seeks */
	bsp->_max_data = rgetw(fp);			/* maximum data elements in ptr file */
	bsp->_data_ptr = 0;					/* set by last call to search() */
	bsp->_ptr_array = bsp + 1;			/* base of pointer array */
	if (xswapin(ptr_file, bsp->_ptr_array, array_size) == ERROR) {
		rclose(fp);						/* fill the ptr array */
		free(bsp);
		return ERROR;
	}
	return bsp;
}

bsclose(bsp)
BSFILE *bsp;
{
	xswapout(bsp->_pfname, bsp->_ptr_array, 
		((bsp->_max_data * 2) + (SECSIZ - 1)) / SECSIZ);		 /* sectors */
	lseek(bsp->_dfp, "0", 3);				/* seek beginning of data file */
	rputw(bsp->_data_num, bsp->_dfp);		/* and write data_num to it */
	rclose(bsp->_dfp);						/* close data file */
	free(bsp);
}

bsread(bsp, key, address)
BSFILE *bsp;
char *key;
char *address;
{
	int result;

	if (key[0]) {					 		/* if key is given, search first */
		switch (bssearch(bsp, key)) {
		case BELONGS:	return BELONGS;
		case FOUND:		rget(bsp->_dfp, address, bsp->_data_size);
						return FOUND;
		case ERROR:
		default:		return ERROR;
		}
	}
	else {									/* seek the proper block */
		if (bseek(bsp->_dfp, bsp->_data_ptr, 0) == ERROR)
			return ERROR;
		rget(bsp->_dfp, address, bsp->_data_size);
		return OK;
	}
}

bswrite(bsp, key, address)
BSFILE *bsp;
char *key;
char *address;
{
	int result;

	if (key[0]) {					 		/* if key is given, search first */
		switch (bssearch(bsp, key)) {
		case BELONGS:	if (bskey_add(bsp) == ERROR) return ERROR;
						rput(bsp->_dfp, address, bsp->_data_size);
						return BELONGS;
		case FOUND:		rput(bsp->_dfp, address, bsp->_data_size);
						return FOUND;
		case ERROR:
		default:		return ERROR;
		}
	}
	else {									/* seek the proper block */
		if (bseek(bsp->_dfp, bsp->_data_ptr, 0) == ERROR)
			return ERROR;
		rput(bsp->_dfp, address, bsp->_data_size);
		return OK;
	}
}

bssearch(bsp, key)
BSFILE *bsp;
char *key;
{
	int bottom, middle, top, result;
	char block_key[MAXLINE];

	bottom = middle = 1; top = bsp->_data_num;
	while (top >= bottom) {						/* till tail passes head... */
		middle = (top + bottom) / 2;					/* find midpoint */
		if (bseek(bsp->_dfp, bsp->_ptr_array[middle], 0) == ERROR)
				return ERROR;							/* record it pts to */
		rgets(block_key, bsp->_dfp);					/* read the key */
		result = (*bsp->_key_comp)(key, block_key);		/* compare them */
		if (result > 0) bottom = ++middle;		/* it would go in NEXT slot */
		else if (result < 0) top = middle - 1;	/* it would go HERE */
		else {				/* set _data_ptr to log. block containing record */
			if (bseek(bsp->_dfp, bsp->_data_ptr = bsp->_ptr_array[middle], 0)
				== ERROR) return ERROR;				/* seek the proper block */
			bsp->_array_index = middle;
			return FOUND;
		}
	}
	bsp->_array_index = middle;						/* not found, */
	return BELONGS;									/* belongs here */
}

bskey_add(bsp)
BSFILE *bsp;
{
  	if (++bsp->_data_num > bsp->_max_data) {
		--bsp->_data_num;
		return ERROR;								/* no room */
	}
	if (bsblk_add(bsp) == ERROR) {
		--bsp->_data_num;
		return ERROR;
	}
	if (bseek(bsp->_dfp, bsp->_data_ptr, 0) == ERROR)
		return ERROR;
	movmem(&bsp->_ptr_array[bsp->_array_index],		/* open hole */
		&bsp->_ptr_array[bsp->_array_index + 1],
		(bsp->_data_num - bsp->_array_index) * 2);
	bsp->_ptr_array[bsp->_array_index] = bsp->_data_ptr; /* add ptr */
	return OK;
}

bsblk_add(bsp)
BSFILE *bsp;
{
	int free_ptr;

	lseek(bsp->_dfp, "2", 3);			/* find second int in file */
	if (!(bsp->_data_ptr = rgetw(bsp->_dfp))) {	/* free list ptr */
		bsp->_data_ptr = bsp->_data_num;		/* nope, end block */
	}
	else {
		if (bseek(bsp->_dfp, bsp->_data_ptr, 0) == ERROR)
			return ERROR;
		free_ptr = rgetw(bsp->_dfp);
		lseek(bsp->_dfp, "2", 3);		/* find second int in file */
		rputw(free_ptr, bsp->_dfp);		/* record top of free list */
	}
	return OK;
}

bskey_rmv(bsp, key)
BSFILE *bsp;
char *key;
{
	int result;

	if ((result = bssearch(bsp, key)) != FOUND) return result;
	movmem(&bsp->_ptr_array[bsp->_array_index + 1],		/* close hole */
		&bsp->_ptr_array[bsp->_array_index],
		(bsp->_data_num - bsp->_array_index) * 2);
	--bsp->_data_num;								/* decrement the count */
	bsblk_rmv(bsp);								/* return to free list */
	return OK;
}

bsblk_rmv(bsp)
BSFILE *bsp;
{
	int free_ptr;

	lseek(bsp->_dfp, "2", 3);				/* find second int in file */
	free_ptr = rgetw(bsp->_dfp);			/* read free ptr from data file */
	if (bseek(bsp->_dfp, bsp->_data_ptr, 0) == ERROR)
		return ERROR;						/* seek freed block */
	rputw(free_ptr, bsp->_dfp);				/* write free ptr to freed block */
	lseek(bsp->_dfp, "2", 3);				/* find second int in file */
	rputw(bsp->_data_ptr, bsp->_dfp);
									/* write block to data file (free ptr) */
	return OK;
}

/* end of the bs code.... */

/*
	these 2 are an expansion of the standard 'swapin()', allow
	the size of core to be specified, insuring that other things
	wont be clobbered.  a 'swapout()' has been added for updating
	the file back to disk
*/

xswapin(name, addr, sectors)
char *name;		/* the file to swap in */
unsigned addr;
int sectors;
{
	int fd, x;
	if (( fd = open(name,0)) == ERROR) {
		return ERROR;
	}
	if ((x = read(fd,addr,sectors)) < 0) {
		close(fd);
		return ERROR;
	}
	close(fd);
	return OK;
}

/* add temp file and rename someday.... */

xswapout(name, addr, sectors)
char *name;		/* the file to swap out */
unsigned addr;
int sectors;
{
	int fd, x;
	if (( fd = open(name,1)) == ERROR) {
		return ERROR;
	}
	if ((x = write(fd,addr,sectors)) < 0) {
		close(fd);
		return ERROR;
	}
	close(fd);
	return OK;
}
>_dfp, "2", 3);				/* find s