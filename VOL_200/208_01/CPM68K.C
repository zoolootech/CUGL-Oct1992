/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48b

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/cpm68k.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	FUNCTIONS: dskcheck, chdir, getwd, format, readdir, getfree
		_inkey, _conin, _conout, raw(), noraw()
	PURPOSE: os dependent functions.
*/



# include <ctype.h>
# include <sgtty.h>
# include "e.h"

/* CP/M bdos function codes */

#define RESET 0
#define CONIN 0x01
#define CONOUT 0x02
#define LSTOUT 0x05
/* in CP/M68K DIRIO differs */
#define DIRIO 0x06
#define IOBYTE 0x07
#define SETIOBYTE 0x08
#define PRINTS 0x09
#define READBUF 0x0A
#define CONSTAT 0x0B
#define VERNO 0x0C
#define RESETDSK 0x0D
#define SELDSK 0x0E
#define OPEN 0x0F
#define CLOSEF 0x10
#define SEARCH1 0x11
#define SEARCHN 0x12
#define DELETEF 0x13
#define READ 0x14
#define WRITE 0x15
#define MAKE 0x16
#define RENAME 0x17
#define LOGINV 0x18
#define CURDSK 0x19
#define SETDMA 0x1A
#define PROTECT 0x1C
#define GETROV 0x1d
/* in CP/M68K DSKPARAMS requires a different argument */
#define DSKPARAMS 0x1F
#define USERCODE 0x20
#define READR 0x21
#define WRITER 0x22
#define FSIZE 0x23
#define SETRR 0x24
#define RESETDRV 0x25


#define CPM3ERRORMODE 45	/*set error mode to return and display
(specific to CP/M Plus) */
# if MSDOS

#define DOSINT 0x21
#define FREESP  0x36
#define SETDIR  0x3b
#define GETDIR  0x47
#define FIND1   0x4e
#define FINDN   0x4f

# endif

extern char *__BDOS();
/* long __BDOS(fn, param)
 * int fn; long param; {
 *	asm("move.l 10(R14),R1");
 *	asm("move.l 8(R14),R0");
 *	asm("trap #2");
 * }
 */

/* _os is an integer version of __BDOS */
static
_os(n, x)
{

	return ((int)__BDOS(n, (long)x )) ;
}

static char *
bios ( fn, p)
{
	struct {
		short bdosn;
		long d1, d2;
	}
	bpb;

	bpb.bdosn = fn;
	bpb.d1 = (long)p;
	return(__BDOS(50, &bpb));
}

/* the argument to dskcheck is altered */
dskcheck(drive)
{
	return((drive >15 || drive < 0)? FAIL: 0);
}

/* return tty status
 * this is heavily system dependent
 * does read(0,buff,1) returns 0 if a char is not
 * ready in your system?
 */
_inkey()
{
	return(_os(CONSTAT));
}

raw()		/* set tty in raw mode */
{
	struct sgttyb obuf;
	if(gtty(0, &obuf) == -1)
		putstr("tty input only"),exit(1);
	obuf.sg_flags |= RAW;
	stty(0, &obuf);
	if(gtty(1, &obuf) == -1)
		putstr("tty output only"), exit(1);
	obuf.sg_flags |= RAW;
	stty(1, &obuf);
}

noraw()		/* restore tty to cooked mode */
{
	struct sgttyb obuf;

	gtty(0, &obuf);
	obuf.sg_flags &= ~RAW;
	stty(0, &obuf);
	gtty(1, &obuf);
	obuf.sg_flags &= ~RAW;
	stty(1, &obuf);
}


/* get a byte RAW from tty
 */
_conin()
{
	char c[2];
	read(0,c,1);
	return(c[0]);
}
/* put a byte RAW to tty. */
_conout(c)
{
	char ch[2];
	ch[0] = c;
	write(1, ch, 1);
	return(c);
}

format(name)	/*expands filename 'name' to full pathname*/
		/*in cp/m68k Output format is uud:ffffffff.xxx\0   */
char *name;
{
	char tempname[FILELEN];

	if (!*name) return;
	if(index(name,':'))
		return;
	strcpy(tempname,curdir);
	strcpy(tempname,":");
	strcpy(tempname,name);
	strcpy(name, tempname);
}

/* change login directory to s[]= "15b"	*/
/* no slash between user number and drive number
 * in CPM68K. This routine calls SELDSK in BIOS.
 */
