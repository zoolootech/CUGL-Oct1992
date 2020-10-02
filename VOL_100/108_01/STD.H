
/**********************************************************
 ***							***
 ***	Copyright (c) 1981 by David M. Fogg		***
 ***							***
 ***		2632 N.E. Fremont			***
 ***		Portland, OR 97212			***
 ***							***
 ***	    (503) 288-3502{HM} || 223-8033{WK}		***
 ***							***
 ***	Permission is herewith granted for non- 	***
 ***	commercial distribution through the BDS C	***
 ***	User's Group; any and all forms of commercial   ***
 ***	redistribution are strenuously unwished-for.	***
 ***							***
 **********************************************************/

/* ---> The BDS C Standard I/O header file	v1.4	July 18, 1980

   --> STD.H <--- (personalization of BDSCIO.H) 15 OCT 80

   23 Oct: add OR, ORR, LST, TAB
   25 Oct: add FOREVER
   26 Oct: add BDOS stuff (DCIO/OFLAG)
   21 Nov: add ICON, OCON
   22 Nov: add MAXNAM
   25 Nov: add BIOS #defs
   29 Nov: chg BOOL->char; add BYTE: char
   6 Feb 81: fix CUROFF, CURON; VSIZ 25->24
   5 Mar: add HOME, DELCH
*/

/*******   Console Characteristics:   *******/

#define CSTAT	16	/* Console status port	*/
#define CDATA	17	/* Console data port	*/
#define CIMASK	 1	/* Console input data ready mask   */
#define COMASK	 2	/* Console output data ready mask  */
#define CAHI	1	/* True if console status active high	*/
#define CRESET	0	/* True if status port needs to be reset  */
#define CRESETVAL 0	/* If CRESET is true, this is the value to send */

/*---> Z-19 Terminal Parameters & Control Strings <---*/

#define HSIZ	80	/* # of columns */
#define VSIZ	24	/* # of lines	*/

#define BLINON	"\033x1"	/* enable 25th line */
#define CLEAR	"\033E" 	/* clear screen */
#define CURDN	"\033B" 	/* move cursor down */
#define CURLEF	"\033D" 	/* move cursor left */
#define CUROFF	"\033x5"	/* turn cursor OFF */
#define CURON	"\033y5"	/*  "     "    ON */
#define CURRIT	"\033C" 	/* move cursor right */
#define CURUP	"\033A" 	/* move cursor up */
#define DELCH	"\033N" 	/* del char & close up line */
#define DELIN	"\033M" 	/* delete current line */
#define EREOL	"\033K" 	/* erase to End-Of-Line */
#define GRAOFF	"\033G" 	/* turn OFF grafix mode */
#define GRAON	"\033F" 	/*  "   ON    "     "   */
#define HOME	"\033H" 	/* home cursor: upleft corner */
#define INCHOF	"\033O" 	/*  "   OFF insert char mode */
#define INCHON	"\033@" 	/*  "   ON    "     "    "   */
#define INSLIN	"\033L" 	/* insert a line */
#define PADNOR	"\033y6"	/* set keypad unshifted */
#define POCRT	"\033z" 	/* reset term to POC status */
#define RESCUR	"\033k" 	/* restore cursor to saved pos */
#define SAVCUR	"\033j" 	/* save cursor position */
#define VIDINV	"\033p" 	/* set video inverse */
#define VIDNOR	"\033q" 	/*  "    "   normal */

#define CURLEA	"\033Y" 	/* POsition CUrsor Lead-In */

/*---> Handy ASCII Characters <---*/

#define BEL	'\007'
#define ESC	'\033'
#define FFEED	'\014'
#define TAB	'\011'

#define OR	|		/* for compatibility w/ VG vrsn */
#define ORR	||		/* (MT can't gen | char (!!!) */


/* ---> BIOS functions <--- */

#define CONST  2     /* CONsole STatus (A->00/FF: NO/YES) */
#define CONIN  3     /* CONsole INput (ret'd in A) */
#define CONOUT 4     /* CONsole OUTput (char in C) */
#define LIST   5     /* LIST device output (char in C) */
#define PUNCH  6     /* PUNCH output (char in C) */
#define READER 7     /* READER input (ret'd in A) */
#define HOME   8     /* HOME selected disk head */
#define SELDSK 9     /* SELect DiSK in reg C (A=0, B=1...) */
#define SETTRK 10    /* SET TRacK in reg C (0..76) */
#define SETSEC 11    /* SET SECtor in reg C (1..26) */
#define SETDMA 12    /* BC=0080H (usually) */
#define SREAD  13    /* Sector READ */
#define SWRITE 14    /* Sector WRITE */
#define DMADDR 0x80  /* standard DMA ADDRess */


/* ---> BDOS functions <--- */

#define DCIO	  6	      /* Direct Console I/O (non-echoing) */
#define OFLAG	  0xFF	      /* Output mode Flag for DCIO */

/**********************************************************************
	General purpose Symbolic constants:
***********************************************************************/

#define BASE 0		/* Base of CP/M system RAM (0 or 0x4200)  */

#define NULL 0		/* Used by some functions to indicate zilch */

#define READ   0	/* unbuff file I/O mode: input only */
#define WRITE  1	/*   "     "	"   "	 output "   */
#define UPDATE 2	/*   "     "	"   "	 in & out   */

#define EOF -1		/* Physical EOF returned by low level I/O functions */
#define CEOF 255	/* EOF enharmonic if in a char var */

#define ERROR -1	/* General "on error" return value */

#define OK 0		/* General purpose "no error" return value */

#define CPMEOF 0x1a	/* CP/M End-of-text-file marker (sometimes!)  */

#define SECSIZ 128	/* Sector size for CP/M read/write calls */

#define MAXNAM	 15	/* longest CP/M filename */

#define MAXLINE 135	/* Longest line of input expected from the console */

#define TRUE 1		/* general purpose true value */
#define YES  1		/* ditto */

#define FALSE 0 	/* general purpose false value */
#define NO    0 	/* ditto */

#define BOOL  char	/* pseudo-typedef */
#define BYTE  char	/* ditto above */

#define FOREVER   for (;;)

/*******   Number of sectors to use for buffered I/O: ***********
 *								*
 * Make sure you use declare all your I/O buffers with the a	*
 * statement such as:						*
 *		char buf_name[BUFSIZ];				*
 *	(and always #include "dmfio.h" in your programs!)      *
 ****************************************************************/

#define ICON	0	/* "iobuf" of Keyboard */
#define OCON	1	/* "iobuf" of CRT screen */
#define LST	2	/* "iobuf" of Printer */

#define NSECTS	8	/* Number of sectors to buffer up in ram */

#define BUFSIZ (NSECTS * SECSIZ + 6 )	/* Don't touch this */

struct _buf {				/* Or this...	    */
	int _fd;
	int _nleft;
	char *_nextp;
	char _buff[NSECTS * SECSIZ];
};

#define ALLOC_ON 1	/* enables storgage allocation if uncommented */


#ifdef ALLOC_ON 		/* if storage allocation enabled, */

struct _header	{
	struct _header *_ptr;
	unsigned _size;
 };

struct _header _base;		/* declare this external data to  */
struct _header *_allocp;	/* be used by alloc() and free()  */

#endif
