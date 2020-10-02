###alltyp.bat
lc -ms -i\lc\ -i \lc\s\ groups >>cc.out
link c+groups+cputim+returnp+ntim_int+fround+error,groups,,lcm+lc >>cc.out
lc -ms -i\lc\ -i \lc\s\ DADD >>cc.out
link c+DADD+cputim+returnp+begint+ntim_int+fround+error,DADD,,lcm+lc >>cc.out
DADD.exe >DADD.out
groups <DADD.out >DADD.grp
del DADD.obj
del DADD.map
del DADD.exe
lc -ms -i\lc\ -i \lc\s\ DAND >>cc.out
link c+DAND+cputim+returnp+begint+ntim_int+fround+error,DAND,,lcm+lc >>cc.out
DAND.exe >DAND.out
groups <DAND.out >DAND.grp
del DAND.obj
del DAND.map
del DAND.exe
lc -ms -i\lc\ -i \lc\s\ DBAN >>cc.out
link c+DBAN+cputim+returnp+begint+ntim_int+fround+error,DBAN,,lcm+lc >>cc.out
DBAN.exe >DBAN.out
groups <DBAN.out >DBAN.grp
del DBAN.obj
del DBAN.map
del DBAN.exe
lc -ms -i\lc\ -i \lc\s\ DBOR >>cc.out
link c+DBOR+cputim+returnp+begint+ntim_int+fround+error,DBOR,,lcm+lc >>cc.out
DBOR.exe >DBOR.out
groups <DBOR.out >DBOR.grp
del DBOR.obj
del DBOR.map
del DBOR.exe
lc -ms -i\lc\ -i \lc\s\ DDIV >>cc.out
link c+DDIV+cputim+returnp+begint+ntim_int+fround+error,DDIV,,lcm+lc >>cc.out
DDIV.exe >DDIV.out
groups <DDIV.out >DDIV.grp
del DDIV.obj
del DDIV.map
del DDIV.exe
lc -ms -i\lc\ -i \lc\s\ DEOR >>cc.out
link c+DEOR+cputim+returnp+begint+ntim_int+fround+error,DEOR,,lcm+lc >>cc.out
DEOR.exe >DEOR.out
groups <DEOR.out >DEOR.grp
del DEOR.obj
del DEOR.map
del DEOR.exe
lc -ms -i\lc\ -i \lc\s\ DEQ >>cc.out
link c+DEQ+cputim+returnp+begint+ntim_int+fround+error,DEQ,,lcm+lc >>cc.out
DEQ.exe >DEQ.out
groups <DEQ.out >DEQ.grp
del DEQ.obj
del DEQ.map
del DEQ.exe
lc -ms -i\lc\ -i \lc\s\ DLSH >>cc.out
link c+DLSH+cputim+returnp+begint+ntim_int+fround+error,DLSH,,lcm+lc >>cc.out
DLSH.exe >DLSH.out
groups <DLSH.out >DLSH.grp
del DLSH.obj
del DLSH.map
del DLSH.exe
lc -ms -i\lc\ -i \lc\s\ DLT >>cc.out
link c+DLT+cputim+returnp+begint+ntim_int+fround+error,DLT,,lcm+lc >>cc.out
DLT.exe >DLT.out
groups <DLT.out >DLT.grp
del DLT.obj
del DLT.map
del DLT.exe
lc -ms -i\lc\ -i \lc\s\ DMUL >>cc.out
link c+DMUL+cputim+returnp+begint+ntim_int+fround+error,DMUL,,lcm+lc >>cc.out
DMUL.exe >DMUL.out
groups <DMUL.out >DMUL.grp
del DMUL.obj
del DMUL.map
del DMUL.exe
lc -ms -i\lc\ -i \lc\s\ DREM >>cc.out
link c+DREM+cputim+returnp+begint+ntim_int+fround+error,DREM,,lcm+lc >>cc.out
DREM.exe >DREM.out
groups <DREM.out >DREM.grp
del DREM.obj
del DREM.map
del DREM.exe
lc -ms -i\lc\ -i \lc\s\ DRSH >>cc.out
link c+DRSH+cputim+returnp+begint+ntim_int+fround+error,DRSH,,lcm+lc >>cc.out
DRSH.exe >DRSH.out
groups <DRSH.out >DRSH.grp
del DRSH.obj
del DRSH.map
del DRSH.exe
lc -ms -i\lc\ -i \lc\s\ DSUB >>cc.out
link c+DSUB+cputim+returnp+begint+ntim_int+fround+error,DSUB,,lcm+lc >>cc.out
DSUB.exe >DSUB.out
groups <DSUB.out >DSUB.grp
del DSUB.obj
del DSUB.map
del DSUB.exe
lc -ms -i\lc\ -i \lc\s\ UINC >>cc.out
link c+UINC+cputim+returnp+begint+ntim_int+fround+error,UINC,,lcm+lc >>cc.out
UINC.exe >UINC.out
groups <UINC.out >UINC.grp
del UINC.obj
del UINC.map
del UINC.exe
lc -ms -i\lc\ -i \lc\s\ UNEG >>cc.out
link c+UNEG+cputim+returnp+begint+ntim_int+fround+error,UNEG,,lcm+lc >>cc.out
UNEG.exe >UNEG.out
groups <UNEG.out >UNEG.grp
del UNEG.obj
del UNEG.map
del UNEG.exe
lc -ms -i\lc\ -i \lc\s\ UPIN >>cc.out
link c+UPIN+cputim+returnp+begint+ntim_int+fround+error,UPIN,,lcm+lc >>cc.out
UPIN.exe >UPIN.out
groups <UPIN.out >UPIN.grp
del UPIN.obj
del UPIN.map
del UPIN.exe
###install.bat
lc -ms -i\lc\ -i\lc\s calib  >>icc.out
lc -ms -i\lc\ -i\lc\s begint  >>icc.out
lc -ms -i\lc\ -i\lc\s cputim  >>icc.out
lc -ms -i\lc\ -i\lc\s returnp  >>icc.out
lc -ms -i\lc\ -i\lc\s error  >>icc.out
lc -ms -i\lc\ -i\lc\s fround  >>icc.out
lc -ms -i\lc\ -i\lc\s intreg  >>icc.out
lc -ms -i\lc\ -i\lc\s sstat  >>icc.out
lc -ms -i\lc\ -i\lc\s sauto  >>icc.out
lc -ms -i\lc\ -i\lc\s lauto  >>icc.out
lc -ms -i\lc\ -i\lc\s dauto  >>icc.out
lc -ms -i\lc\ -i\lc\s cauto  >>icc.out
lc -ms -i\lc\ -i\lc\s fauto  >>icc.out


