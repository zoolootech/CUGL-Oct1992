/*	EPSON PRINTER CONSTANTS		


			 Author - Joe Shannon
			    566 Rock Road
			State College, PA 16801
			    (814) 238-0029

			      Version 1.0
			   February 28, 1982

**This program is released to public domain with the request that author credit
  be retained in the source.
*/

#define	PRDATA	0x7C	/* printer data port */
#define	PRSTAT	0x7D	/* printer status port */
#define	PRMASK	1	/* printer ready mask */

#define	NUL	0	/* ends TAB ESC B ESC C */
#define	BEL	0x07	/* sounds bell for 1 sec */
#define	BS	0x08	/* backspace */
#define	HT	0x09	/* horizontal tab */
#define	LF	0x0A	/* line feed */
#define	VT	0x0B	/* vertical tab */
#define	FF	0x0C	/* form feed */
#define	CR	0x0D	/* carriage return */
#define	SO	0x0E	/* shift out - large chr. prnt. */
#define	SI	0x0F	/* shift in - condensed chr. prnt. */
#define	DC1	0x11	/* device cntrl 1 - sel print. rdy to rec data */
#define	DC2	0x12	/* device cntrl 2 - condensed chr off */
#define	DC3	0x13	/* device cntrl 3 - desel prt no rdy to rec data */
#define	DC4	0x14	/* device cntrl 4 - large chr off */
#define	ESC	0x1B	/* escape */
#define	ESC0	0x30	/* sets line space to 8 lines p/inch */
#define	ESC2	0x32	/* sets line space to 6 lines p/inch */
#define	ESC8	0x38	/* desel paper end detctor */
#define	ESC9	0x39	/* selects paper end detctor */
#define	ESCA	0x41	/* sets line spc btw 1/72" & 85/72 */
#define	ESCB	0x42	/* sets VT to 8 positions */
#define	ESCC	0x43	/* sets frm lgth to 127 lines or 22" */
#define	ESCD	0x44	/* sets HT up to 12 positions */
#define	ESCE	0x45	/* turn on emphasized chr */
#define	ESCF	0x46	/* turn off emphasized chr */
#define	ESCK	0x4B	/* turn on norm density bit image mode */
#define	ESCL	0x4C	/* turn off dual density bit image mode */
#define	ESCN	0x4E	/* set skip-over perf */
#define	ESCO	0x4F	/* rels skip-over perf */
#define	ESCQ	0x51	/* set column lgth */
#define	ESCR	0x52	/* select international chr set */



/*------------ PRINTER OUTPUT ----------------*/
pntout(BYTE)
char	BYTE;
{	while((inp(PRSTAT) & PRMASK)==0);
	outp(PRDATA,BYTE);
	return;
}

/*------------ PRINTER INITIALIZE ------------*/
pntint()
{	prsel();
	prsel();	/* dumb printer sometimes needs 2 */
	colsiz(80);	/* default collumn size */
	tabset(9,17,25,33,41,49,57,65,73,0);	/* set tabs @8 ea. */
	deflin();	/* default line spacing */
	smlcan();
	empcan();
	perfcan();
	pntout(CR);
	return;
}

/*------------ ESCAPE SEQUENCE PRINT ---------*/
escpnt(BYTE)
char	BYTE;
{	pntout(ESC);
	pntout(BYTE);
	return;
}

/*----------- SET TABS ON PRINTER ------------*/
tabset(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)
int	T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12;
{	escpnt(ESCD);
	if(T1==NUL)	goto DONE;	pntout(T1);
	if(T2==NUL)	goto DONE;	pntout(T2);
	if(T3==NUL)	goto DONE;	pntout(T3);
	if(T4==NUL)	goto DONE;	pntout(T4);
	if(T5==NUL)	goto DONE;	pntout(T5);
	if(T6==NUL)	goto DONE;	pntout(T6);
	if(T7==NUL)	goto DONE;	pntout(T7);
	if(T8==NUL)	goto DONE;	pntout(T8);
	if(T9==NUL)	goto DONE;	pntout(T9);
	if(T10==NUL)	goto DONE;	pntout(T10);
	if(T11==NUL)	goto DONE;	pntout(T11);
	if(T12==NUL)	goto DONE;	pntout(T12);
DONE:	pntout(NUL);
	return;
}

