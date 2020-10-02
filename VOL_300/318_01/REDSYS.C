/*
	RED operating system module -- Full C version

	Source:  redsys.c
	Version: June 18, 1986; January 18, 1990.

	Written by
	
		Edward K. Ream
		166 N. Prospect
		Madison WI 53705
		(608) 257-0802


	PUBLIC DOMAIN SOFTWARE

	This software is in the public domain.

	See red.h for a disclaimer of warranties and other information.
*/

#include "red.h"

#ifdef MICRO_SOFT
#include <bios.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys\types.h>
#include <sys\stat.h>
#endif

#ifdef TURBOC
#include <alloc.h>
#include <conio.h>
#include <fcntl.h>
#include <io.h>
#include <mem.h>
#include <sys\stat.h>
#endif

static char sysinbuf[128];		/* file buffer		*/
static int  sysincnt;			/* index to sysinbuf[]	*/
static char syscbuf[MAXLEN];		/* typeahead buffer	*/
static int  sysccnt;			/* index for syscbuf[]	*/
static int  sysrcnt;			/* repeat count		*/
static int  syslastc;			/* last character	*/
static int  systopl,systopy,sysnl;	/* interrupt info	*/

/*
	NOTE:  This module should contain all routines that
	might have to be changed for different compilers or
	different operating systems.  

	Some routines in this module probably will probably
	work regardless of operating system.  These I have
	called PORTABLE routines.
*/

/*
	Return a pointer to a block of n contiguous bytes.
	Return NULL (i.e.,  0) if fewer than n bytes remain.
*/
char *
sysalloc(n)
int n;
{
	char * p;
	p = (char *) malloc(n);

	TRACEP("sysalloc",
		sl_lpout(); sl_iout(n);
		sl_sout(") returns "); sl_pout(p); sl_cout('\n'));

	return p;
}

/*
	o  Wait for next character from the console.
	o  Do NOT echo the character.
	o  Print any waiting lines if there is no input ready.
        o  Swap out any dirty blocks if nothing else is happening.
	   (The SWAP constant enables this feature)
*/
int
syscin(void)
{
	int c;
	int main_byte, aux_byte;

	SL_DISABLE();

	for(;;) {
		c = syscstat();
		if (c != -1) {
			break;
		}

		/* Output queued ? */
		if (hasint && sysnl > 0) {
			/* Print one line. */
			bufout(systopl, systopy, sysnl);
		}

#ifdef SWAP
		/* Do not interrupt screen activity. */
		if (sysnl == 0) {
			/* Swap out one dirty buffer. */
			swap_one();
		}
#endif

	}

#ifdef IBM

	main_byte = c & 0xff;
	aux_byte  = (c & 0xff00) >> 8;

	TRACEP("syscin",
		sl_sout(" main: "); sl_iout(main_byte);
		sl_sout(" aux: ");  sl_iout(aux_byte);
		sl_cout('\n'));

	/* Handle cursor keys and the numeric keypad. */
	if (main_byte == 0) switch (aux_byte) {

		case 71: return HOME_KEY;	 /* Home key.     */
		case 79: return END_KEY; 	/* End key.       */

		case 73: return PAGE_UP;	/* Page up key.   */
		case 81: return PAGE_DN;	/* Page down key. */
		case 82: return UP_INS;		/* Insert key     */
		case 83: return DEL2;		/* Delete Key     */

		case 77:  return RIGHT;		/* Arrow keys.    */
		case 75:  return LEFT;
		case 72:  return UP;
		case 80:  return DOWN;
	}

#endif

	return c & 0x7f;
}

/*
	Close an unbuffered file.
	Return OK (0) or ERROR (-1).
*/
int
sysclose(int fd)
{
	TRACEP("sysclose", sl_lpout(); sl_iout(fd); sl_rpout());

	close(fd);
}

/*
	Copy a file name from args to buffer.
	This routine is SEMI-PORTABLE,  since it depends,
	to a certain extent,  on what constitutes a file name.
*/
void
syscopfn(char *args, char *buffer)
{
	int n;

	TRACEP("syscopfn",       sl_lpout();
		sl_sout(args);   sl_csout();
		sl_pout(buffer); sl_rpout());

	for (n = 0;
	     n < SYSFNMAX -1 && args [n] != '\0' && args [n] != ' ';
	     n++) {

		buffer [n] = args [n];
	}
	buffer[n] = '\0';
}