link c+calib+cputim+returnp+begint+error+fround+ntim_int,calib,,lcm+lc >>icc.out
del calib.map
del calib.obj

calib.exe > parms.dat

type parms.dat


link c+cauto+cputim+returnp+begint+error+fround+ntim_int,cauto,,lcm+lc >>icc.out
cauto.exe >cauto.out
del cauto.obj
del cauto.map
del cauto.exe


link c+fauto+cputim+returnp+begint+error+fround+ntim_int,fauto,,lcm+lc >>icc.out
fauto.exe >fauto.out
del fauto.obj
del dauto.map
del dauto.exe





link c+intreg+cputim+returnp+begint+error+fround+ntim_int,intreg,,lcm+lc >>icc.out
intreg.exe >intreg.out
del intreg.obj
del intreg.map
del intreg.exe


link c+sstat+cputim+returnp+begint+error+fround+ntim_int,sstat,,lcm+lc >>icc.out
sstat.exe >sstat.out
del sstat.obj
del sstat.map
del sstat.exe


link c+sauto+cputim+returnp+begint+error+fround+ntim_int,sauto,,lcm+lc >>icc.out
sauto.exe >sauto.out
del sauto.obj
del sauto.map
del sauto.exe


link c+lauto+cputim+returnp+begint+error+fround+ntim_int,lauto,,lcm+lc >>icc.out
lauto.exe >lauto.out
del lauto.obj
del lauto.map
del lauto.exe


link c+dauto+cputim+returnp+begint+error+fround+ntim_int,dauto,,lcm+lc >>icc.out
dauto.exe >dauto.out
del dauto.obj
del dauto.map
del dauto.exe


