/*
 *	The BDS C Standard I/O header file --  v1.50	12/29/82
 *		MODIFIED 4/27/85 FOR USE WITH C/80
 *
 *	This file contains global definitions, for use in all C programs
 *	in PLACE of (yechhh) CONSTANTS. Characteristics of your system such
 *	buffered I/O allocations, storage allocator state, etc., should all
 *	be configured just once within this file. Any program which needs
 *	them should contain the preprocessor directive:
 *
 *		#include <bdscio.h>
 *
 *	near the beginning. 
 */

#define C80	/* Define C80 if C80 compiler in use else BDS/C */

/*
 *	General purpose Symbolic constants:
 */

#define BASE 0		/* Base of CP/M system RAM (0 or 0x4200)  */
#define NULL 0
#define EOF -1		/* Physical EOF returned by low level I/O functions*/
#define ERROR -1	/* General "on error" return value */
#define OK 0		/* General purpose "no error" return value */
#define JBUFSIZE 6	/* Length of setjump/longjump buffer	*/
#define CPMEOF 0x1a	/* CP/M End-of-text-file marker (sometimes!)  */
#define SECSIZ 128	/* Sector size for CP/M read/write calls */
#define MAXLINE 150	/* Longest line of input expected from the console */
#define TRUE 1		/* logical true constant */
#define FALSE 0		/* logical false constant */
#define BEGIN {
#define THEN {
#define LOOP {
#define ENDIF }
#define ENDLOOP }
#define END }
#define FUNCTION /**/

#ifdef C80
#define STDIN  fin
#define STDOUT fout
#define STDERR 0
#define DIOIN fin
#define DIOOUT fout	/* for C80 fout is equivalent to DIOOUT */
extern int fin, fout;	/* for C80 fin is equivalent to DIOIN */
#else
#define STDIN  0
#define STDOUT 1
#define STDERR 4
#endif

#define STDLST 2
#define STDRDR 3
#define STDPUN 3

/* FOR USE WITH OPEN & buffered BDS IO*/
#ifdef C80
#define READ   "rb"
#define WRITE  "wb"
#define READWRITE "ub"
#define NBLCHAR 128
#define fopen c_open
#define fclose c_close
#define putc c_putc
#define getc c_getc
#define read c_read
#define write c_write
#define seek c_seek
#define fabort close
#else
#define READ   0
#define WRITE  1
#define READWRITE 2
#endif

/*
 *	The NSECTS symbol controls the compilation of the buffered
 *	I/O routines within STDLIB2.C, allowing each user to set the
 *	buffer size most convenient for his system, while keeping
 *	the numbers totally invisible to the C source programs using
 *	buffered I/O (via the BUFSIZ defined symbol.) For larger
 *	NSECTS, the disk I/O is faster...but more ram is taken up.
 *	To change the buffer size allocation, follow these steps:
 *
 *	1) Change NSECTS below
 *	2) Re-compile STDLIB1.C and STDLIB2.C
 *	3) Use CLIB to combine STDLIB1.CRL and STDLIB2.CRL to make
 *	   a new DEFF.CRL. This isn't as touch as it sounds.
 *
 *	Make sure you declare all your I/O buffers with the a
 *	statement such as:
 *
 *		char buf_name[BUFSIZ];
 */

#ifndef C80
#define NSECTS 8	/* Number of sectors to buffer up in ram */

#define BUFSIZ (NSECTS * SECSIZ + 7)	/* Don't touch this */

struct _buf {				/* Or this...	    */
	int _fd;
	int _nleft;
	char *_nextp;
	char _buff[NSECTS * SECSIZ];
	char _flags;
};
#define FILE struct _buf	/* Poor man's "typedef" */
#define _READ 1
#define _WRITE 2

#else
#define NSECTS 1
#define BUFSIZ 1
#define FILE int	/* for C80 define FILE as an int then 
			iobuf is used as a pointer in C80 versions
			of buffered IO functions */
#endif

#ifdef C80
#define wildexp command	/* expand wild card names in command line */

/* C/80 definitions for printf, fprintf and sprintf 
	to allow multiple args. */

#define printf prnt_1(),prnt_2
#define fprintf prnt_1(),prnt_3
#define sprintf prnt_1(),prnt_4
#define typef prnt_1(),prnt_5

/* Header file for scanf */
#undef scanf
#undef fscanf
#undef sscanf
#define scanf STK_pos(),scan_f
#define fscanf STK_pos(),f_scan
#define sscanf STK_pos(),s_scan

#else

struct _long {char _lc[2];};
struct _float {char _ft[5];};
#define LONG struct _long	/* LONG typedef */
#define FLOAT struct _float	/* FLOAT typedef */
#define REAL struct _float	/* REAL typedef */

/*
 * Storage allocation data, used by "alloc" and "free"
 */

struct _header  {
	struct _header *_ptr;
	unsigned _size;
 };

struct _header _base;		/* declare this external data to  */
struct _header *_allocp;	/* be used by alloc() and free()  */
#endif