/*----------- SET COLUMN SIZE ON PRINTER -----*/
colsiz(BYTE)
char	BYTE;
{	if((BYTE<1)||(BYTE>233)) return;
	escpnt(ESCQ);
	pntout(BYTE);
	return;
}
/*----------- SET LINE SPACING 1-85 ----------*/
linsiz(BYTE)
char	BYTE;
{	if((BYTE<1)||(BYTE>85)) return;
	escpnt(ESCA);
	pntout(BYTE);
	return;
}

/*---------- SET LINE SPACING TO DEFAULT -----*/
deflin()
{	escpnt(ESC2);
	return;
}

/*--------- SET LINE SPACING TO 8 LINES/IN. --*/
lin8()
{	escpnt(ESC0);
	return;
}

/*--------- SET VERTICAL TABS ----------------*/
vtabs(T1,T2,T3,T4,T5,T6,T7,T8)
char	T1,T2,T3,T4,T5,T6,T7,T8;
{	escpnt(ESCB);
	if(T1==NUL)	goto DONE;	pntout(T1);
	if(T2==NUL)	goto DONE;	pntout(T2);
	if(T3==NUL)	goto DONE;	pntout(T3);
	if(T4==NUL)	goto DONE;	pntout(T4);
	if(T5==NUL)	goto DONE;	pntout(T5);
	if(T6==NUL)	goto DONE;	pntout(T6);
	if(T7==NUL)	goto DONE;	pntout(T7);
	if(T8==NUL)	goto DONE;	pntout(T8);
DONE:	pntout(NUL);
	return;
}
		
/*--------- SET FORM SIZE BY LINE OR LEGNTH --*/
formlin(BYTE)
char	BYTE;
{	if((BYTE<1)||(BYTE>127)) return;
	escpnt(ESCC);
	pntout(BYTE);
	return;
}
formsiz(BYTE)
{	if((BYTE<1)||(BYTE>22)) return;
	escpnt(ESCC);
	pntout(0);
	pntout(BYTE);
	return;
}


/*------------ SKIP PERFORATION BY LINES -----*/
perfskip(BYTE)
char	BYTE;
{	if((BYTE<1)||(BYTE>127)) return;
	escpnt(ESCN);
	pntout(BYTE);
	return;
}
perfcan()
{	escpnt(ESCO);
	return;
}

/*---------- TURN ON LARGE PRINT -------------*/
lrgpnt()
{	pntout(SO);
	return;
}
lrgcan()		/* cancel large print */
{	pntout(DC4);
	return;
}

/*---------- TURN ON SMALL PRINT -------------*/
smlpnt()
{	pntout(SI);
	return;
}
smlcan()		/* cancel small print */
{	pntout(DC2);
	return;
}

/*----------- TURN ON EMPHASIZED PRINT -------*/
emppnt()
{	escpnt(ESCE);
	return;
}
empcan()		/* cancel empha print */
{	escpnt(ESCF);
	return;
}

/*------------ CHANGE CHARACTER SET ----------*/
chgchr(BYTE)
char	BYTE;
{	if((BYTE<1)||(BYTE>7)) return;
	escpnt(ESCR);
	pntout(BYTE);
	return;
}

/*------------- PRINTER SELECT (CLR BUFFER)-- */
prsel()
{	pntout(DC1);
	return;
}

prdsel()
{	pntout(DC3);	  /* deselect printer */
	return;
}

/*------------- PAPER END DETECTOR OFF/ON ----*/
pedon()
{	escpnt(ESC9);
	return;
}
pedoff()
{	escpnt(ESC8);
	return;
}

/*------------ RING PRINTER BELL --------------*/
prbell()
{	pntout(BEL);
	return;
}

/*------------ GRAPHICS MODES -----------------*/
ngraph(NBYTES)
int	NBYTES;
{	escpnt(ESCK);
	pntout(NBYTES);
	pntout(NBYTES>>8);
	return;
}
dgraph(NBYTES)
int	NBYTES;
{	escpnt(ESCL);
	pntout(NBYTES);
	pntout(NBYTES>>8);
	return;
}