/*
	Create an file for use by sysread and syswrite.
	Erase it if it exists and leave it open for read/write access.
	Return a small positive int or ERROR  (-1).
*/
int
syscreat(char * filename)
{
	int result;

	TRACEP("syscreat", sl_lpout(); sl_sout(filename));

#ifdef MICRO_SOFT
	_fmode = O_BINARY;
	result = creat(filename, S_IREAD | S_IWRITE);
	_fmode = O_TEXT;
#endif

#ifdef TURBOC
	_fmode = O_BINARY;
	result = creat(filename, S_IREAD | S_IWRITE);
	_fmode = O_TEXT;
#endif

	TRACE("syscreat",
		sl_sout(") returns "); sl_iout(result); sl_cout('\n'));

	return result;
}

/*
	Return -1 if no character is ready from the keyboard.
	Otherwise, return the character itself.

	This routine handles typeahead buffering.  It would
	also handle the repeat key,  if that feature is used,
	which it is NOT at present.  The code enclosed in
	comments handles the repeat key.
*/
int
syscstat(void)
{
	int c;

	SL_DISABLE();

	/* Always look for another character. */

#ifdef MICRO_SOFT

	c = _bios_keybrd(_KEYBRD_READY);
	if (c == 0) {
		c = -1;
	}
	else {
		_bios_keybrd(_KEYBRD_READ);
	}
#endif

#ifdef TURBOC

	c = bioskey(1);
	if (c == 0) {
		c = -1;
	}
	else {
		bioskey(0);
	}
#endif

	return c;
}


/*
	Execute args as if they were entered from the command line.
*/
void
sysexec(char *args)
{

	SL_DISABLE();

#ifdef MICRO_SOFT
	system(args);
	puts("\nPress any key to resume editing: ");
	syscin();
#endif

#ifdef TURBOC
	system(args);
	puts("\nPress any key to resume editing: ");
	syscin();
#endif

}

/*
	Return TRUE if the file exists and FALSE otherwise.
	This routine is PORTABLE.
*/
int
sysexists(char * filename)
{
	int fd;

	TRACEPB("sysexists", sl_lpout(); sl_sout(filename); sl_rpout());

	if ((fd = sysopen(filename)) != ERROR) {
		sysclose(fd);

		RETURN_BOOL("sysexists", TRUE);
	}
	else {
		RETURN_BOOL("sysexists", FALSE);
	}
}

/*
	Close and flush a file opened with sysfopen or sysfcreat.
	Return OK (0) or ERROR (-1).
*/
int
sysfclose(FILE *fd)
{
	TRACEP("sysfclose", sl_lpout(); sl_pout(fd); sl_rpout());

	return fclose(fd);
}

/*
	Create a buffered output file for use with sysfputs.

	The file is cleared (erased) if it already exists.
	A pointer to FILE is returned if all goes well.
	NULL (0), is returned if the file can not be created.
*/
FILE *
sysfcreat(char *filename)
{
	FILE * f;

	TRACEP("sysfcreat", sl_lpout();	sl_sout(filename));

#ifdef MICRO_SOFT
	f = fopen(filename, "w");
#endif

#ifdef TURBOC
	f = fopen(filename, "w");
#endif

	TRACE("sysfcreat",
		sl_sout(") returns "); sl_pout(f); sl_cout('\n'));

	return f;
}