lc -ms -i\lc\ -i\lc\s fmttbl  >>icc.out
link c+fmttbl+cputim+returnp+begint+error+fround+ntim_int,fmttbl,,lcm+lc >>icc.out
fmttbl sauto.out cauto.out lauto.out dauto.out fauto.out
###mkavg.bat
rem after running install.bat you can run mkavg.bat to compute averages
lc -ms -i\lc\ -i\lc\s avg   >>avgcc.out
link c+avg+cputim+returnp+begint+error+fround+ntim_int,avg,,lcm+lc  >>avgcc.out

avg intreg.out 10 >intreg.avg
avg sstat.out 10 >sstat.avg
avg sauto.out 10 >sauto.avg
avg lauto.out 50 >lauto.avg
avg dauto.out 400 >dauto.avg
avg cauto.out 10 >cauto.avg
avg fauto.out 400 >fauto.avg
###runmost.bat
lc -ms -i\lc\ -i \lc\s\ FNCALL >>rcc.out
link c+FNCALL+cputim+returnp+begint+ntim_int+fround+error,FNCALL,,lcm+lc >>rcc.out
FNCALL.exe >FNCALL.out 
del FNCALL.obj
del FNCALL.map
del FNCALL.exe
lc -ms -i\lc\ -i \lc\s\ LOOPS1 >>rcc.out
link c+LOOPS1+cputim+returnp+begint+ntim_int+fround+error,LOOPS1,,lcm+lc >>rcc.out
LOOPS1.exe >LOOPS1.out 
del LOOPS1.obj
del LOOPS1.map
del LOOPS1.exe
lc -ms -i\lc\ -i \lc\s\ LOOPS2 >>rcc.out
link c+LOOPS2+cputim+returnp+begint+ntim_int+fround+error,LOOPS2,,lcm+lc >>rcc.out
LOOPS2.exe >LOOPS2.out 
del LOOPS2.obj
del LOOPS2.map
del LOOPS2.exe
lc -ms -i\lc\ -i \lc\s\ 102 >>rcc.out
link c+102+cputim+returnp+begint+ntim_int+fround+error,102,,lcm+lc >>rcc.out
102.exe >102.out 
del 102.obj
del 102.map
del 102.exe
lc -ms -i\lc\ -i \lc\s\ 104 >>rcc.out
link c+104+cputim+returnp+begint+ntim_int+fround+error,104,,lcm+lc >>rcc.out
104.exe >104.out 
del 104.obj
del 104.map
del 104.exe
lc -ms -i\lc\ -i \lc\s\ 105 >>rcc.out
link c+105+cputim+returnp+begint+ntim_int+fround+error,105,,lcm+lc >>rcc.out
105.exe >105.out 
del 105.obj
del 105.map
del 105.exe
lc -ms -i\lc\ -i \lc\s\ 106 >>rcc.out
link c+106+cputim+returnp+begint+ntim_int+fround+error,106,,lcm+lc >>rcc.out
106.exe >106.out 
del 106.obj
del 106.map
del 106.exe
lc -ms -i\lc\ -i \lc\s\ 401 >>rcc.out
link c+401+cputim+returnp+begint+ntim_int+fround+error,401,,lcm+lc >>rcc.out
401.exe >401.out 
del 401.obj
del 401.map
del 401.exe
lc -ms -i\lc\ -i \lc\s\ 402 >>rcc.out
link c+402+cputim+returnp+begint+ntim_int+fround+error,402,,lcm+lc >>rcc.out
402.exe >402.out 
del 402.obj
del 402.map
del 402.exe
lc -ms -i\lc\ -i \lc\s\ 403 >>rcc.out
link c+403+cputim+returnp+begint+ntim_int+fround+error,403,,lcm+lc >>rcc.out
403.exe >403.out 
del 403.obj
del 403.map
del 403.exe
lc -ms -i\lc\ -i \lc\s\ 405 >>rcc.out
link c+405+cputim+returnp+begint+ntim_int+fround+error,405,,lcm+lc >>rcc.out
405.exe >405.out 
del 405.obj
del 405.map
del 405.exe
lc -ms -i\lc\ -i \lc\s\ 411 >>rcc.out
link c+411+cputim+returnp+begint+ntim_int+fround+error,411,,lcm+lc >>rcc.out
411.exe >411.out 
del 411.obj
del 411.map
del 411.exe
lc -ms -i\lc\ -i \lc\s\ 413 >>rcc.out
link c+413+cputim+returnp+begint+ntim_int+fround+error,413,,lcm+lc >>rcc.out
413.exe >413.out 
del 413.obj
del 413.map
del 413.exe
lc -ms -i\lc\ -i \lc\s\ 501 >>rcc.out
link c+501+cputim+returnp+begint+ntim_int+fround+error,501,,lcm+lc >>rcc.out
501.exe >501.out 
del 501.obj
del 501.map
del 501.exe
lc -ms -i\lc\ -i \lc\s\ 503 >>rcc.out
link c+503+cputim+returnp+begint+ntim_int+fround+error,503,,lcm+lc >>rcc.out
503.exe >503.out 
del 503.obj
del 503.map
del 503.exe
lc -ms -i\lc\ -i \lc\s\ 504 >>rcc.out
link c+504+cputim+returnp+begint+ntim_int+fround+error,504,,lcm+lc >>rcc.out
504.exe >504.out 
del 504.obj
del 504.map
del 504.exe
lc -ms -i\lc\ -i \lc\s\ 505 >>rcc.out
link c+505+cputim+returnp+begint+ntim_int+fround+error,505,,lcm+lc >>rcc.out
505.exe >505.out 
del 505.obj
del 505.map
del 505.exe
lc -ms -i\lc\ -i \lc\s\ 506 >>rcc.out
link c+506+cputim+returnp+begint+ntim_int+fround+error,506,,lcm+lc >>rcc.out
506.exe >506.out 
del 506.obj
del 506.map
del 506.exe
lc -ms -i\lc\ -i \lc\s\ 507 >>rcc.out
link c+507+cputim+returnp+begint+ntim_int+fround+error,507,,lcm+lc >>rcc.out
507.exe >507.out 
del 507.obj
del 507.map
del 507.exe
lc -ms -i\lc\ -i \lc\s\ 508 >>rcc.out
link c+508+cputim+returnp+begint+ntim_int+fround+error,508,,lcm+lc >>rcc.out
508.exe >508.out 
del 508.obj
del 508.map
del 508.exe
lc -ms -i\lc\ -i \lc\s\ 601 >>rcc.out
link c+601+cputim+returnp+begint+ntim_int+fround+error,601,,lcm+lc >>rcc.out
601.exe >601.out 
del 601.obj
del 601.map
del 601.exe

