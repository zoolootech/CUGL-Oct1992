/*
$title ('yam7.c: file i/o, initialization')
$date (11 NOV 85)
*/
/*
 * File open and close stuff and more ...
 * This file has been modified for operation on a MS-DOS system
 */

#include <dos.h>
#include "yam.h"
#include <fcntl.h>

/* globals used by dodir */
unsigned Numfiles;  /* Total number of files expanded */
unsigned Blocks;   /* Number of records */

/* following are MSDOS system calls */
#define COMPFILSIZ 0x23	/* compute file size */
#define CHANGEDIR 0x3B	/* change current directory */
#define FINDFIRST 0x4E	/* bdos search for file pattern*/
#define FINDNEXT 0x4F	/* search for next occurrence */
#define GETDEFDISK 0x19	/* get current disk */
#define GETFREE 0x36    /* get free disk space */
#define SELDSK 0x0E		/* bdos select disk 0=default disk */
#define SETDMA 0x1A		/* set address for read, write, etc. */
#define DIRAT 0x10		/* directory attribute for file searches */
#define ARCHIVE 0x20	/* archive bit */
#define NOTFOUND 18		/* file not found in search */
#define CHATTRIB 0x43	/* change attribute */
#define CARRYFLG 0x01

extern char *getenv();

/****************************************************************************
FUNCTION:
	get an single char from console.  Display it as entered.

CALLING PARAMETERS:
	none

RETURNED PARAMETERS:
	character value typed on console
===========================================================================*/
char getopt()
{
	char c;

		/* note: if <ctype.h> included, tolower lattice c function makes
		   this routine request double entrys for each char. */
	c = tolower(getcty());
	putcty(c);
	return c;
} /* getopt */


/****************************************************************************
FUNCTION:
	open a file for receive.  This file will be written to.

CALLING PARAMETERS:
	pathname:
		pointer to ascii file pathname representation
===========================================================================*/
openrx(pathname)
char *pathname;
{
	char option;
#ifdef RESTRICTED
	char *s;

	/* no garbage names please */
	for(s=pathname; *s; )
		if(*s++ > 'z')
			return ERROR;
#endif
	unspace(pathname);
		/* show the name right away */
	printf("'%s' ", pathname);

		/* open file for read.  If exists, allow option to overwrite */
	if(!Creamfile && ((fout=fopen(pathname,"r")) != NULL))
	{
			/* close file if successful open */
		fclose(fout);
#ifdef XMODEM
		printf("Can't upload: file exists\n");
		return ERROR;
#else
		printf("Exists, Replace/Append/Quit (r/a/q)?");
		option=getopt();
		if(!index(option, "ary"))
			return ERROR;
#endif
	}
	if(option=='a')
		fout=fopen(pathname,"ab+");
	else
		fout=fopen(pathname,"wb");
	if (fout==NULL)
	{
		printf(" Can't create\n");
		return ERROR;
	}
		/* receive file is open */
	Rfile= TRUE;
	strcpy(Rname, pathname);

		/* Dumping means term input will go to disk.  Squelch allows this
		   to be turned on and off.  Why I don't know. */
	Dumping = !Squelch;
	printf(" Open\n");
#ifdef STATLINE
	lpstat("Receiving %s", Rname);
#endif
	return OK;
}


/****************************************************************************
FUNCTION:
	close receive data file

CALLING PARAMETERS:
	pad:
		sets pad for log file
===========================================================================*/
closerx(pad)
{
		/* close only if sucessful open */
	if(Rfile)
	{
			/* flush all buffers before closing */
		fflush(fout);
		fclose(fout);
		Rfile=FALSE;
#ifdef LOGRX
		logfile(LOGRX, Rname, pad?'*':'R');	/* record file xmsn */
#endif
	}
}


/****************************************************************************
FUNCTION:
	get file size and return number bytes occupied by file

CALLING PARAMETERS:
	*pathname:
		pointer to file pathname.
===========================================================================*/
long getfilesize(pathname)
char *pathname;
{
	struct find_buf f_buf;
	union REGS inregs;
	union REGS outregs;

		/* DMA must point to f_buf structure */
	bdos(SETDMA, &f_buf);
		/* set pointer to file name */
	inregs.x.dx = (short)pathname;
		/* find first DOS function */
	inregs.h.ah = FINDFIRST;
		/* allow search for files only */
	inregs.x.cx = DIRAT;

		/* dos request function */
	intdos(&inregs,&outregs);
	if (outregs.h.al != NULL)
		return NULL;

		/* return number of blocks used by file */
	return f_buf.filesize;
} /* getfilesize */


