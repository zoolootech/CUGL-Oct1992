/* [DOCTOR3.C of JUGPDS Vol.18]
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*	Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/

/* DOCTOR3 - Disk Doctor for CP/M Plus */

#include "stdio.h"

#define UP		0x05	/* Screen_Edit mode Cursole_up		*/
#define DOWN		0x18	/*        --II--    Cursole_down	*/
#define LEFT		0x13	/*        --II--    Cursole_letf	*/
#define RIGHT		0x06	/*        --II--    Cursole_right	*/
#define ESC_CH		'@'	/* Special command sequence code	*/
#define ENDINPUT	'Z'	/* Screen_Edit mode exit		*/
#define ABORT		'X'	/* Screen_Edit mode exit with out WRite */

/*
 ************************************************************************
 *	HOME		Cursole Home Positioning			*
 *	CLRS		Clear Screen and Cursole Home Positioning	*
 *	POS(x,y)	Set Cursole position (x,y)			*
 ************************************************************************
*/
#define ESC		0x1b
#define	HOME		bios(4,ESC); bios(4,'H')
#define	CLRS		bios(4,ESC); bios(4,'E')
#define	POS(x,y)	bios(4,ESC); bios(4,'Y'); bios(4,y+32); bios(4,x+32)


/*
 *	Global DATA defintion
 */

unsigned	tpd, bpd, No_Of_Track, No_Of_Sector, No_Of_Block, Block_Length;

/*
 *	CP/M DPB (disk parameter block) definition
 */

struct	dpb {
	int	spt;		/* logical sector per track */
	char	bsh,		/* block shift factor       */
		blm,		/* block mask		    */
		exm;		/* extent mask		    */
	int	dsm,		/* disk storage (record)    */
		drm;		/* total directory entry - 1*/
	char	al0,al1;	/* allocation bit mask	    */
	int	cks;		/* size of directory check  */
	int	off;		/* offset (system track)    */
	char	psh,phm;
};

/*
 *	CP/M DPH (disk parameter header) definition
 */


struct	dph {
	char	*xltp;		/* logical to physical trans  */
	char	dmy1[9];	/* system use		    */
	char	mf;		/* Media flag		    */
	struct dpb	*dpbp;	/* pointer for DPB	    */
	char	*csvp,		/* pointer for check_size   */
		*alvp;		/* pointer for allocation   */
	char	*dirbcb, *dtabcb, *hash, hbank;
};

struct	_biospb {
	char		BIOS_NO;
	char		A_REG;
	unsigned	BC_REG;
	unsigned	DE_REG;
	unsigned	HL_REG;
} biospb;

#define	max(x,y)	(((x)>(y))?(x):(y))
#define	min(x,y)	(((x)<(y))?(x):(y))

struct	dph	*DPH;
struct	dpb	*DPB;


int		bn, bufcnt, es;
char		vflag, logflag, blkflag;
char		queflag, *quep;

char		*hexdigit;

main()
{
	char	*p, c, comnd, buf[2048], w[128], *wp;
	int	d, i, j, v, len, t, s, n, sc;

	hexdigit = "0123456789ABCDEFabcdef";
	queflag = 0;
	vflag = 0;
	logflag = 1;
	blkflag = 0;
	bn = 0;
	s = 1;
	p = buf;
	CLRS;
	POS(0,0); printf("Disk:"); d = toupper(bios(3,0)) - 'A';
	t = seldisk(d);
	while (GetSector(d,t,s,p) == 0) {
loopd:		POS(0,4);
		Display(p+128*es);
		c = getchr();
		switch ((comnd = tolower(c))) {
		case 'a' :
			Modify(t, s, p+128*es, comnd, p);
			break;
		case 'b' :
			POS(57,0);	scanf("%d", &bn);
			if (bn < 0 || bn > DPB->dsm)
				break;
			blkflag = (blkflag) ? 0 : 1;
			logflag = 1;
			sc = bn * ( DPB->blm + 1);
			t  = sc / DPB->spt;
			s  = sc % DPB->spt + 1;
			break;
		case 'c' :
			setmem(p+es*128, 128, 0);
			PutSector(t, s, p);
			break;
		case 'd' :
			POS(5, 0); d = toupper(bios(3, 0)) - 'A';
			seldisk(d);
			break;
		case 'e' :
			exit();
		case 'h' :
			Modify(t, s, p+128*es, comnd, p);
			break;
		case 'i' :
			setmem(p+es*128, 128, 0x0e5);
			PutSector(t, s, p);
			break;
		case 'l' :
			logflag = logflag ? 0 : 1;
			seldisk(d);
			break;
		case 'q' :
			queflag = queflag ? 0 : 1;
			quep = "++++++++++++++++";
			goto loopd;
		case 't' :
			POS(16, 0);	scanf("%d", &t);
			break;
		case 's' :
			POS(37, 0);	scanf("%d", &s);
			break;
		case '+' :
		case ';' :
			if ( ++s > DPB->spt ) {s = 1; t++;}
			break;
		case '-' :
		case '=' :
			if ( --s < 1 ) {s = DPB->spt; t--;}
			break;
		case '>' :
		case '.' :
			t++;
			break;
		case '<' :
		case ',' :
			t--;
			break;
		default :
			goto loopd;
		}
		s = max(1, s);
		s = min(DPB->spt, s);
		t = max(0, t);
		t = min(tpd + DPB->off*logflag, t);
		bn = (t * DPB->spt + s -1)/(DPB->blm + 1);
	}
	POS(0, 23);
}