chdir(s)
char *s;
{
	short user = 0;
	register char drive;

	if(*s == 0)
		return(FAIL);
	while(isdigit(*s)){
		user = 10*user + *s - '0';
		s++;
		if(user >15)return(FAIL);
	}
	drive = tolower(*s);
	if(isalpha(drive))
		if(drive < 'a' || drive > 'p' || bios(9, drive - 'a')== 0)
			return(FAIL);
		else
			_os(SELDSK,drive-'a');
	/* allow 0: or 10a:	*/
	_os(USERCODE, user);
	return(0);
}

/* return the name of the working directory.
 * in CPM68K it is  a string like "15c" without slashes.
 * you will need a colon to form a full-pathed name.
 */
char *
getwd(dir)
char *dir;
{
	register char *p;
	short user;
	p = dir;
	if(user = _os(USERCODE, 0xff)){
		if(user >9)*p++ = '1';
		*p++ = (user % 10) + '0';
	}
	*p++ = _os(CURDSK) + 'a';
	*p = 0;
	return dir;
}

/* readdir returns a stream of filenames delimited by a NEWLINE
 * and terminated by a NULL.
 * the present implementation uses a fixed sized buffer
 * to contain 256 filenames.
 * In other systems, directory entries are not consecutive and
 * readdir() returns a pointer to the next directory entry or
 * NULL. The buffer is usually obtained via calloc(), you will
 * also need to cfree() via closedir().
 */

#define EXTENT 12
#define DIRSIZ 3330

char *
readdir(s)
char *s;
{
	char olddir[5];		/* save old directory */
	static char dmabuf[128];
	static char dirbuf[DIRSIZ];
	static char fcb[36]= "?????????????";
	register int pos, i;
	register char *p, *entri;
	/* my compiler does not complain about '*entry', though */
	char c;

	getwd(olddir);
	if(chdir(s) == FAIL){
		dirbuf[0] = 0;
		return dirbuf;
	}
	fcb[0] = 0;	/* search default drive only */
	p = dirbuf;
	__BDOS(SETDMA, dmabuf);
	pos= (int)__BDOS(SEARCH1,fcb);
	while (pos != 0xff) {
		entri= &dmabuf[pos << 5];
		for (i=1; i < 12; i++) {
			c= *(entri+i) & 0x7f;
			if (i == 9 && c != ' ')
				*p++ = '.';
			if (c != ' ')
				*p++ = tolower(c);
		}
		*p++ = '\n';
		pos=(int)__BDOS(SEARCHN,fcb);
	}
	if(p >= dirbuf + DIRSIZ){
		putstr("dir too big\n");
		exit(1);
	}
	*p = 0;
	chdir(olddir);
	return dirbuf;
}

/* return available storage of directory in kilobytes
 * in CP/M80 Plus
 * 	the most significant byte must be masked.
 * in CP/M80 2.2 and CP/M86
 *	bdos function 27 return a pointer(in HL or BX+ES)
 *	to the allocation bit map of the logged in drive.
 *	The bit on shows that the block is used.
 *	The length of the bit map is to be calculated from
 *	infomation given in a DPB structure,
 *	the pointer to which is again obtained by the bdos
 *	function 31. The DPB structure also contains a value
 *	to mask the first two bytes of the bit map( al0, al1)
 */

getfree(path)
char *path;
{
	register long *free;
	register char *p;
	char dmabuff[128];
	register int drive;
	p = path;
	if(*p == 0)
		return 0;
	while(*p)
		p++;
	--p;
	drive = tolower(*p) - 'a';
	if(dskcheck(drive) == FAIL)
		return 0;
	__BDOS(SETDMA,dmabuff);
	_os(46, drive);
	free = (long *)dmabuff;
	return (int)(*free >> 3);
}

/*	----- bug fix of CP/M68K -------
 *	read/write in RAW mode.
 *	tuned for RAW only.
 *	My own version is much more complicated than what you
 *	will see; it handles tty info correctly ( RAW, CBREAK, ECHO
 *	XTABS, erase, kill). That is why I used stty() and gtty()
 *	elsewhere.
 */
int nofloat();
int nofilesz();
int nobinary(); /* see option.h supplied by DRI	*/

/* _ttyin() is eventually called by read()
 */
_ttyin(fp,buffer,nbytes)
char *fp, *buffer;	/* fp is a struct, to be exact */
{
	if(nbytes == 0)
		return(0);
	if(nbytes != 1)
		return(-1);
	*buffer = _os(DIRIO, 0xff);
	return(1);
}

/* if you want a real fix, you must fix _wrtchr()
 * so that RAW mode can be set on/off
 * it would also enable you to set XTABS on/off
 * so that tab be processed before listing

 */
 /* _ttyout() is eventually called by write()
  */
_ttyout(buf)
register char *buf;
{
	register int count, ii;
	ii= count = *buf++;
	while(ii--)
		_os(DIRIO, *buf++);
	return(count);
}