/****************************************************************************
FUNCTION:
	open a file for transmission to a remote

CALLING PARAMETERS:
	mode:
		0=don't compute file size
		1=compute file size
		>1=use file size already computed in f_buf
	f_buf:
		pointer to a structure containing information about file. if
		0 routine will not report file transmission time.  If 1, file
		size must be calculated, otherwise the structure is already
		initialized.
	*pathname:
		pointer to a string representing file name to be opened
===========================================================================*/
opentx(mode,f_buf,pathname)
struct find_buf *f_buf;
char *pathname;
int mode;
{
	int blocks;
	long bytes;

		/* set file name into global buffer */
	unspace(pathname);
	strcpy(Tname, pathname);

	printf("'%s' ", pathname);
	if((fin=fopen(pathname,"rb"))==NULL)
	{
		printf("Can't open\n");
		return ERROR;
	}
	printf("Open\n");

#ifdef RESTRICTED
			/* restriced file stuff needs to be added */
		fclose(fin);
		printf("Not for Distribution\n");
		return ERROR;
#endif
 
		/* transmit file is open */
	Tfile= TRUE;

		/* compute file size. mode will <> NULL when f_buf it is initialized
		   by another routine */
	if (mode == 1)
		bytes = getfilesize(pathname);
	else if (mode != NULL)
		bytes = f_buf->filesize;

		/* display download time */
	blocks = (int)(bytes/blklen + (bytes%blklen?1:0));
	if (mode != NULL)
		dis_xmit_time(1,blocks);

	return OK;
} /* opentx */


/****************************************************************************
FUNCTION:
	display transmission time of a file

CALLING PARAMETERS:
	nfiles:
		number of file(s) to be sent
	blks:
		number of blks occupied by file(s)
===========================================================================*/
dis_xmit_time(nfiles,blks)
unsigned nfiles;
unsigned blks;
{
	unsigned spm;
	unsigned dminutes;

		/* sector per minute */
		/* (Baudrate*60)/(10 bits each char * 136 chars) */
	spm = Baudrate/23;

		/* download minutes calculation */
	dminutes = nfiles+((10*(nfiles+blks))/spm)+(blks/20);

		/* display computed transmission time */
		/* display number and size.  #k/s = 128b/s / 1024b/k = 1/8 */
	printf("%u Blocks %dk %u.%u Minutes Xmsn Time at %u Baud\n",
		blks, blks/8, dminutes/10, dminutes%10, Baudrate);

	/* display file name, length and minutes for transfer on status line */
#ifdef STATLINE
	lpstat("%s %u Blocks %dk %u.%u Min",
		Tname, blks, blks/8, dminutes/10, dminutes%10);
#endif
} /* dis_xmit_time */


/****************************************************************************
FUNCTION:
	closetx(status) call with status != 0 if incomplete file xmsn

CALLING PARAMETERS:
	status:
		if false and LOGTX, will call logfile with Tname
===========================================================================*/
closetx(status)
{
	if(Tfile)
	{
		fclose(fin);
#ifdef LOGTX
		if(!status)
			logfile(LOGTX, Tname, 's');	/* record file xmsn */
#endif
		Tfile=FALSE;
	}
} /* closetx */


/****************************************************************************
FUNCTION:
	search the phone file for name

CALLING PARAMETERS:
	name:
		pointer to name to search for in phone file
	buffer:
		pointer to buffer to use to read phone file into

RETURNED PARAMETERS:
	TRUE if succesful, ERROR if something went wrong
===========================================================================*/
#ifdef PHONES
getphone(name, buffer)
char *name, *buffer;
{
	closetx(TRUE);

	searchpath(PHONES,Utility.ubuf);
	if((fin=fopen(Utility.ubuf,"rb"))==NULL)
	{
		printf("Cannot open %s\n", Utility.ubuf);
		return ERROR;
	}
	else
	{
		while(fgets(buffer, 100, fin))
			if(cmdeq(buffer, name))
			{
				fclose(fin);
				return OK;
			}
	}
	fclose(fin);
	return ERROR;
} /* get phone */
#endif


/****************************************************************************
FUNCTION:
	change default disk.  Removed the valid disk check, let the
	operating system do that.  It his job.

CALLING PARAMETERS:
	p:
		pointer to disk to change to.
===========================================================================*/
chdsk(p)
char *p;
{

		/* check if file is open for transmit or receive */
	if(Rfile||Tfile)
	{
		printf("Must close files first");
		return ERROR;
	}
	*p=toupper(*p);
	defdisk= *p - 'A';
	bdos(SELDSK, defdisk);
	return OK;

} /* chdsk */


/****************************************************************************
FUNCTION:
	change current directory.

CALLING PARAMETERS:
	ptr:
		pointer to directory to change to.
===========================================================================*/
chgdir(ptr)
char **ptr;
{
	int stat;

	stat = bdos(CHANGEDIR, *ptr);
	if (stat != OK)
		printf("directory not found\n");
} /* chdir */


/****************************************************************************
FUNCTION:
	print the current working directory.

CALLING PARAMETERS:
	none
===========================================================================*/
dopwd()
{

	union REGS inregs;
	union REGS outregs;

		/* pointer to scratch buffer */
	inregs.x.si = (short)&Utility.ubuf;

		/* get text of current directory */
	inregs.h.ah = 0x47;

		/* use current disk */
	inregs.h.dl = 0;

		/* dos request function */
	intdos(&inregs,&outregs);

	if (outregs.x.ax==0x15)
		printf("invalid drive\n");
	else
		printf("%s\n",&Utility.ubuf);
} /* dopwd */



