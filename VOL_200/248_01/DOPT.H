/*	DOPT:	Option settings for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

#ifdef	LATTICE
#undef	LATTICE		/* don't use their definitions...use ours	*/
#endif
#ifdef	MSDOS
#undef	MSDOS
#endif
#ifdef	CPM
#undef	CPM
#endif
#ifdef	AMIGA
#undef	AMIGA
#endif
#ifdef	EGA
#undef	EGA
#endif

#define	PROGNAME	"MicroSPELL"
#define	VERSION		"1.0"

/*	Machine/OS definitions			*/

#define AMIGA   0                       /* AmigaDOS			*/
#define ST520   0                       /* ST520, TOS                   */
#define MSDOS   1                       /* MS-DOS                       */
#define V7      0                       /* V7 UNIX or Coherent or BSD4.2*/
#define	BSD	0			/* UNIX BSD 4.2	and ULTRIX	*/
#define	USG	0			/* UNIX system V		*/
#define VMS     0                       /* VAX/VMS                      */
#define CPM     0                       /* CP/M-86                      */
#define	FINDER	0			/* Macintosh OS			*/
#define CMS     0                       /* IBM VM/CMS                   */

/*	Compiler definitions			*/
#define MWC86   0	/* marc williams compiler */
#define	LATTICE	0	/* Lattice 2.14 thruough 3.0 compilers */
#define	AZTEC	1	/* Aztec C 3.20e */
#define	MSC	0	/* MicroSoft C compiler version 3 & 4 */
#define TURBO	0	/* Turbo C version 1.00 */

/*	Debugging options	*/
#define	RAMSIZE	0	/* dynamic RAM memory usage tracking */
#define	RAMSHOW	0	/* auto dynamic RAM reporting */

/*	Terminal Output definitions		*/

#define ANSI    0			/* ANSI escape sequences	*/
#define	HP150	0			/* HP150 screen driver		*/
#define	HP110	0			/* HP110 screen driver		*/
#define	VMSVT	0			/* various VMS terminal entries	*/
#define VT52    0                       /* VT52 terminal (Zenith).      */
#define VT100   0                       /* Handle VT100 style keypad.   */
#define RAINBOW 0                       /* Use Rainbow fast video.      */
#define TERMCAP 0                       /* Use TERMCAP                  */
#define	IBMPC	1			/* IBM-PC CGA/MONO/EGA driver	*/
#define	DG10	0			/* Data General system/10	*/
#define	TIPC	0			/* TI Profesional PC driver	*/
#define	Z309	0			/* Zenith 100 PC family	driver	*/
#define	MAC	0			/* Macintosh			*/
#define	ATARI	0			/* Atari 520/1040ST screen	*/
#define IBM3270 0                       /* IBM 3270 terminal device     */

/*	Configuration options	*/

#define	REVSTA	1	/* Status line appears in reverse video		*/
#define	COLOR	1	/* color commands and windows			*/

#define	CRYPT	0	/* file encryption enabled?			*/
#define	EMACS	1	/* Allow the use of an editor for replacement scan*/
#define	CMPRS	1	/* main dictionary compression enabled		*/

#define ASCII	1	/* always using ASCII char sequences for now	*/
#define EBCDIC	0	/* later IBM mainfraim versions will use EBCDIC	*/

/*	System dependant library redefinitions, structures and includes	*/

#define	FLAG	int

#if	TURBO
#undef LATTICE
#define	LATTICE	1
#endif

#if	CMS
/* EXIT() seems to not work as documented, while RETURN() does... */
#define	exit(a)	return(a)
#define	fopen	cmsopen
FILE *cmsopen();
#endif

#if	AZTEC & MSDOS
#undef	fputc
#undef	fgetc
#undef	fgets
#define	fgetc	a1getc
#define	fgets	a1gets
#define	fputc	aputc
#define	int86	sysint
#define	intdos(a, b)	sysint(33, a, b)
#define	inp	inportb
#define	outp	outportb

struct XREG {
	int ax,bx,cx,dx,si,di;
};

struct HREG {
	char al,ah,bl,bh,cl,ch,dl,dh;
};

union REGS {
	struct XREG x;
	struct HREG h;
};
#endif

#if	MSDOS & MWC86
#include	<dos.h>
#define	int86(a, b, c)	intcall(b, c, a)
#define	inp	in

struct XREG {
	int ax,bx,cx,dx,si,di,ds,es,flags;
};

struct HREG {
	char al,ah,bl,bh,cl,ch,dl,dh;
	int ds,es,flags;
};

union REGS {
	struct XREG x;
	struct HREG h;
};
#endif

#if	MSDOS & MSC
#include	<dos.h>
#include	<memory.h>
#define	peek(a,b,c,d)	movedata(a,b,FP_SEG(c),FP_OFF(c),d)
#define	poke(a,b,c,d)	movedata(FP_SEG(c),FP_OFF(c),a,b,d)
#define	movmem(a, b, c)		memcpy(b, a, c)
#endif

#if	MSDOS & LATTICE
#undef	CPM
#undef	LATTICE
#include	<dos.h>
#undef	CPM
#endif

#if	VMS
#define	unlink(a)	delete(a)
#endif

/*	define memory mapped flag	*/

#define	MEMMAP	0

#if	IBMPC
#undef	MEMMAP
#define	MEMMAP	1
#endif

#if	Z309
#undef	MEMMAP
#define	MEMMAP	1
#endif

/*	internal constants	*/

#define	TRUE	1
#define	FALSE	0

#define NFILEN  80                      /* # of bytes, file name        */
#define NLINE   256                     /* # of bytes, input line       */
#define	NSTRING	128			/* # of bytes, string buffers	*/
#define	MAXLINE	1024			/* max line length of source file */
#define HUGE    1000                    /* Huge number                  */
#define	NCOLORS	8			/* number of supported colors	*/
#define	MAXCOM	1000			/* max # of common words	*/
#define	MAXSPELL 8			/* max # of files to spell	*/
#define	MAXWORDS 1000			/* max # of source words/bucket	*/
#define	NBUFSIZ	1024			/* file buffer size for decomp	*/

/*	exit codes	*/

#define	EXGOOD		0		/* successfull run!		*/
#define	EXMDICT		1		/* missing dictionary		*/
#define	EXBADOPT	2		/* bad command line option	*/
#define	EXNORAM		3		/* out of RAM			*/
#define	EXTEMP		4		/* can't open temp file		*/
