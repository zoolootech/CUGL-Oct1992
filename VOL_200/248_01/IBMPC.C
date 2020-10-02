/*	IBMPC:	IBMPC I/O routines for MicroSPELL 1.0
		Spell Checker and Corrector

		(C)opyright May 1987 by Daniel Lawrence
		All Rights Reserved
*/

#include	<stdio.h>
#include	"dopt.h"
#include	"dstruct.h"
#include	"ddef.h"

#if	IBMPC
union REGS rg;			/* cpu register for use of DOS calls */

TTopen()		/* open the IBM-PC screen */

{
	return(TRUE);
}

TTclose()		/* close the IBM-PC screen */

{
	return(TRUE);
}

TTkopen()	/* open the keyboard */

{
	return(TRUE);
}

TTkclose()	/* close the keyboard */

{
	return(TRUE);
}

TTgetc(c)	/* get a character from the keyboard */

char c;		/* character to print */

{
	/* call the dos to get a char */
	rg.h.ah = 7;		/* dos Direct Console Input call */
	intdos(&rg, &rg);
	c = rg.h.al;		/* grab the char */
	return(c & 255);
}

TTputc(c)

char c;		/* character to print */

{
	bdos(6, c, 0);
}

TTflush()		/* flush the I/O  (nop) */

{
	return(TRUE);
}

TTmove(row, col)	/* move cursor to location on screen */

int row, col;

{
	rg.h.ah = 2;		/* set cursor position function code */
	rg.h.dl = col;
	rg.h.dh = row;
	rg.h.bh = 0;		/* set screen page number */
	int86(0x10, &rg, &rg);
}

TTbeep()	/* ring the bell */

{
	TTputc(7);
}

#if	0
#define	TTeeol		(*term.t_eeol)
#define	TTeeop		(*term.t_eeop)
#define	TTrev		(*term.t_rev)
#define	TTrez		(*term.t_rez)
#if	COLOR
#define	TTforg		(*term.t_setfor)
#define	TTbacg		(*term.t_setback)
#endif
#endif

#endif
