/*
	random file routines

	pre-usersgroup release by special request, this package will
be polished and extended before general distribution. address any ideas,
requests, bug reports etc. to:

	Steve Passe
	New Ideas
	2500 S. Pennsylvania St.
	Denver, CO	80210

	(303) 761-4378  who knows when?  your best bet is in the evening

	some notes on these functions-

	since the buffer is completely written to disk even though only
one byte may have been added, a program that uses the entire 8 megabyte
range could fill a disk with 240 byes (assuming a declared buffer size
of 1 k and even distribution of those bytes over the entire range). one
solution would be to keep the buffer size down to 1 sector. the best is
to be reasonable with the address range of your program. logically a file
of 8 megabytes could be spread over x disks if necessary.

	these functions need:	1.cp/m 2.x
				2.alloc() installed in the stdlib
				3:_allocp = NULL; in main
				4.the undocumented (but existing)
					functions rsrec()
						  rcfsiz()
						  rtell()
						  rseek()
						  rread()
						  rwrite()
				  the code for these is in deff2a.asm
				  and is the only existing doc for them

	the buffer size is declared on opening the file and is maintained
in a space provided by alloc(). it can be any length from 1 sector to
memory size available (keep in mind the above note on buffer size as affected
by random writes). if your needs change you can close and reopen with a diff-
erent size buffer. closes AUTOMATICALLY flush the buffer, no explicit call
of rflush() needed.

	see pages 164-169 of 'the c programming language' for insight
into the madness of the package

*/

#include "a:std.h"		/* bdscio.h plus my own system defs */

/*
	routines to parallel the buffered input and output routines,
allowing completely random buffered read and writes to a file

	these are closely modeled on the routines by Leor for
buffered sequential files.

functions - 

	rcreat(filename, fp, mode, secs)
		creat an as yet non-existant file
	ropen(filename, fp, mode, secs)
		open an already existing file
	rclose(fp)
		close a random file (takes care of all 'flushing')
	rflush(fp)
		'flush' a random file buffer to disk
	fseek(fp, offset, origin)
		set the 'byte-pointer' to a specific byte within the
			8 megabyte range of 2.2
	ftell(fp)
		report the current position of the 'byte-pointer' in
			an open file
	rgetc(fp)
		returns the byte pointed at by the 'byte-pointer'
			and bumps the pointer by 1
	rputc(c, fp)
		puts the byte 'c' in the file at the position defined
			by the 'byte-pointer', bumps the pointer by 1

	the next four are not contained in this package. to impliment
take the code of their cousins (getw(),putw(),fprintf(),fscanf()) and
replace all instances of getc() with rgetc(), putc() with rputc().
although I haven't tried this yet they should work properly as all
modularized c code does.

	rgetw(fp)
	rputw(w, fp)
	rprintf(fp, format, arg1, arg2,...)
	rscanf(fp, format, arg1, arg2,...)

	the next four again are unwritten. rgetl() and rputl() will
essentially be fgets() and fputs() modified as in the previous example.
the change to 'l' is to denote the fact that they work with 'lines'
in a cp/m file environment. these two would be used to diddle with cp/m
compatible files. rgets() and rputs() are more straightforward functions
that merely handle strings as c intended. these will be included in the
package for distribution by the users group.

	rgets(str, fp)
	rputs(str, fp)
	rgetl(line, fp)
	rputl(line, fp)

 the following four functions (names) are as yet unwritten but are
reserved for use by this package. again, they will be written for the
distribution package.

	rget(fp, destination, nbyt)
	rput(fp, source, nbyt)
	rgetstruct(fp, destination, structsiz)
	rputstruct(fp, source, structsiz)
*/

/*
	this is the structure that controls the show. such a structure
	must be declared for each file in main. also, very important, be
	sure to include an initialization of _allocp = NULL; in main()
	before doing anything else.
*/

struct _file {
	int _rfd;		/* file descripter */
	int _secs;		/* # of sectors in buffer */
	unsigned _frstsec;	/* first sector in buffer */
	unsigned _cursec;	/* cp/m current random sec */
	byte _curbyt;		/* current random byte */
	byte *_nxtbyt;		/* next byte to be processed */
	byte *_bufbase;		/* location of base */
	byte *_pastbuf;		/* first byte beyond end of buffer */
	char _mode;		/* read, write, append, or direct */
	int _update;		/* buffer modified flag */
	unsigned _curblk;	/* currently addressed block */
	int _blksiz;		/* size of a logical block */
};

