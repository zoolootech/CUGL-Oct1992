/* HEADER: CUG270;
   TITLE: Video Header File
   DATE: 04/11/1988
   DESCRIPTION: Various video functions
   VERSION: 1.0
   KEYWORDS: VIDEO, BIOS FUNCTIONS
   FILENAME: VIDEO.H
   WARNINGS: Dependant on ROM BIOS
   SEE-ALSO: VID_DEMO.C
   SYSTEM: MS-DOS
   COMPILERS: Turbo C & Microsoft C v4.0
   AUTHORS: Marc Bosser

   	Comments, questions, bugs & suggestions
	Monday, Wedndesday, Friday, Sunday
	Contact ME: Marc Bosser
		    1922 Julia Goldbach
		    Ronkonkoma, NY 11779
*/

		/************************************************/
		/*   File Name:VIDEO.H					   */
		/*   C File:3,573 bytes					   */
		/*   Portability:Direct ROM BIOS calls		   */
		/*									   */
		/* CONTAINS:xy(y,x), ewriteM(y,x,char,color,len)*/
		/*	    fill(y1,sr,len,er,color),cursoroff()   */
		/* Total: 4 routines					   */
		/* External: int86, cprintf				   */
		/* User Defined: xy,fill,ewriteM			   */
		/************************************************/

#define	MAXLENGTH		80		/* MAXLENGTH OF STRING PASSED TO EWRITEM */
#define	VIDEO_BIOS	0x10		/* IBM PC VIDEO BIOS INTERRUPT NUMBER */

#ifndef NULL					/* DEFINE NULL CHARACTER IF NOT DEFINED */
#define NULL 0
#endif

/* NOTE:  ALL ROUTINES DO NOT CHECK FOR VALID SCREEN PARAMETERS */
/* Requires that DOS.H be called before compiling this module */

#ifdef __TURBOC__
void xy(unsigned char y,unsigned char x);
void cursoroff(void);
void ewriteM(unsigned char y,unsigned char x,unsigned char ch,unsigned short len);
void fill(unsigned char y1,unsigned char x1,unsigned char len,unsigned char x2,
		unsigned char color);
#endif

/******************************************************************************/

void xy(y,x)
unsigned char y;
unsigned char x;

	/* LOCATES THE CURSOR AT POSITION Y,X */
	/* ACTUAL SCREEN COORDS START AT 0,0 BUT FUNCTION SUBTRACTS ONE */
	/* LEGAL VALUES FOR Y ARE 1 TO 80 & X FROM 1 TO 25 */
	/* Portability: Depends on ROM BIOS call 2 */

{
 union REGS r;

 r.h.ah=2;
 r.h.bh=0;
 r.h.dl=--y;
 r.h.dh=--x;
 int86(VIDEO_BIOS,&r,&r);
}

/******************************************************************************/

void cursoroff()

	/* TURNS THE CURSOR OFF BY PLACING IT OFF THE SCREEN AT (0,25) */
	/* Portability: Depends on ROM BIOS call 2 */

{
 union REGS r;

 r.h.ah=2;
 r.h.bh=0;
 r.h.dl=0;
 r.h.dh=25;
 int86(VIDEO_BIOS,&r,&r);
}

/******************************************************************************/

void ewriteM(y,x,ch,len)
unsigned char y;
unsigned char x;
unsigned char ch;
unsigned short len;

	/* PRINTS A STRING BY ADDING ALL THE CHARACTERS TOGETHER */
	/* ewriteM:extended write many */
	/* Portability:VANILLA */

{
 register unsigned short loop=0;
 unsigned char message[MAXLENGTH];

 do {
    message[loop]=ch;
    loop++;
 } while (loop < len);
 message[loop]=NULL;
 xy(y,x);
 cprintf(message);
}

/******************************************************************************/

void fill(y1,x1,len,x2,color)
unsigned char y1;
unsigned char x1;
unsigned char len;
unsigned char x2;
unsigned char color;

	/* FILLS AN AREA OF THE SCREEN IN THE PARAMETER COLOR */
	/* Portability: Depends on ROM BIOS call 6 */

{
 union REGS r;

 r.h.ah=6;
 r.h.al=0;
 r.h.bh=color;
 r.h.ch=--x1;
 r.h.cl=--y1;
 r.h.dh=x2;
 r.h.dl=y1+len;
 int86(VIDEO_BIOS,&r,&r);
}

/******************************************************************************/

					/**** END OF VIDEO.H ****/