/****************************************************************************
FUNCTION:
	print string for terminals with a status line starting at column 48.
	note that a call to lpstat will erase what pstat displays

CALLING PARAMETERS:
	*a,*b,*c:
		pointers to strings to be printed.
===========================================================================*/
pstat(a,b,c)
char *a, *b, *c;
{
	char pbuf[40];

#ifdef STATLINE
	/* insert string for terminal to go to stat line, col 48 */
	lputs("\0");
#endif
	sprintf(pbuf, a,b,c);
	lputs(pbuf);

	/* insert line to go return from stat line */
#ifdef STATLINE
	lputs("\0");
#else
	printf("\n");
#endif
} /* pstat */


/****************************************************************************
FUNCTION:
	print string for terminals with a status line starting at column 1.
	Rest of line is erased

CALLING PARAMETERS:
	*a...*g
		pointers to strings to be printed.
===========================================================================*/
lpstat(a,b,c,d,e,f,g)
char *a, *b, *c, *d, *e, *f, *g;
{
	/* insert line to go to status line col 1 */
#ifdef STATLINE
	lputs("\0");
#endif
	printf(a,b,c,d,e,f,g);

	/* insert line to go to return from status line */
#ifdef STATLINE
	lputs("\0");
#else
	printf("\n");
#endif
}


/****************************************************************************
FUNCTION:
	List a file to the console.  Wild card listings are allowed

CALLING PARAMETERS:
	argc:
		integer number of parameters at argp
	**argp:
		pointer to pointer to pathname for directory search
===========================================================================*/
dolist(argc, argp)
char **argp;
int argc;
{
	int listfile();

	printf("^S pauses, ^X skips to next file, ^C terminates\n");
	expand(listfile, argc, argp);
} /* dolist */


/****************************************************************************
FUNCTION:
	Display a directory of the disk.

CALLING PARAMETERS:
	argc:
		count of number of paramters at argp
	argp:
		pointer to pointer to pathname for directory search
===========================================================================*/
dodir(argc, argp)
char **argp;
int argc;
{
	int pdirent();

		/* reset variables */
	cfast=Numfiles=Blocks=0;

		/* do function.  error return will cause abort(such as file not
		   found or ^C entered on keyboard */
	if (expand(pdirent, argc, argp)==ERROR)
		return;

		/* make sure bytes left is on its own line */
	if (cfast!=3)
		putch('\n');

		/* number of total files, including directorys */
	printf("%d File(s) ",Numfiles);

		/* display bytes left on disk */
	printdfr();
	putch('\n');

		/* display download time */
	dis_xmit_time(Numfiles,Blocks);

} /* dodir */


/****************************************************************************
FUNCTION:
	print directory entry left justfied in a 14 char wide field, followed
	by length of file and calculated transmission time.

CALLING PARAMETERS:
	f_buf:
		pointer to a structure containing information about file
	*pathname:
		pointer to directory path name
===========================================================================*/
pdirent(f_buf,pathname)
struct find_buf *f_buf;
char *pathname;
{
	long blks;

	if (f_buf->attrib & DIRAT)
		printf("%-14s <DIR>    ",f_buf->pname);

		/* ignore special files */
	else if ((f_buf->attrib & !ARCHIVE)==0)
	{
		printf("%-14s  %7ld ",f_buf->pname,f_buf->filesize);

			/* number of blklen byte records */
		blks = (f_buf->filesize)/blklen;
		if (f_buf->filesize % blklen)
			blks++;

			/* global indication of how many files were found */
		Numfiles = Numfiles + 1;

			/* total blocks */
		Blocks = Blocks + (int)blks;
	}

	if (++cfast==3)
	{
		cfast=0;
		putchar('\n');
	}
	else
		printf("| ");

		/* allow ^C to abort */
	if (CIREADY && CICHAR==ETX)
		return ERROR;
	return OK;
} /* pdirent */


/****************************************************************************
FUNCTION:
	expand the requested path by pre-pending the requested search rules
	as defined in the PATH environment parameter.  This allows default
	files to be stored where the main program is loaded from.  The user
	can then access all special files from anywhere, without being restricted
	to a specific directory.  The routine creates the pathname and tries to 
	open it.  If successful it returns, otherwise it creates the next pathname
	and tries again.

CALLING PARAMETERS:
	fname:
		pointer to string representing pathname to search for
	pathbuf:
		pointer to buffer where resulting path name will be contructed.
===========================================================================*/
searchpath(fname,pathbuf)
char *fname;
char *pathbuf;
{
	char *env;
	char *eptr;
	int baselen;

		/* get environent path string */
	env = getenv("PATH");

		/* search for pathname separator */
	eptr = env;
	baselen=0;
	while(1)
	{
		baselen++;
		if ( (*++eptr == 0) || (*eptr == ';') )
		{
				/* copy basename to local buffer */
			movmem(env,pathbuf,baselen);
			pathbuf[baselen] = '\\';
			pathbuf[baselen+1] = 0;

				/* concat file name to create full pathname */
			strcat(pathbuf,fname);

				/* if not a successful open, continue */
			fin=fopen(pathbuf,"rb");
			fclose(fin);
			if (fin!=NULL)
				return;
			
				/* reset for next interation */
			baselen = 0;	

				/* done on last string */
			if (!*eptr)
				break;
			env = ++eptr;
		}
	}
		/* copy default name into buffer */
	strcpy(pathbuf,fname);
} /* searchpath */