GetSector(d, t, s, p)
char    *p;
{
	int	bn;

	es = (s-1) % (DPB->phm + 1);
	bn = (t * DPB->spt + s -1)/(DPB->blm+1);
	POS(16, 0);	printf("%4d      ", t);
	POS(37, 0);	printf("%3d      ", s);
	POS(56, 0);	printf("%4d      ", bn);
	BIOS_CALL(10, t+logflag*DPB->off, 0, 0);	/* set Track No  */
	BIOS_CALL(11, (s-1)/(DPB->phm+1)+1, 0, 0);	/* set Sector No */
	BIOS_CALL(12, p, 0, 0);				/* set DMA       */
	BIOS_CALL(28, 0, 0, 1);
	BIOS_CALL(13, 0, 0, 0);
	return 0;
}


PutSector(t, s, p)
int     t, s;
char    *p;
{
	BIOS_CALL(10,t+logflag*DPB->off, 0, 0);		/* set track no */
	BIOS_CALL(11,(s-1)/(DPB->phm+1)+1, 0, 0);	/* set Sector No */
	BIOS_CALL(12, p, 0, 0);				/* set dma      */
	BIOS_CALL(28, 0, 0, 1);
	BIOS_CALL(14, 1, 0, 0);
}


getchr()
{
	if (queflag) {
		if (*(quep+1) == '\0')
			queflag = 0;
		return *quep++;
		}
	return bios(3, 0);
}


Modify(t, s, p, mode, bp)
char	*p, *bp, mode;
{
	int	j, x, y, c, c1, end_flag;

	j = 0;
	x = 0;	y = 0;

	POS(0, 12);
	printf("                                                  ");
	POS(0, 12);
	printf("%s mode Modify-Data :", mode == 'a' ? "Ascii" : "Hexdec");
	end_flag = 0;
	while (!end_flag) {
		POS(x+x, (mode == 'a' ? y+y+5 : y+y+4));
		switch (c = bios(3, 0)) {
			case UP :
				if (y)
					y--;
				break;
			case DOWN :
				if (y<4)
					y++;
				break;
			case LEFT :
				if (x)
					x--;
				else if (y) {
					x = 31;
					y--;
					}
				break;
			case RIGHT :
				if (x < 31)
					x++;
				else if (y < 3) {
					x = 0;
					y++;
					}
				break;
			default :
				if (c == ESC_CH) {
					if ((c = toupper(bios(3, 0))) == ENDINPUT) {
						end_flag = 1;
						break;
						}
					else if (c == ABORT) {
						end_flag = 1;
						break;
						}
					}
				if (mode == 'h') {
					c1 = bios(3, 0);
					c1 = toupper(c1);
					c  = toupper(c);
					if ((c1 = index_(hexdigit, c1)) >= 0
					 && (c = index_(hexdigit, c)) >= 0) {
						*(p + j) = c*16+c1;
					}
				}
				else
					*(p + j) = c;
				POS(0, 4);
				Display(p);
				if (x < 31)
					x++;
				else if (y < 3) {
					x = 0;
					y++;
					}
			}
		j = (y << 5) + x;
		}
	if (c != ABORT)
		PutSector(t, s, bp);
	POS(0, 12);
	printf("                                                  ");
}


seldisk(d)
int d;
{
	unsigned	x,y,z;

/*	bios(27, 0);
*/
	DPH = BIOS_CALL(9, d, 0, 0);		/* select disk  */
	DPB = DPH->dpbp;
	x = DPB->blm+1;    
	y = DPB->dsm;
	z = DPB->spt;
	No_Of_Sector  = z;
	No_Of_Track   = (x * y + z - 1)/ z + DPB->off;
	No_Of_Block   = y + 1;
	Block_Length  = x * 128;


	POS(0,21);
     printf("Track_No :=%6d       Sector_No :=%6d       Block_No   :=%6d\n",
			     No_Of_Track, No_Of_Sector, No_Of_Block);
	printf("Offset   :=%6u       Directry  :=%6u       Block_size :=%6d",
			DPB->off, DPB->drm+1, Block_Length);
	POS(0,0);
	printf("Disk:     Track:              Sector:              Block:");
	POS(5 ,0);	putchar(d+'A');
	POS(0 ,2);	printf(logflag ? "(  Log" : "( Phys");
	POS(6 ,2);	printf("ical-CP/M-Sector )");
	tpd = No_Of_Track-1;
	bpd = No_Of_Block-1;
	return DPB->off*logflag;
}


setmem(p, l, d)
char *p, d;
int  l;
{
	while (l--) *p++ = d;
}


index_(s, c)
char *s, c;
{
	char *p;

	for (p = s; *s != c; s++)
		if (!*s)
			return -1;
	return s-p;
}


BIOS_CALL(fn, bcreg, dereg, areg)
{
	biospb.BIOS_NO = fn;
 	biospb.BC_REG  = bcreg;
 	biospb.DE_REG  = dereg;
 	biospb.A_REG  = areg;
	return bdos(50, &biospb);
}