/*
	purge any old copies, creat and open 'filename', buffer
size equal to secs * size of one sector, mode of write (from
beginning of file), append (write at end of file), or direct
(read or write, starting at beginning of file, random access
to any part of file). read not allowed as you can't read a
new file.
*/

struct *
rcreat(filename, fp, mode, secs)
char *filename;
struct _file *fp;
char mode;
int secs;
{
	int fd;
	struct *ropen();

	switch (mode) {
	case 'r':	return NULL;
	case 'w':
	case 'a':
	case 'd':
		if ((fd = creat(filename)) < 0 ) return NULL;
		fabort(fd);
		return ropen(filename, fp, mode, secs);
	default:
		return NULL;
	}
}
/*
	open 'filename' buffer in space provided by alloc(), setup
fp, buffer of size SECSIZ * secs
*/

struct *
ropen(filename, fp, mode, secs)
char *filename;
struct _file *fp;
char mode;
int secs;
{
	byte *alloc();
	unsigned rcfsiz(), rsrec(), ftell();

	switch (mode) {
	case 'r':
		fp -> _rfd = open(filename, 0);
		break;
	case 'w':
	case 'a':
	case 'd':
		fp -> _rfd = open(filename, 2);
		break;
	default:
		return NULL;
	}
	if (fp -> _rfd < 0) return NULL;

	if ((fp -> _bufbase = alloc(secs * SECSIZ)) == 0) return NULL;
	fp -> _secs = secs;
	fp -> _nxtbyt = fp -> _bufbase;
	fp -> _pastbuf = fp -> _bufbase + (secs * SECSIZ);
	fp -> _mode = mode;
	fp -> _frstsec = (mode == 'a') ?
		rcfsiz(fp->_rfd) : rsrec(fp->_rfd);
	ftell(fp);
	if (_fillbuf(fp) < 0 ) return NULL;
	return fp;
}

/*
	flushes, then closes the random file at fp
*/

rclose(fp)
struct _file *fp;
{
	if (rflush(fp) < 0 ) return ERROR;
	free(fp->_bufbase);
	return close(fp -> _rfd);
}
/*
	fills buffer pointed at by fp->_bufbase, with file fp->_rfd,
with fp->_secs sectors, starting with cp/m random record field
fseek calls this after updating cp/m's rrf and fp's pointers (if
necessary because seek is out of buffer area)
*/

_fillbuf(fp)
struct _file *fp;
{
	int fd, secs, got;
	byte *base;
	unsigned rseek();

	fd = fp -> _rfd;
	secs = fp -> _secs;
	base = fp -> _bufbase;

	while  (secs -= (got = rread(fd, base, secs))) {
		if ((got > 1000) OR (got == -1)) return ERROR;
		setmem((base += (got * SECSIZ)), SECSIZ, 0);
		base += SECSIZ;
		rseek(fd, 1, 1);
		--secs;
	}
	fp -> _update = NO;
	return OK;
}
/*
	flushes the buffer fp->_bufbase if open for writing AND
fp->_update shows the buffer has indeed been written to since
last _fillbuf call. ignores flushes of i/o devices
*/

rflush(fp)
struct _file *fp;
{
	unsigned rseek();

	if ((fp < 4) OR (fp->_mode == 'r') OR (fp->_update == NO))
		return OK;
	if (rseek(fp -> _rfd, fp -> _frstsec, 0) < 0) return ERROR;
	if (rwrite(fp->_rfd, fp->_bufbase, fp->_secs) != fp->_secs)
		return ERROR;
	fp -> _update = NO;
	return OK;
}
/*
	sets '_nxtbyt' of 'fp' to 'sector's and 'byt's from
'origin' , returns cp/m current random sector.
*/