###runmost2.bat
lc -ms -i\lc\ -i \lc\s\ switch >>rcc2.out
link c+switch+cputim+returnp+begint+ntim_int+fround+error,switch,,lcm+lc >>rcc2.out
switch.exe >switch.out 
del switch.obj
del switch.map
del switch.exe

lc -ms -i\lc\ -i \lc\s\ ctypelib >>rcc2.out
link c+ctypelib+cputim+returnp+begint+ntim_int+fround+error,ctypelib,,lcm+lc >>rcc2.out
ctypelib.exe >ctypelib.out 
del ctypelib.obj
del ctypelib.map
del ctypelib.exe

lc -ms -i\lc\ -i \lc\s\ fdliblib >>rcc2.out
link c+fdliblib+cputim+returnp+begint+ntim_int+fround+error,fdliblib,,lcm+lc >>rcc2.out
fdliblib.exe >fdliblib.out 
del fdliblib.obj
del fdliblib.map
del fdliblib.exe

lc -ms -i\lc\ -i \lc\s\ mathlib >>rcc2.out
link c+mathlib+cputim+returnp+begint+ntim_int+fround+error,mathlib,,lcm+lc >>rcc2.out
mathlib.exe >mathlib.out 
del mathlib.obj
del mathlib.map
del mathlib.exe

lc -ms -i\lc\ -i \lc\s\ strlib >>rcc2.out
link c+strlib+cputim+returnp+begint+ntim_int+fround+error,strlib,,lcm+lc >>rcc2.out
strlib.exe >strlib.out 
del strlib.obj
del strlib.map
del strlib.exe

lc -ms -i\lc\ -i \lc\s\ stdlib >>rcc2.out
link c+stdlib+cputim+returnp+begint+ntim_int+fround+error,stdlib,,lcm+lc >>rcc2.out
stdlib.exe >stdlib.out 
del stdlib.obj
del stdlib.map
del stdlib.exe