/****************************************************************************
FUNCTION:
	Processes a DOS pathname for ambiguous file references.  The fixed up
	search path is placed in a buffer specified by the sp pointer.  The
	buffer at sp will contain the input, possibly augmented with "*.*" or
	"\*.*" so that it will work with the DOS FINDFIRST and FINDNEXT functions.
	Basic concept stolen from Dr. Dobbs Journal, 10/85 p16.
 
CALLING PARAMETERS:
	ip:
		input path pointer
	sp:
		pointer to buffer to receive search path

RETURNED PARAMETERS:
	ERROR if the input path should not be used, OK otherwise.
===========================================================================*/
int fixpath(ip,sp)
char *ip;
char *sp;
{
	union REGS inregs;
	int flags;

		/* all cases return the input string */
	strcpy(sp,ip);

		/* 4 special cases:
			1) null string = "*.*"
			2) "x:" = "x:*.*"
			3) root dir "\" which is mis-handled by MS-DOS 2.1 and 3.0
			4) any reference ending in "\"	*/
	if ( (strlen(ip)==0)		/* null string */
	   ||(strcmp(ip+1,":")==0)	/* d: only */
	   ||(index(ip[strlen(ip)-1],PATHSEP)) )	/* ends in path separator */
	{
			/* add *.* for search */
		strcat(sp,"*.*");
		return(OK);
	}

		/* non-null terminated string.  possibilities are:
			1) explicit file reference
			2) explicit directory reference
			3) ambiguou file reference
			4) unknown name
		   These can be separted reliably with the dos change/read attributes
		   function. */
	inregs.h.ah = CHATTRIB;
	inregs.h.al = 0;
	inregs.x.dx = (short)ip;
	inregs.x.cx = 0;
	flags = intdos(&inregs,&inregs);

		/* final name unknown, no use to search */
	if ( (flags & CARRYFLG) && (inregs.x.ax==0x0002) )
		return ERROR;

		/* path not found or file is not a directory.  File open will fail
		   if file does not exist */
	if ( ((flags & CARRYFLG)&&(inregs.x.ax==0x0003)
		||(((flags & CARRYFLG)==0)&&((inregs.x.cx & DIRAT)==0))) )
	{
		return OK;
	}

		/* file is a directory, append "\*.*" */
	if (((flags & CARRYFLG)==0)&&(inregs.x.cx & DIRAT))
	{
		strcat(sp,"\\*.*");
		return OK;
	}
	else
		return ERROR;
} /* fixpath */


/****************************************************************************
FUNCTION:
	expand an ambiguous file name, and call a function using the expanded
	name.

CALLING PARAMETERS:
	*fnx():
		pointer to a function to call after file name is expanded.
	argc:
		integer count for number of argp's
	argp:
		pointer to pointer to arguments
===========================================================================*/
expand(fnx, argc, argp)
int (*fnx)();
int argc;
char **argp;
{
	char name[PATHLEN];

		/* no arguments, use *.* */
	if(argc<=0)
		return e1xpand(fnx, "*.*");
	else
		while(--argc>=0)
		{
				/* expand ambiguous file name */
			fixpath(*argp++,name);
			return e1xpand(fnx, name);
		}
	return OK;
} /* expand */


/****************************************************************************
FUNCTION:
	e1xpand expands ambiguous pathname afnp	calling fnx for each.  Full
	MS-DOS pathnames are supported.

CALLING PARAMETERS:
	fnx:
		pointer to function to call after file name is expanded
	*afnp:
		pointer to ambiguous file name
===========================================================================*/
e1xpand(fnx, afnp)
int (*fnx)();
char *afnp[];
{
	char pathname[PATHLEN];
	int filecount,
		baselen;
	union REGS inregs;
	union REGS outregs;
	struct find_buf f_buf;

		/* extract directory name */
	strcpy(pathname,afnp);
		/* search backwards for directory separator */
	for (baselen=strlen(pathname); baselen>=0; baselen--)
	{
		if (index(pathname[baselen],PATHSEP))
			break;
		pathname[baselen]=0;	
	}
	baselen++;

		/* DMA must point to f_buf structure */
	bdos(SETDMA, &f_buf);
		/* set pointer to file name */
	inregs.x.dx = (short)afnp;
		/* allow search for files and directorys */
	inregs.x.cx = DIRAT;
		/* find first DOS function */
	inregs.h.ah = FINDFIRST;

	filecount = 0;
	while(1)
	{
		intdos(&inregs,&outregs);
			/* done?? */
		if (outregs.x.ax == NOTFOUND)
			break;

			/* indicate how many files were found */
		filecount = filecount + 1;

			/* search command finds name of file, not full pathname.
			   send full path name along with file info */
		strcat(pathname,f_buf.pname);
		if((*fnx)(&f_buf,pathname)==ERROR)
			return ERROR;

			/* reset pathname to directory name */
		pathname[baselen]=0;

			/* after findfirst, rest must be findnext */
		inregs.h.ah = FINDNEXT;
	}

	if (filecount==0)
	{
		printf("'%s' NOT FOUND\n", afnp);
		return ERROR;
	}

	return OK;
} /* e1xpand */