unsigned
fseek(fp, sector, byt, origin)
struct _file *fp;
unsigned sector;
byte byt;
int origin;
{
	unsigned ftell(), rseek(), rcfsiz();

	switch (origin) {
	case 0:	break;
	case 1:	sector += ftell(fp);
		break;
	case 2:	sector += rcfsiz(fp->_rfd);
		break;
	default:
		return ERROR;
	}
	if ((sector >= fp->_frstsec) AND
		(sector < (fp->_frstsec + fp->_secs))) {
		fp->_nxtbyt = (sector - fp->_frstsec)*SECSIZ
			+ byt + fp->_bufbase;
		return ftell(fp);
	}
	if (rflush(fp) < 0) return ERROR;
	if (rseek(fp->_rfd, sector, 0) < 0) return ERROR;
	fp->_frstsec = sector;
	fp->_nxtbyt = byt + fp->_bufbase;
	if (_fillbuf(fp) < 0) return ERROR;
	return ftell(fp);
}
/*
	sets 'fp->_cursec and _curbyt', returns cp/m current random
sector ('fp->_cursec').
*/

unsigned
ftell(fp)
struct _file *fp;
{
	unsigned b;

	fp->_curbyt = (b = fp->_nxtbyt - fp->_bufbase) % SECSIZ;
	return (fp->_cursec = b / SECSIZ + fp->_frstsec);
}
/*
	seek a current byte that equals the block size * block #,
	return the current block number or NULL
	not finished, needs code change to return NULL instead of
	ERROR on error, btell() to compliment it

unsigned
bseek(fp, block, origin)
struct _file *fp;
unsigned block;
int origin;
{
	unsigned sector, fseek();
	int r1, r2, blocksiz;
	byte byt;

	switch (origin) {
	case 0:	break;
	case 1:	block += fp->_curblk;
		break;
	case 2:		/* not supported */
	default:
		return ERROR;
	}
	blocksiz = fp->_blksiz;
	sector = (blocksiz/128)*block + (r1=(blocksiz%128))*(block/128)
			+ (r1*(r2=(block%128)))/128;
	byt = r1 * r2 % 128;
	if (fseek(fp, sector, byt, 0) == 0) return NULL;
	return (fp->_curblk = block);
}
*/
/*
	gets 'nbyts' into memory starting at 'destination' from
file in 'fp'

rget(fp, destination, nbyt)
struct _file *fp;
unsigned destination;
int nbyt;
{
}
*/
/*
	puts 'nbyts' into file 'fp' from 'source'

rput(fp, source, nbyt)
struct _file *fp;
unsigned source;
int nbyt;
{
}
*/
/*
	returns a char from random buffered file 'fp'
*/

int
rgetc(fp)
struct _file *fp;
{
	if (fp == 0) return getchar();
	if (fp == 3) return bdos(3);
	if (fp->_pastbuf - fp->_nxtbyt)  return *fp->_nxtbyt++;
	if (fseek(fp,0,0,1) < 0) return ERROR;
	return *fp->_nxtbyt++;
}
/*
	puts the char 'c' into random buffered file 'fp'
*/

int
rputc(c, fp)
char c;
struct _file *fp;
{
	if (fp->_mode == 'r') return ERROR;
	if (fp == 1) return putchar(c);
	if (fp == 2) return bdos(5,c);
	if (fp == 3) return bdos(4,c);
	if (fp->_pastbuf - fp->_nxtbyt) {
		fp->_update = YES;
		return *fp->_nxtbyt++ = c;
	}
	if (fseek(fp,0,0,1) < 0) return ERROR;
	fp->_update = YES;
	return *fp->_nxtbyt++ = c;
}
/*
	returns a 16 bit word from file 'fp'

int
rgetw(fp)
struct _file *fp;
{
}
*/
/*
	writes the 16 bit word 'w' to file 'fp'

int
rputw(w, fp)
int w;
struct _file *fp;
{
}
*/
/*
	gets a TRUE c string (null term.) from file 'fp'
into 'str'

int
rgets(str, fp)
char *str;
struct _file *fp;
{
}
*/
/*
	puts the c 'str' into 'fp'

int
rputs(str, fp)
char *str;
struct _file *fp;
{
}
*/
/*
	gets the LF terminated text from 'fp' into 'line',
stripping off CRs from CR-LF combos

int
rgetl(line, fp)
char *line;
struct _file *fp;
{
}
*/
/*
	puts string at 'line' into 'fp', converting LFs to CR-LFs
if no LF exists no terminator is written (ala fputs)

int
rputl(line, fp)
char *line;
struct _file *fp;
{
}
*/
/*
	printf to random buffered file

int
rprintf(fp, format, arg1, arg2,...)
struct _file *fp;
char *format;
{
}
*/
/*
	scanf to random buffered file

int
rscanf(fp, format, arg1, arg2,...)
struct _file *fp;
char *format;
{
}
*/