/*
	Read the next line from buffered input file.
	End the line with an '\n'.
	Return the count of the characters read.
	Return ERROR (-1) on end-of-file.
*/
int
sysfgets(FILE *fd, char *buffer, int maxlen)
{
	int c, count;

	TRACEP("sysfgets",      sl_lpout();
		sl_pout(fd);     sl_csout();
		sl_pout(buffer); sl_csout(); 
		sl_iout(maxlen));

	count = 0;
	while(1) {
		c = sysgetc(fd);
		if (c == '\r') {
			/* Ignore pseudo newline. */
			continue;
		}
		else if (c == EOF_MARK || c == ERROR) {
			buffer [min(count, maxlen-2)] = '\n';
			buffer [min(count, maxlen-1)] = '\0';

			TRACE("sysfgets", sl_sout(") returns ERROR\n"));
			return ERROR;
		}
		else if (c == '\n') {
			buffer [min(count, maxlen-2)] = '\n';
			buffer [min(count, maxlen-1)] = '\0';
			TRACE("sysfgets",
				sl_sout(") returns ");
				sl_iout(count);
				sl_cout('\n'));
			return count;
		}
		else if (count < maxlen - 2) {
			buffer [count++] = c;
		}
		else {
			count++;
		}
	}
}

/*
	Write n BYTES from the buffer to the END of an unbuffered file.
	Return ERROR (-1) or 1.
*/
int
sysflush(int fd, char * buffer, int n)
{
	int result;

#ifdef MICRO_SOFT
	result = (write(fd, buffer, n) == -1) ? ERROR : 1;
#endif

#ifdef TURBOC
	result = (write(fd, buffer, n) == -1) ? ERROR : 1;
#endif

	return result;

}

/*
	Open a buffered input file for use by sysfgets and sysgetc.

	Return FILE or NULL if the file does not exist.
*/
FILE *
sysfopen(char *filename)
{
	FILE * f;

#ifdef MICRO_SOFT
	f = fopen(filename, "r");
#endif

#ifdef TURBOC
	f = fopen(filename, "r");
#endif

	return f;
}

/*
	Get the next characer from a file opened by sysfopen.
	Mask off the high bit of the character.
	return ERROR on end of file.
*/
int
sysgetc(FILE *fd)
{
	int c;

	SL_DISABLE();

	c = getc(fd);

	/* Be careful not to mask off bit on EOF. */
	return ((c == ERROR) ? ERROR : c & 0x7f);

}

/*
	Initialize the system module.

	The storage allocation routines are a real problem.
	The AZTEC and DRI compilers completely botch it.
*/
void
sysinit(void)
{
	TICK("sysinit");

	/*
		System dependent part.
		This code sets the "moat" (in Unix terminology),
		i.e., the amount of space that is guarenteed for
		use by the stack by the memory allocation routines.
		If you don't reserve enough space for the stack
		the inject, extract, move and copy commands will crash.
	*/

#ifdef DESMET
	scr_setup();
	freeall(4000);
#endif

#ifdef TURBOC
	/* Use raw input. */
	setcbrk(0);
#endif

	/*
		PORTABLE part.
	*/
	sysnl    = 0;
	sysccnt  = 0;
	sysrcnt  = 0;
	syslastc = 0;
}

/*
	Save info for interrupted screen update.
	This routine is PORTABLE.
*/
void
sysintr(int systl, int systy, int sysn)
{
	SL_DISABLE();

	systopl = systl;
	systopy = systy;
	sysnl   = max(0,sysn);
}

/*
	Print character on the printer.
*/
int
syslout(char c)
{
	SL_DISABLE();

#ifdef DESMET
	_os(5, c);	
#endif

	return c;
}

/*
	Move a block of memory.

	This code MUST work regardless of whether the
	source pointer is above or below the dest pointer.

	The for loop below is PORTABLE,  but this is time-
	critical code;  it should work as fast as possible.
	It may be worth your while to rewrite it in assembly
	language.
*/
void
sysmove(char source [], char dest [], int count)
{

#ifdef TURBOC
	movmem(source, dest, count);
#endif

#ifdef MICRO_SOFT
	memmove(dest, source, (int) count);
#endif

#ifndef MICRO_SOFT
#ifndef TURBOC

	int i;

	TRACEP("sysmove",        sl_lpout();
		sl_pout(source); sl_csout();
		sl_pout(dest);	 sl_csout(); 
		sl_iout(count);	 sl_rpout());

	if (count <= 0) {
		return;
	}
	else if (source > dest) {
		/* Copy head first. */
		for (i = 0; i < count; i++) {
			dest [i] = source [i];
		}
	}
	else {
		/* Copy tail first. */
		for (i = count - 1; i >= 0; i--) {
			dest [i] = source [i];
		}
	}
#endif
#endif

}