/****************************************************************************
FUNCTION:
	cisubstr(string, token) searches for lower case token in string s
	returns pointer to token within string if found, NULL otherwise
	ignore parity in strings s

CALLING PARAMETERS:
	*s:
		pointer to string
	*t:
		pointer to character token to search for
===========================================================================*/
char *cisubstr(s, t)
char *s,*t;
{
	char *ss,*tt;
	/* search for first char of token */
	for(ss=s; *s; s++)
		if(tolower(*s & 0177)==*t)
			/* compare token with substring */
			for(ss=s,tt=t; ;)
			{
				if(*tt==0)
					return s;
				if(tolower(*ss++ & 0177) != *tt++)
					break;
			}
	return NULL;
} /* cisubstr */


/****************************************************************************
FUNCTION:
	copy string s onto itself deleting spaces "hello there" > "hellothere"

CALLING PARAMETERS:
	*s:
		pointer to string
===========================================================================*/
unspace(s)
char *s;
{
	char *p;
	for(p=s; *s; s++)
		if(*s != ' ')
			*p++ = *s;
	*p++ =0;
} /* unspace */


#ifdef LOGFILE
/****************************************************************************
FUNCTION:
	logfile keeps a record of files transmitted.

CALLING PARAMETERS:
	*log:
	*name:
	mode:
===========================================================================*/
logfile(log, name, mode)
char *log, *name;
char mode;
{
	int thedisk;
	char *i;

	/* find out what disk was used */
	thedisk=defdisk + 'a';
	if(i=index(':', name))
	{
		thedisk = *name;
		name= ++i;
	}
	if(i=index('\t', Phone))	/* keep just the system name */
		*i = 0;
#ifdef XMODEM
	if(i=index('\r', Phone))	/* keep just user's name */
		*i = 0;
	if(i=index('\n', Phone))	/* keep just user's name */
		*i = 0;
#endif

	if(fin=fopen(log,"ab")!=NULL)
	{
		fprintf(fin, "%c %5u	%c:%-14s %s\n",
		 mode, Baudrate, thedisk, name, Phone);
		putc(CPMEOF, fin);
		fflush(fin);
		fclose(fin);
	}
} /* logfile */
#endif


/****************************************************************************
FUNCTION:
	perform CRC calculation on a file
	The generator is X^16 + X^12 + X^5 + 1 as recommended by CCITT.
	This routine stops calculating CRC on EOF so it should have the
	same numbers as its CPM counterpart.

CALLING PARAMETERS:
	f_buf:
		pointer to a structure containing information about file
	*pathname:
		pointer to directory path name
===========================================================================*/
#ifdef BIGYAM
crcfile(f_buf,pathname)
struct find_buf *f_buf;
char *pathname;
{
	printf("%-14s ",f_buf->pname);
	oldcrc = 0;
	if ((Utility.ufbuf=fopen(pathname,"rb"))==NULL)
		return ERROR;

		/* read file and do crc calculation */
	while ((firstch=getc(Utility.ufbuf))!=EOF)
	{
			/* crc calculation */
		oldcrc = updcrc(firstch, oldcrc);
	}

		/* complete crc calculation */
	oldcrc=updcrc(0,updcrc(0,oldcrc));
	fclose(Utility.ufbuf);
	if(CIREADY)
		return ERROR;

		/* display crc and file name */
	printf("CRC = %04x\n", oldcrc);
	return OK;
} /* crcfile */


/****************************************************************************
FUNCTION:
	perform CRC calculation on a file, or group of files

CALLING PARAMETERS:
	argc:
		integer count for number of argp's
	argp:
		pointer to pointer to arguments
===========================================================================*/
docrc(argc, argp)
char **argp;
{
	expand(crcfile, argc, argp);
} /* docrc */


/****************************************************************************
FUNCTION:
	perform CRCK calculation on one or a group of files

CALLING PARAMETERS:
	argc:
		integer count for number of argp's
	argp:
		pointer to pointer to arguments
===========================================================================*/
docrck(argc, argp)
char **argp;
{
	int crckfile();

	expand(crckfile, argc, argp);
} /* docrck */