lc -ms -i\lc\ -i \lc\s\ stdiolib >>rcc2.out
link c+stdiolib+cputim+returnp+begint+ntim_int+fround+error,stdiolib,,lcm+lc >>rcc2.out
stdiolib.exe >stdiolib.out 
del stdiolib.obj
del stdiolib.map
del stdiolib.exe

lc -ms -i\lc\ -i \lc\s\ harness1 >>rcc2.out
link c+harness1+cputim+returnp+begint+ntim_int+fround+error,harness1,,lcm+lc >>rcc2.out
rem harness1
time
harness1.exe >harness1.out 
time
del harness1.obj
del harness1.map
del harness1.exe

lc -ms -i\lc\ -i \lc\s\ harness2 >>rcc2.out
link c+harness2+cputim+returnp+begint+ntim_int+fround+error,harness2,,lcm+lc >>rcc2.out
rem harness2
time
harness2.exe >harness2.out 
time
del harness2.obj
del harness2.map
del harness2.exe

lc -ms -i\lc\ -i \lc\s\ harness3 >>rcc2.out
link c+harness3+cputim+returnp+begint+ntim_int+fround+error,harness3,,lcm+lc >>rcc2.out
rem harness3
time
harness3.exe >harness3.out 
time
del harness3.obj
del harness3.map
del harness3.exe

lc -ms -i\lc\ -i \lc\s\ harness4 >>rcc2.out
link c+harness4+cputim+returnp+begint+ntim_int+fround+error,harness4,,lcm+lc >>rcc2.out
rem harness4
time
harness4.exe >harness4.out 
time
del harness4.obj
del harness4.map
del harness4.exe


lc -ms -i\lc\ -i \lc\s\ optimtst >>rcc2.out
link c+optimtst+cputim+returnp+begint+ntim_int+fround+error,optimtst,,lcm+lc >>rcc2.out
optimtst.exe >optimtst.out 
del optimtst.obj
del optimtst.map
del optimtst.exe

###config.h
/* config.h - configuration data for each environment
 *		This file should either be customized to the intended
 *		compiler/machine environment, or parameterized via -D flags
 *		on the compile step:
 *		-D UTINY	if compiler supports unsigned char
 *		-D USHORT	if compiler supports unsigned short
 *		-D ULONG	if compiler supports unsigned LONG
 *		-D VOID		if compiler supports void
 */
/* no UTINY - Lattice PC 2.10 compiler */
/* no USHORT - Lattice PC 2.10 compiler */
/* no ULONG - Lattice PC 2.10 compiler */
/* no VOID - Lattice PC 2.10 compiler */
#ifdef USHORT
typedef unsigned short ushort;
#else
typedef unsigned ushort;	/* assumes short == int */
#endif
#ifdef UTINY
typedef unsigned char utiny;
#endif
#ifdef ULONG
typedef unsigned long ulong;
#endif
#ifndef VOID
#define void int
#endif
#define MINSAM 1000		/* minimum timing sample (usec) */
#define NTRY 100		/* maximum no of code excerpts per file */
#define FR_ANCHOR arg1	/* what is the "frame anchor": arg1 or var1 */
#define RETOFFSET (-1)	/* offset of return pointer
						   relative to frame anchor */
#define JUNKFILE "c:junkfile"
###cputim.c
/*
 *   cputim.c   		Christopher Skelly	
 *									
 *		 Returns a long time containing the number of clock	
 *		 ticks since the last call of cputim. Returns zero on	
 *		 the first call. Clock assumes a 1.193182e+6 standard	
 *		 PC clockrate, and sets 8253 timer channel zero to	
 *		 operate in mode two on the first call. 		
 *									
 */
#include <stdio.h>

#define COUNTDOWN   65535
#define BIOSOVHD    136		/* ticks per bios-tick overhead */
#define OVHD		453		/* overhead for fn execution */

long clockfuzz = 0;			/* cumulative time correction */