/*
	Open an existing file for unbuffered i/o.
	Return OK (0) or ERROR (-1).
*/
int
sysopen(char *name)
{
	int result;

	SL_DISABLE();

#ifdef MICRO_SOFT
	result = open(name, O_RDWR | O_BINARY);
#endif

#ifdef TURBOC
	result = open(name, O_RDWR | O_BINARY);
#endif

	return result;
}

/*
	Put one character to a buffered output file.
	Returns the character itself,  or ERROR if the
	disk becomes full.
*/
int
sysputc(char c, FILE *fd)
{
	SL_DISABLE();

	return putc(c, fd);
}

/*
	Read one block (DATA_SIZE bytes) from an unbuffered
	file into the buffer of size DATA_SIZE.
	Return the number of bytes read or -1 if an
	end-of-file occurs immediately.
	If less than a full block is read,  put an EOF_MARK mark
	after the last byte.
*/
int
sysread(int fd, char * buffer)
{
	int n, result;

	TRACEP("sysread",        sl_lpout();
		sl_iout(fd);     sl_csout();
		sl_pout(buffer));

#ifdef MICRO_SOFT
	n = read(fd, buffer, DATA_SIZE);
#endif

#ifdef TURBOC
	n = read(fd, buffer, DATA_SIZE);
#endif

	if (n == ERROR) {
		result = 0;
	}
	else if (n == DATA_SIZE) {
		result = n;
	}
	else {
		/* Force an end-of-file mark */
		buffer [n] = EOF_MARK;
		result = n;
	}

	TRACE("sysread",
		sl_sout(") returns "); sl_iout(result); sl_cout('\n'));

	return result;
}

/*
	Rename the old file to be the new file.
	RED makes sure that the file is NOT open when
	sysrename() is called.
*/
void
sysrename(char *oldname, char *newname)
{
	TRACEP("sysrename",       sl_lpout();
		sl_sout(oldname); sl_csout(); 
		sl_sout(newname); sl_rpout());

	rename(oldname, newname);
}

/*
	Seek to the specified block of an unbuffered file.
	Return OK (0) or ERROR (-1).
*/
int
sysseek(int fd, int block)
{
	int result;
	long lresult;
	long temp;

	TRACEP("sysseek",
		sl_lpout(); sl_iout(fd); sl_csout(); sl_iout(block));

#ifdef MICRO_SOFT
	temp = ((long) block) * DATA_SIZE;
	lresult = lseek(fd, temp, SEEK_SET);
	result = (lresult == -1L) ? ERROR : OK;
#endif

#ifdef TURBOC
	/*
		The following line is a workaround a bug in the compiler.
		Without the cast, the code does not work if block == 32.
	*/
	temp = ((long) block) * DATA_SIZE;
	lresult = lseek(fd, temp, 0);
	result = (lresult == -1L) ? ERROR : OK;
#endif

	TRACE("sysseek",
		sl_sout(") returns "); sl_lout(lresult); sl_cout('\n'));

	return result;
}

/*
	Write n bytes from the buffer to an unbuffered file.
	Return the number of bytes written or ERROR (-1).
*/
int
syswrite(int fd, char * buffer, int n)
{
	int result;

	TRACEP("syswrite", sl_lpout();
		sl_iout(fd);     sl_csout();
		sl_pout(buffer); sl_csout(); 
		sl_iout(n));

#ifdef MICRO_SOFT
	result = write(fd, buffer, n);
#endif

#ifdef TURBOC
	result = write(fd, buffer, n);
#endif

	TRACE("syswrite",
		sl_sout(") returns "); sl_iout(result); sl_cout('\n'));

	return result;
}

/*
	Erase the file from the file system.
	Return ERROR (-1) if the file does not exist or
	can not be erased.
*/
int
sysunlink(char * filename)
{
	TRACEP("sysunlink", sl_lpout(); sl_sout(filename); sl_rpout());

	unlink(filename);
}

/*
	Wait for all console output to be finished.
	This routine is PORTABLE.
*/
void
syswait(void)
{
	SL_DISABLE();

	while (hasint && sysnl > 0) {
		bufout(systopl, systopy, sysnl);
	}
}