/****************************************************************************
FUNCTION:
	Accumulate and print a "crck" for a file
	CCITT standard polynominal:
	X^16 + X^15 + X^13 + X^7 + X^4 + X^2 + X + 1
	This routine stops when the EOF is read.  CPM versions always do the
	crc for an entire sector.  Some CRCK programs in CPM use this algorythm
	but generate numbers for data past EOF.  Therefore this routine
	will not generate same numbers unless file is exact multiple of sector
	size.

CALLING PARAMETERS:
	f_buf:
		pointer to a structure containing information about file
	*pathname:
		pointer to directory path name
===========================================================================*/
crckfile(f_buf,pathname)
struct find_buf *f_buf;
char *pathname;
{
	unsigned crck();
	int fd, st;

	printf("%-14s ",f_buf->pname);

		/* level 1 io, open read only, raw mode */
	fd=open(pathname, (O_RDONLY | O_RAW));
	if( fd==ERROR )
		return ERROR;

		/* init crc */
	oldcrc=0;

		/* read a block */
	while((st=read(fd, Utility.ubuf, blklen)) > 0)
				/* update CRCK */
			oldcrc=crck(Utility.ubuf, st, oldcrc);

	if (close(fd))
		printf("file I/O error\n");
	if(CIREADY)
		return ERROR;

		/* display CRC, number of records and file name */
	printf("CRCK = %04x\n", oldcrc);
	return OK;

} /* crckfile */
#endif


/****************************************************************************
FUNCTION:
	print number of free blocks on default disk

CALLING PARAMETERS:
	none.
===========================================================================*/
printdfr()
{
		/* print number of free bytes.  add 1 to defdisk because 0 means
		   currently logged disk to getfree */
	printf("%u kb Free on %c: ", getfree(defdisk+1), defdisk+'A');

} /* printdfr */


/****************************************************************************
FUNCTION:
	return total free kilobytes of disk

CALLING PARAMETERS:
	disk:
		drive number to find free space on. 0=defdisk,1=A,...
===========================================================================*/
unsigned getfree(disk)
int disk;
{
	union REGS inregs;
	union REGS outregs;
	long fullsize;

		/* get disk free space function */
	inregs.h.ah = GETFREE;

		/* set drive number */ 
	inregs.x.dx = disk;

		/* dos request function */
	intdos(&inregs,&outregs);
	if (outregs.x.ax == 0xFFFF)
		printf("INVALID DISK\n");

		/* total space left in bytes */
	fullsize = (long)outregs.x.ax * (long)outregs.x.bx * (long)outregs.x.cx;

		/* return KBytes */
	return (unsigned)(fullsize/1000);

} /* getfree */


/****************************************************************************
FUNCTION:
	erase files specified by path name atr argp

CALLING PARAMETERS:
	argc:
		integer number of parameters at argp
	argp:
		pointer to a pointer to string of file names.
===========================================================================*/
doerase(argc, argp)
char **argp;
{
	int erasefile();

	expand(erasefile, argc, argp);
	printdfr();

} /* doerase */


/****************************************************************************
FUNCTION:
	erase a single file from directory

CALLING PARAMETERS:
	f_buf:
		pointer to a structure containing information about file
	*pathname:
		pointer to directory path name
===========================================================================*/
erasefile(f_buf,pathname)
struct find_buf *f_buf;
char *pathname;
{
		/* don't allow directorys to be erased */
	if (f_buf->attrib & DIRAT)
		return FALSE;

	printf("Erase %s (y/n/q)? ",f_buf->pname);
	switch(getopt())
	{
	case 'y':
		unlink(pathname);
	case 'n':
		putch('\n');
		return FALSE;
	}
	return ERROR;
} /* erasefile */


#ifdef BIGYAM
/****************************************************************************
FUNCTION:
	Sum bytes in file mod 2^16 discard CR and EOF+junk.  Displays
	sum and a sort of CRC for file 

CALLING PARAMETERS:
	f_buf:
		pointer to a structure containing information about file
	*pathname:
		pointer to directory path name
===========================================================================*/
sumfile(f_buf,pathname)
struct find_buf *f_buf;
char *pathname;
{
	unsigned nbytes;

	oldcrc = 0;
	nbytes = 0;
	printf("%-14s ",pathname);
	
		/* try to open file for read.  Utility buff is used as scratch */
	if ((Utility.ufbuf=fopen(f_buf->pname,"rb"))==NULL)
	{
		fprintf(stderr, "Can't open %s\n", pathname);
		return ERROR;
	}

		/* successful open, do sum calc */
	for (; (cfast=getc(Utility.ufbuf))!=EOF && cfast!=0x1a; )
	{
		if (cfast=='\r')
			continue;
		++nbytes;

			/* crc calculation */
		if (oldcrc&01)
			oldcrc = (oldcrc>>1) + 0x8000;
		else
			oldcrc >>= 1;
		oldcrc += cfast;
	}

	fclose(Utility.ufbuf);
	if(CIREADY)
		return ERROR;

		/* dipsplay crc, number of bytes */
	printf("CRC = %5u #bytes = %7u\n", oldcrc, nbytes);
	return OK;
} /* sumfile */


