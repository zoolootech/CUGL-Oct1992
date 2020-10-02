/* vi: set ai : */
/* IBM PC BIOS version    written by Rainer Gerhards */
/* CURRENTLY MSC NEEDED!!!! */
/* Only the IBM 360Kb format is supported. There have to be 2 sides, */
/* each side has to have 40 tracks and each track 9 sectors of 512 bytes. */
/* Any other physical format will cause the program to fail. */
#include <stdio.h>
#include <dos.h>

#define	RETRIES	3

int		wrsct(strtsect, sectbuf)
int		strtsect;
char		*sectbuf;
{
int		error = 1;
int		tries;
int		diskstat;
char far	*bufadr = (char far *) sectbuf;
union REGS	regs;
struct SREGS	sregs;

bufadr = (char far *) sectbuf;
for(tries = RETRIES ; tries > 0 ; --tries)	
	{
	regs.h.ah = 3;			/* write sector	*/
	regs.h.al = 1;			/* one sector	*/
	regs.h.dl = 0;			/* drive one	*/
	regs.h.cl = (strtsect % 9) + 1;	/* sector	*/
	regs.h.ch = (strtsect / 2) / 9;	/* track	*/
	regs.h.dh = (((strtsect % 18) / 9) > 0) ? 1 : 0;/* side		*/
	regs.x.bx = FP_OFF(bufadr);	/* data buffer	*/
	sregs.es = FP_SEG(bufadr);	/* data buffer	*/
	int86x(0x13, &regs, &regs, &sregs);		/* call bios	*/
	if(regs.x.cflag)		/* error?	*/
		{
		diskstat = regs.h.ah;	/* save error	*/
		regs.h.ah = 0;		/* reinitialize	*/
		int86x(0x13, &regs, &regs, &sregs);	/* do it!	*/
		}
	else
		{
		tries = 0;		/* end loop	*/
		error = 0;		/* "good" state	*/
		}
	}
if(error == 1)
	fprintf(stderr, "ERROR: disk status 0x%2.2x\n", diskstat);
return(error);
}
