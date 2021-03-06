/*
	.title inkey
;+
; index Read character from the keyboard
;
; usage
;	int inkey(&chara, &scancode, nowait)
;
; in
;   	int nowait;
;
; out
; 	char chara;
;	int  scancode;
;
; Description
;
;    this function reads the keyboard. The read can be wait or
;    nowait. With wait I/O a key is always read and the function always
;    returns with a true value. With no-wait if no key was pressed the
;    function would return with a false value. If a key is pressed in
;    no-wait mode, a wait read with guaranteed success is issued to
;    make sure that the keyboard buffer is updated accordingly.
;
;    When a key is read, a character code in character form , and a scan
;    code in int form is returned. See the Technical Reference Manual
;    pages 2-16 thru 2-17, and Appendix C for the codes. For the function
;    keys and sometimes the numeric key pad, there may not be a character
;    code returned, the value inserted is the null character, or 0
;
; status
;
; bugs
;
; updates
;	Date	Vers	Who	Description
;	8/31/83 0001	CMC	Initial version
;
;
;   Communications Programming Inc.
;   C. M. Cheng 8/83
;-
*/

#include "csig.h"

int inkey(chara, scancode, nowait)

char *chara;
int *scancode, nowait;

{

    int intnum;                             /* INTERRUPT NUMBER */
    int al, ah, flg;                       /* WORK VARIABLES */
    int inreg[4], outreg[4];                /* I/O REGISTERS */
    char *cax;

    if (nowait == 0)
       { al = 0; ah = 0; }
     else
       { al = 0, ah = 1; }
    inreg[0] = (ah << 8) + al;              /* FORMULATE AX */
    intnum = 22;                            /* FOR KEYNOARD */
                                            /*   INTERRUPT CALL */
    flg = sysint(intnum, &inreg, &outreg);
    if ( (nowait != 0) &&                   /* NOTHING AVAIL. ON */
         ( (flg & zf) != 0) )               /*   NOWAIT I/O */
          return(false);
    if (nowait != 0)                        /* SOMETHING THERE ON */
       {                                    /*    NOWAIT I/O, USE   */
          inreg[0] = 0;                     /*    WAIT I/O TO READ  */
          flg = sysint(intnum, &inreg, &outreg );  /*  IT FOR SYSTEM   */
       }                                    /*    POINTER SAKE      */
    cax = &outreg[0];
    *chara = *cax;
    *scancode = outreg[0] >> 8;
    return(true);
}

                                                                                                                           