/****************************************************************************
FUNCTION:
	Sum bytes in file or group of files.  See sumfile.

CALLING PARAMETERS:
	argc:
		integer count for number of argp's
	argp:
		pointer to pointer to arguments
===========================================================================*/
dosum(argc, argp)
char **argp;
{
	int sumfile();
	expand(sumfile, argc, argp);
} /* dosum */


/****************************************************************************
FUNCTION:
	count lines words bytes and carriage returns in file

CALLING PARAMETERS:
	f_buf:
		pointer to a structure containing information about file
	*pathname:
		pointer to directory path name
===========================================================================*/
wcfile(f_buf,pathname)
struct find_buf *f_buf;
char *pathname;
{
	unsigned lines, words, ncr;

	if ((Utility.ufbuf=fopen(pathname,"rb"))==NULL)
		return ERROR;
	lines = words = firstch = ncr = checksum = 0;
	while(1)
	{
		cfast = getc(Utility.ufbuf);
		if (cfast == EOF || cfast == 0x1a)
			break;
		++firstch;
		if (cfast>040 && cfast<0177)
		{
			if (!checksum)
			{
				++words;
				++checksum;
			}
			continue;
		}
		if (cfast=='\n')
			++lines;
		else if (cfast=='\r')
			++ncr;
		else if (cfast!=' ' && cfast!='\t')
			continue;
		checksum = 0;
	}
	fclose(Utility.ufbuf);
	if(CIREADY)
		return ERROR;
	printf("%7u lines,%7u words,%7u%7u %s\n",
		 lines,words, firstch-ncr, firstch, f_buf->pname);
	return OK;
} /* wcfile */


/****************************************************************************
FUNCTION:
	entry point for word count.  Count lines words bytes and carriage
	returns in file
	
CALLING PARAMETERS:
	argc:
		count of parameters at argp
	argp:
		pointer to pointer to file name.
===========================================================================*/
dowc(argc, argp)
char **argp;
{
	expand(wcfile, argc, argp);
} /* dowc */


/****************************************************************************
FUNCTION:
	dump a file in side by side hex-ascii format to crt

CALLING PARAMETERS:
	f_buf:
		pointer to a structure containing information about file
	*pathname:
		pointer to directory path name
===========================================================================*/
dump(f_buf,pathname)
struct find_buf *f_buf;
char *pathname;
{
	int fd, st;
	char *val;
	unsigned i;	
	long startnum;

	printf("%-14s\n",f_buf->pname);

		/* level 1 io, open read only, raw mode */
	fd=open(pathname, (O_RDONLY | O_RAW));
	if( fd==ERROR )
		return ERROR;

		/* display a line at a time of the file */
	startnum = 0;
	while((st=read(fd, Utility.ubuf, 16)) > 0)
	{
			/* display in hex */
		val = (char *)Utility.ubuf;
		printf("%05lx  ",startnum);
		for(i=0; i<st; i++)
			printf("%02x ",*val++);

		while(i++ <= 16)
			printf("   ");
		printf("  ");

			/* display in ascii */
		val = (char *)Utility.ubuf;
		for(i=0; i<st; i++,val++)
			printf("%c",(*val&0x7f)>' '?(*val&0x7f):'.');

			/* to next line */
		startnum=startnum+16;
		putchar('\n');
		if(CIREADY)
			return ERROR;
	}
	if (close(fd))
		printf("file I/O error\n");
	return OK;
} /* dump */


/****************************************************************************
FUNCTION:
	do a hex-ascii dump of a file to srceen.
	
CALLING PARAMETERS:
	argc:
		count of parameters at argp
	argp:
		pointer to pointer to file name.
===========================================================================*/
dodump(argc, argp)
char **argp;
{
	expand(dump, argc, argp);
} /* dodump */

#endif  /* if BIGYAM */


/****************************************************************************
FUNCTION:
	copy string at sourceptr to destptr, converting special escaped
	character sequences to their proper values.  This is used at initialization
	to read convert ascii strings from the configuration file

CALLING PARAMETERS:
	destptr:
		pointer to string to receive the formated data
	sourceptr:
		pointer to source ascii string which may contain escape sequences
===========================================================================*/
sconfig(destptr,sourceptr)
char *destptr, *sourceptr;
{
	char *xptr;

		/* move to start of string */
	while(*sourceptr++ != '=');

		/* copy to local buffer and convert escaped strings */
	while(*sourceptr)
	{	
		if (*sourceptr == '\\')
		{
			sourceptr++;

				/* carrige return */
			if (*sourceptr == 'n')
			{
				*destptr++ = '\n';
				sourceptr++;
			}
				/* default to a hex number of format \0xnn */
			else
			{
				sourceptr = sourceptr + 2;
				sscanf(sourceptr,"%x",destptr++);
				sourceptr = sourceptr + 2;
			}	
		}
		else if (*sourceptr == '\n')
			sourceptr++;
		else
		{
			*destptr++ = *sourceptr++;
		}
	}
	*destptr = '\0';

}/* sconfig */