/* cputim - return time (in ticks) since last call */
long cputim()
	{
	extern ntim_int();  	/* 8086 routine to load the time struct   */

	unsigned latch_dif; 	/* timer latch difference since last call */
	unsigned cnt_dif;		/* BIOS TIMER_LOW difference (corrected)  */
	unsigned ovhd = OVHD;	/* overhead from bios-tick + fn-exec */
	unsigned junk = ~0;		/* used so all calls take same time */
	unsigned junk2 = ~0;	/* used so all calls take same time */
	static int first = 1;	/* flag for first call		*/

	static struct t
		{
		unsigned latch;
		unsigned tlow;
		unsigned thigh;
		} time = {0}, oldtime = {0};
	/* if first entry, initialize the clock chip */
	if (first)	/* first time through */
		{
		/* set the 8253 timer chip:
		 * channel 0, mode 2, write bytes */
		outp(67, 52);	/* 00110100b */
		outp(64, 0);
		outp(64, 0);

		first = 0;

		for (junk = 64000; junk < 65000; ++junk)
			;		/* soak up some time */
		cputim();	/* make first result more reliable */
		}

	ntim_int(&time);	/* 8086 routine */

	cnt_dif = time.tlow - oldtime.tlow;

	if (time.latch < oldtime.latch)
		--cnt_dif, ovhd += BIOSOVHD;	/* "borrow" */
	else
		--junk, junk2 += BIOSOVHD;

	latch_dif = time.latch - oldtime.latch;

	oldtime.latch = time.latch;
	oldtime.thigh = time.thigh;
	oldtime.tlow  = time.tlow;

	clockfuzz += cnt_dif * BIOSOVHD + ovhd;

	return ((long)cnt_dif * (COUNTDOWN-BIOSOVHD) + latch_dif - ovhd);
	}
###cputim.h
/* cputim.h  */
#ifndef TIME_H
#define TIME_H
#define CLOCK_TICKS_PER_SECOND 1193182
typedef long cputim_t;
extern cputim_t cputim();
#endif
###ntim_int.asm
;
;CHEADER.INC -- HEADER FILE FOR MICROSOFT C AND LATTICE ASM ROUTINES
;
PGROUP	GROUP	PROG	       ; Standard heading
PROG	SEGMENT BYTE PUBLIC 'PROG'
	ASSUME CS:PGROUP
;
REG	STRUC
AXR	DW	?
TLO	DW	?
THI	DW	?
REG	ENDS
;
BIOS_DATA_SEG	EQU	40H
BIOS_DATA	     SEGMENT AT 40H
		ORG	6CH
TIMER_LOW	DW	?
TIMER_HIGH	DW	?
BIOS_DATA	ENDS
;
TIMER_0 	EQU	40H
TIMER_CTL	EQU	43H
TIMER_0_LATCH	EQU	00H
;
	PUBLIC	NTIM_INT
NTIM_INT	PROC	NEAR
	PUSH	BP
	MOV	BP,  SP
	MOV	SI, [BP + 4]

	PUSH	DS		 ;save data segment
	MOV	AX,  BIOS_DATA_SEG
	MOV	DS, AX
	ASSUME	DS:BIOS_DATA

	MOV	AL, TIMER_0_LATCH
   ;CLI
	OUT	TIMER_CTL, AL
	MOV	BX, TIMER_LOW
	MOV	CX, TIMER_HIGH
	IN	AL, TIMER_0
	MOV	AH, AL
	NOP
	IN	AL, TIMER_0
   ;STI
	XCHG	AH, AL
	NEG	AX

	POP	DS
	MOV	[SI].AXR,  AX
	MOV	[SI].TLO, BX
	MOV	[SI].THI, CX

	POP	BP
	RET

NTIM_INT ENDP
PROG	ENDS
	END

###readme.
to run the timing suite on lattice PC

first of all, be sure that your lattice compile procedure
specifies the  -c  ("standard non-nesting comments") flag.

the source disk requires these steps to run:

you will probably want all this in a hard disk directory;
the procedure involves dozens of compiles.
the stdiolib test needs lots of free disk space, 200 K would suffice.

assemble ntim_int.asm to produce ntim_int.obj

run  install.bat

	this produces the tables for basic operations, and creates the
	object files needed for the other steps.

run  mkavg.bat

	this does some simple "cluster analysis" to produce an
	"average time" estimate for each data category.  any time
	that is within a factor of 3 of the average is included in
	the average; the out-of-range times are listed individually.

run  runmost.bat

	this times various control structures

run  runmost2.bat

	more control structures, library tests, simple timing harnesses

run  alltyp.bat

	this times each C operator on all the basic types, and
	produces a "grouping" of significantly different categories.
	(in other words, what combintions of type and storage class
	affect the timing of each operator.)

call with any questions.

tom plum  609-927-3770
###EOF