/****************************************************************************
FUNCTION:
	read the configuration file a line at a time and copy it to local storage.
	This was implemented to allow strings which are frequently changed to be
	modified without re-compliation.  NOTE: the destination strings must
	be large enough to accept all the data from the configuration file.  NO
	length checking is done.  This routine causes the signon message to
	be displayed, and shows the current baud and port configuration.

CALLING PARAMETERS:
	buffer:
		pointer to scratch buffer to use when reading configuration file
===========================================================================*/
read_config(buffer)
char *buffer;
{

	searchpath(YAMCONFIG,Utility.ubuf);
	if((fin=fopen(Utility.ubuf,"r"))==NULL)
	{
		printf("Cannot open %s\n", Utility.ubuf);
		return;
	}
	else
	{
			/* read config file a line at a time */
		while(fgets(buffer, 80, fin))
		{
		
			/* ignore null lines and those with a ';' as the
				   first character */
			if( (*buffer != ';') && (*buffer != '\n') )			
			{
					/* set default port configuration(Baudrate, parity,etc */
				if(strncmp("PORTINIT",buffer,8)==0)
					setparams(buffer);

					/* display signon strings */
				else if( (strncmp("FLAVOR",buffer,6)==0) ||
						 (strncmp("REVISION",buffer,8)==0) )
				{
					sconfig(Utility.ubuf,buffer);
					printf("%s",Utility.ubuf);
				}

					/* characters to ignore */
				else if(strncmp("RXNONO",buffer,6)==0)
					sconfig(rxnono,buffer);

					/* xon string */
				else if(strncmp("XON",buffer,3)==0)
				{
					sconfig(Utility.ubuf,buffer);
					xon = Utility.ubuf[0];
				}

					/* xoff string */
				else if(strncmp("XOFF",buffer,4)==0)
				{
					sconfig(Utility.ubuf,buffer);
					xoff = Utility.ubuf[0];
				}

				else
					printf("\n%sBad command in configuration file\n",buffer);
			}
		}
	}
	fclose(fin);
}/* read_config */


/****************************************************************************
FUNCTION:
	Perform various operating system dependent initializations that should
	be performed but once.

CALLING PARAMETERS:
	none
===========================================================================*/
onceonly()
{
	bufst = (char *)malloc(10000);
	if (bufst==NULL)
	{
		printf("insufficent memory");
		exit(1);
	}
	bufend = bufst + 10000;
	T1pause=290*CLKMHZ;	/* calibration for deciseconds in readbyte() */

#ifdef IBMPC
		/* set up comm interrupt.  Tell it the lowwater mark where 
		   and point where ^s will be sent */
	init_com(LOWWATER);

		/* set default port.  Needed by chgport for first time thru */
	commport = 0;
	Dport = DPORT;

#endif
} /* onceonly */


/****************************************************************************
FUNCTION:
	init resets all variables to their "standard" values (whatever that
	is ...).  Init is called at the beginning of yam and when the "init"
	command is given.

CALLING PARAMETERS:
	none
===========================================================================*/
init()
{
		/* init user terminal */
	terminit();

		/* default xon-xoff, incase config file messed up */
	xon = 0x11; /* ^q */
	xoff = 0x13; /* ^s */

	tabstop = TABS;
	Mstatus=0;	
	Rfile=Tfile=Pflag=FALSE;
#ifdef Cis02
	Cis02=FALSE;
#endif
	Waitbunch=Exoneof=Hdx=Zeof=Squelch=FALSE;
	Txgo=TRUE;
	Originate= TRUE;
	Txeoln= EOL_NOTHING;
	Tpause=1500*CLKMHZ;
	Throttle=22*CLKMHZ;
	Waitnum=1;
	GOchar= '\n';

		/* init buffer pointers.  This must be done before any access to
		   modem.  If not characters gotten from the interrupt routine
		   will be placed in random locations */
	clearbuff();

		/* get default disk */
	defdisk = bdos(GETDEFDISK,0);

#ifdef XMODEM
	if((fin=fopen(LASTCALR,"rb"))!=NULL)
	{
		fgets(Phone, 100, fin);
		fclose(fin);
	}
#endif

		/* read config file, used phoneline buffer as scratch */
	read_config(Phone);

		/* initialize Phone to blank line */
	Phone[0] = 0;

		/* set up default port number to use */
	chngport(0);

		/* allows extra user externs to be initialized */
	userinit(FALSE);

} /* init */


/****************************************************************************
FUNCTION:
	pause n 1/10s of a sec.  NOTE: this is implemented as a software loop
	and will be compiler and processor dependent

CALLING PARAMETERS:
	n:
		number of 1/10s of a sec to pause
===========================================================================*/
sleep(n)
int n;
{
	int i,j;
	for(i=0; i<n; i++)
		for(j=0; j<(LOOPS); j++);
} /* sleep */
