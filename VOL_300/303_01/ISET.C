/*
 *	SCCS:	%W%	%G%	%U%
 *	Decode instructions.
 *
 *EMACS_MODES:c
 */

#include <stdio.h>
#include <a.out.h>
#ifdef	COFF
#include <ldfcn.h>
#endif	/*  COFF  */
#include "unc.h"

ef_fids	mainfile;
long	endt;

void	gette(), putte();
void	mkdref();
long	gettw();
symbol	textlab();

int	l1(), l2(), el1(), lea(), lmove(), lcbch(), jj();
int	limed(), lsbit(), lmvml(), lone(), loone(), lonew(), lonel();

int	pmove(), pcbch(), pdbcc(), pscc(), pcs(), pmovc(), pstop(), pexg();
int	pimed(), pmovp(), psbit(), pdbit(), pcs2(), pone(), ppea();
int	plea(), pdreg(), pmvml(), ptrap(), plink(), pareg(), podreg();
int	pqu(), pmqu(), ptreg(), pcmpm(), pomode(), pmshf(), pshf();

struct	opstr	{
	unsigned  short	 mask;
	unsigned  short  match;
	int	(*opsize)();
	int	(*opprin)();
	char	*prarg;
} optab[] = {
#ifdef	COFF
	0xf000, 0x2000, lmove, pmove, ".l",
	0xf000, 0x3000, lmove, pmove, ".w",
	0xf000, 0x1000, lmove, pmove, ".b",
#else	/*  !COFF  */
	0xf000, 0x2000, lmove, pmove, "l",
	0xf000, 0x3000, lmove, pmove, "w",
	0xf000, 0x1000, lmove, pmove, "b",
#endif	/*  !COFF  */
	0xf000, 0x6000, lcbch, pcbch, 0,
	0xffbf, 0x003c, l2,    pcs,   "or",
	0xff00, 0x0000, limed, pimed, "or",
	0xffbf, 0x023c, l2,    pcs,   "and",
	0xff00, 0x0200, limed, pimed, "and",
	0xff00, 0x0400, limed, pimed, "sub",
	0xff00, 0x0600, limed, pimed, "add",
	0xffbf, 0x0a3c, l2,    pcs,   "eor",
	0xff00, 0x0a00, limed, pimed, "eor",
	0xff00, 0x0c00, limed, pimed, "cmp",
	0xf138, 0x0108, l2,    pmovp, 0,
	0xff00, 0x0800, lsbit, psbit, 0,
	0xf100, 0x0100, lonew, pdbit, 0,
	0xffc0, 0x40c0, lonew, pcs2,  "sr",
	0xff00, 0x4000, lone,  pone,  "negx",
	0xff00, 0x4200, lone,  pone,  "clr",
	0xffc0, 0x44c0, lonew, pcs2,  "cc",
	0xff00, 0x4400, lone,  pone,  "neg",
	0xffc0, 0x46c0, lonew, pcs2,  "sr",
	0xff00, 0x4600, lone,  pone,  "not",
	0xffc0, 0x4800, lonew, ppea,  "nbcd",
#ifdef	COFF
	0xfff8, 0x4840, l1,    pdreg, "swap.w",
#else	/*  !COFF  */
	0xfff8, 0x4840, l1,    pdreg, "swap",
#endif	/*  !COFF  */
	0xffc0, 0x4840, lonel, ppea,  "pea",
#ifdef	COFF
	0xfff8, 0x4880, l1,    pdreg, "ext.w",
	0xfff8, 0x48c0, l1,    pdreg, "ext.l",
#else	/*  !COFF  */
	0xfff8, 0x4880, l1,    pdreg, "extw",
	0xfff8, 0x48c0, l1,    pdreg, "extl",
#endif	/*  !COFF  */
	0xfb80, 0x4880, lmvml, pmvml, 0,
	0xffc0, 0x4ac0, lonew, ppea,  "tas",
	0xff00, 0x4a00, lone,  pone,  "tst",
	0xfff0, 0x4e40, l1,    ptrap, 0,
	0xfff8, 0x4e50, l2,    plink, 0,
	0xfff8, 0x4e58, l1,    pareg, "unlk\t%s",
#ifdef	COFF
	0xfff8, 0x4e60, l1,    pareg, "mov.l\t%s,%%usp",
	0xfff8, 0x4e68, l1,    pareg, "mov.l\t%%usp,%s",
#else	/*  !COFF  */
	0xfff8, 0x4e60, l1,    pareg, "movl\t%s,usp",
	0xfff8, 0x4e68, l1,    pareg, "movl\tusp,%s",
#endif	/*  !COFF  */
	0xffff, 0x4e70, l1,    pareg, "reset",
	0xffff, 0x4e71, l1,    pareg, "nop",
	0xffff, 0x4e72, l2,    pstop, 0,
	0xffff, 0x4e73, el1,   pareg, "rte",
	0xffff, 0x4e75, el1,   pareg, "rts",
	0xffff, 0x4e76, l1,    pareg, "trapv",
	0xffff, 0x4e77, el1,   pareg, "rtr",
	0xfffe, 0x4e7a, l2,    pmovc, 0,
	0xffc0, 0x4e80, jj,    ppea,  "jsr",
	0xffc0, 0x4ec0, jj,    ppea,  "jmp",
	0xf1c0, 0x4180, lonew, podreg,"chk",
	0xf1c0, 0x41c0, lonel, plea,  0,
	0xf0f8, 0x50c8, lcbch, pdbcc, 0,
	0xf0c0, 0x50c0, lonew, pscc,  0,
	0xf100, 0x5000, lone,  pqu,   "add",
	0xf100, 0x5100, lone,  pqu,   "sub",
	0xf100, 0x7000, l1,    pmqu,  0,
	0xf1c0, 0x80c0, lonew, podreg,"divu",
	0xf1c0, 0x81c0, lonew, podreg,"divs",
	0xf1f0, 0x8100, l1,    ptreg, "sbcd",
	0xf000, 0x8000, loone, pomode,"or",
#ifdef	COFF
	0xf1f0, 0x9100, l1,    ptreg, "subx.b",
	0xf1f0, 0x9140, l1,    ptreg, "subx.w",
	0xf1f0, 0x9180, l1,    ptreg, "subx.l",
#else	/*  !COFF  */
	0xf1f0, 0x9100, l1,    ptreg, "subxb",
	0xf1f0, 0x9140, l1,    ptreg, "subxw",
	0xf1f0, 0x9180, l1,    ptreg, "subxl",
#endif	/*  !COFF  */
	0xf000, 0x9000, loone, pomode,"sub",
#ifdef	COFF
	0xf1f8, 0xb108, l1,    pcmpm, "cmp.b",		/* CMPM	*/
	0xf1f8, 0xb148, l1,    pcmpm, "cmp.w",		/* CMPM	*/
	0xf1f8, 0xb188, l1,    pcmpm, "cmp.l",		/* CMPM	*/
#else	/*  !COFF  */
	0xf1f8, 0xb108, l1,    pcmpm, "cmpmb",
	0xf1f8, 0xb148, l1,    pcmpm, "cmpmw",
	0xf1f8, 0xb188, l1,    pcmpm, "cmpml",
#endif	/*  !COFF  */
	0xf100, 0xb000, loone, pomode,"cmp",
	0xf1c0, 0xb1c0, loone, pomode,"cmp",
	0xf100, 0xb100, loone, pomode,"eor",
	0xf1c0, 0xc0c0, lonew, podreg,"mulu",
	0xf1c0, 0xc1c0, lonew, podreg,"muls",
	0xf1f0, 0xc100, l1,    ptreg, "abcd",
	0xf130, 0xc100, l1,    pexg,  0,
	0xf000, 0xc000, loone, pomode,"and",
#ifdef	COFF
	0xf1f0, 0xd100, l1,    ptreg, "addx.b",
	0xf1f0, 0xd140, l1,    ptreg, "addx.w",
	0xf1f0, 0xd180, l1,    ptreg, "addx.l",
#else	/*  !COFF  */
	0xf1f0, 0xd100, l1,    ptreg, "addxb",
	0xf1f0, 0xd140, l1,    ptreg, "addxw",
	0xf1f0, 0xd180, l1,    ptreg, "addxl",
#endif	/*  !COFF  */
	0xf000, 0xd000, loone, pomode,"add",
	0xf8c0, 0xe0c0, lonew, pmshf,  0,
	0xf000, 0xe000, l1,    pshf,   0,
	0
};

#ifdef	COFF
char	*areg[] = { "%a0", "%a1", "%a2", "%a3", "%a4", "%a5", "%fp", "%sp"};
#else	/*  !COFF  */
char	*areg[] = { "a0", "a1", "a2", "a3", "a4", "a5", "a6", "sp"};
#endif	/*  !COFF  */
char	*cclist[] = { "hi", "ls", "cc", "cs", "ne", "eq", "vc", "vs",
			"pl", "mi", "ge", "lt", "gt", "le"};
	
char	*shtype[] = { "as", "ls", "rox", "ro" };
char	*bittyp[] = { "tst", "chg", "clr", "set" };

#ifdef	COFF
char	*creg[] = { "%sfc", "%dfc", "%usp", "%vbr" };
#else	/*  !COFF  */
char	*creg[] = { "sfc", "dfc", "usp", "vbr" };
#endif	/*  !COFF  */

#ifdef	COFF
int swbegflg = 0;

#endif	/*  COFF  */
/*
 *	Length functions.
 */

int	l1()
{
	return	1;
}

int	l2()
{
	return	2;
}

int	el1(te)
t_entry	*te;
{
	te->t_bchtyp = T_UNBR;
	return	1;
}

int	lea(instr, size, pos)
unsigned  instr, size;
long	pos;
{
	switch  ((instr >> 3) & 0x7)  {
	case  0:
	case  1:
	case  2:
	case  3:
	case  4:
		return	1;
	case  5:
	case  6:
		return	2;
	default:
		switch  (instr & 0x7)  {
		case  0:
		case  2:
		case  3:
			return	2;
		case  1:
			mkdref(pos, size);
			return	3;
		case  4:
			if  (size > 2)
				return	3;
			return	2;
		default:
			return	0;
		}
	}
}

/*
 *	Lengths of move instructions.
 */

int	lmove(te, pos)
t_entry	*te;
long	pos;
{
	register  unsigned  tc  =  te->t_contents;
	unsigned  sz  =  1;
	int	lng, lng2;
	
	lng  = tc & 0xf000;
	if  (lng == 0x3000)
		sz = 2;
	else  if  (lng == 0x2000)
		sz = 4;
	
	if  ((lng = lea(tc, sz, pos+2)) <= 0)
		return	0;
	lng2 = lea(((tc>>3) & 0x38) | ((tc>>9) & 0x7), sz, pos+lng+lng);
	if  (lng2 <= 0)
		return	0;
	return	lng + lng2 - 1;
}

/*
 *	Lengths for conditional branches and dbcc instructions.
 */

int	lcbch(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	long	dest  =  pos + 2;
	int	res   =  2;
	
	if  ((tc & 0xf000) == 0x5000  ||  (tc & 0xff) == 0)
		dest += (short)gettw(&mainfile, pos+2, R_WORD);
	else  {
		dest += (char) tc;
		res = 1;
	}
#ifdef	COFF
 	if ( dest < 0x290000 && (dest < mainfile.ef_tbase 
 		|| dest >= mainfile.ef_tbase+mainfile.ef_tsize 
 		|| (dest & 1) != 0 ))
 		return 0;		/* Illegal branch destination */
#endif	/*  COFF  */
	if  ((tc & 0xff00) == 0x6000)
		te->t_bchtyp = T_UNBR;
	else  if  ((tc & 0xff00) == 0x6100)
		te->t_bchtyp = T_JSR;
	else
		te->t_bchtyp = T_CONDBR;

#ifdef	COFF
 	if ( dest < 0x290000 && ((te->t_relsymb = textlab(dest, pos)) == NULL )) {
 		te->t_bchtyp = T_NOBR;/* Branch to a continuation */
 		return 0;
 	}
#else	/*  !COFF  */
	te->t_relsymb = textlab(dest, pos);
#endif	/*  !COFF  */
	return	res;
}

int	jj(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	t_entry	nextl;
#ifdef	COFF
 	long dest;
#endif	/*  COFF  */
	
	te->t_bchtyp = (tc & 0x40)? T_UNBR: T_JSR;
#ifdef	COFF
	if ((tc & 0x3f) == 0x39) {
#else	/*  !COFF  */
	if  ((tc & 0x3f) == 0x39)  {
#endif	/*  !COFF  */
		gette(&mainfile, pos+2, &nextl);
		if  (nextl.t_relsymb == NULL)  {
#ifdef	COFF
 			dest = gettw(&mainfile, pos + 2, R_LONG );
 			if ( dest < 0x290000 && (dest < mainfile.ef_tbase
 				|| dest >= mainfile.ef_tbase+mainfile.ef_tsize
 				|| (dest & 1) != 0 ))
 				return 0;	/* Illegal branch destination */
 			if ( dest < 0x290000 && ( nextl.t_relsymb = textlab(dest, pos) ) == NULL )
 			return 0;	/* Branch to a continuation */
#else	/*  !COFF  */
			nextl.t_relsymb = textlab(gettw(&mainfile, pos+2, R_LONG), pos);
#endif	/*  !COFF  */
			putte(&mainfile, pos+2, &nextl);
		}
		te->t_relsymb = nextl.t_relsymb;	/*  Easy ref  */
	}
#ifdef	COFF
	else if ((tc & 0x3f) == 0x3a) {
		gette(&mainfile, pos+2, &nextl);
		if  (nextl.t_relsymb == NULL)  {
			dest = pos+2+ (int)((short) 
				gettw(&mainfile, pos + 2, R_WORD ));
 			if ( dest < 0x290000 && (dest < mainfile.ef_tbase
 			    || dest >= mainfile.ef_tbase+mainfile.ef_tsize
 			    || (dest & 1) != 0 ))
			return	lea(tc, 4, pos+2);
 			if (dest < 0x290000 && 
			    (nextl.t_relsymb = textlab(dest, pos)) == NULL)
	 			return 0;	/* Branch to a continuation */
			putte(&mainfile, pos+2, &nextl);
		}
		te->t_relsymb = nextl.t_relsymb;	/*  Easy ref  */
	}
#endif	/*  COFF  */
	return	lea(tc, 4, pos+2);
}

int	limed(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	int	lng;
	
	/*
	 *	Specifically exclude byte address register operands,
	 *	and ones which have lengths of 3.
	 */

	if  ((tc & 0xf8) == 0x08)
		return  0;
	
	if  ((tc & 0xc0) >= 0x80)  {
		if  (tc & 0x40)
			return  0;
		lng = lea(tc, 4, pos+6);
		if  (lng > 0)
			lng += 2;
	}
	else  {
		lng = lea(tc, (unsigned)((tc & 0xc0)?2:1), pos+4);
		if  (lng > 0)
			lng++;
	}
	return	lng;
}

int	lsbit(te, pos)
t_entry	*te;
long	pos;
{
	int	lng = lea(te->t_contents, 1, pos+4);
	
	if  (lng > 0)
		lng++;
	return	lng;
}

int	lmvml(te, pos)
t_entry	*te;
long	pos;
{
	int	lng = lea(te->t_contents,
			(unsigned)(te->t_contents&0x40? 4:2), pos+4);
	
	if  (lng > 0)
		lng++;
	return	lng;
}

/*
 *	Length depends on bits 6 and 7 of instruction.
 */

int	lone(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	
	return	lea(tc, 1 << ((tc >> 6) & 3), pos+2);
}

/*
 *	Length depends on bits 6-8 of instruction.
 */

int	loone(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	
	switch  ((tc >> 6) & 7)  {
	case  0:
	case  4:
		return	lea(tc, 1, pos+2);
	case  1:
	case  3:
	case  5:
		return  lea(tc, 2, pos+2);
	case  2:
	case  6:
	case  7:
		return	lea(tc, 4, pos+2);
	}
	/*NOTREACHED*/
}

int	lonew(te, pos)
t_entry	*te;
long	pos;
{
#ifdef	COFF
	if (te->t_contents == 0x4afc) { /* swbeg ... */
		swbegflg++;
		return (2 + gettw(&mainfile,pos+2,2));
	}
#endif	/*  COFF  */
	return	lea(te->t_contents, 2, pos+2);
}

int	lonel(te, pos)
t_entry	*te;
long	pos;
{
	return	lea(te->t_contents, 4, pos+2);
}

/*
 *	Print routines.
 */

#ifdef	COFF
/*
 * print out small integers in decamil notation, all others in hex.
 */

void prind(n)
unsigned short n;
{
	if ((short) n > -128 && (short) n < 128)
		(void) printf("%d", (long) ((short) n));
	else
		(void) printf("0x%x",(unsigned long) n);
}
   
#endif	/*  COFF  */
int	findleng(tc)
unsigned  tc;
{
	switch  ((tc >> 6) & 3)  {
	case  0:
		return	'b';
	case  1:
		return	'w';
	default:
		return	'l';
#ifndef	COFF
	}
#endif	/*  !COFF  */
}
#ifdef	COFF
}
#endif	/*  COFF  */

#ifdef	COFF
/* print @(0x4,d0.l) */
void	piword(reg,disp)
char * reg;
#else	/*  !COFF  */
void	piword(disp)
#endif	/*  !COFF  */
unsigned  disp;
{
#ifdef	COFF
int	szc;

	(void) printf("%d(%s,", disp & 0xff, reg);
	if  (disp & 0x8000) {
		(void) fputs(areg[(disp >> 12) & 0x7]);
		(void) putchar('.');
	}
#else	/*  !COFF  */
	int	szc;
	
	(void) printf("@(0x%x,", disp & 0xff);
	if  (disp & 0x8000)
		(void) printf("%s", areg[(disp >> 12) & 0x7]);
#endif	/*  !COFF  */
	else
#ifdef	COFF
		(void) printf("%%d%d.", (disp >> 12) & 0x7);
	(void) putchar((disp & (1 << 10)) ? 'l' :'w');
	(void) putchar(')');
#else	/*  !COFF  */
		(void) printf("d%d", (disp >> 12) & 0x7);
	szc = 'w';
	if  (disp & (1 << 10))
		szc = 'l';
	(void) printf(":%c)", szc);
#endif	/*  !COFF  */
}

#ifdef	COFF
extern struct	commit	abstab;

#endif	/*  COFF  */
void	paddr(pos)
long	pos;
{
	t_entry	tent;
	symbol	symb;
	

	gette(&mainfile, pos, &tent);
	if  (tent.t_relsymb != NULL)  {
		symb = tent.t_relsymb;
		if  (symb->s_lsymb != 0)
#ifdef	COFF
			(void) printf("L%%%u", symb->s_lsymb);
		else
			(void) fputs(symb->s_name, stdout);
		if  (tent.t_reldisp != 0)
		    (void) printf("+0x%x", tent.t_reldisp);
#else	/*  !COFF  */
			(void) printf("%u$", symb->s_lsymb);
		else
			(void) printf("%s", symb->s_name);
		if  (tent.t_reldisp != 0)
			(void) printf("+0x%x", tent.t_reldisp);
#endif	/*  !COFF  */
		return;
	}
#ifdef	COFF
	if ((pos = gettw(&mainfile, pos, R_LONG)) >= 0x290000)
	 {
	    register int i;
	    for (i=0; i < abstab.c_int; i++)
	        if (abstab.c_symb[i]->s_value == pos)
		 {	     
		    (void) fputs(abstab.c_symb[i]->s_name, stdout);
		    return;
		 }
	 }
	(void) printf("0x%x", pos);
#else	/*  !COFF  */
	(void) printf("0x%x", gettw(&mainfile, pos, R_LONG));
#endif	/*  !COFF  */
}

int	prea(ea, pos, sz)
unsigned  ea, sz;
long	pos;			/*  Address of previous word to extn  */
{
	unsigned  reg  =  ea & 0x7;
	long	disp;
#ifdef	COFF
	t_entry	tent;
#endif	/*  COFF  */
	
	pos += 2;
	
	switch  ((ea >> 3) & 0x7)  {
	case  0:
#ifdef	COFF
		(void) printf("%%d%d", reg);
#else	/*  !COFF  */
		(void) printf("d%d", reg);
#endif	/*  !COFF  */
		return	0;
	case  1:
#ifdef	COFF
		(void) fputs(areg[reg], stdout);
#else	/*  !COFF  */
		(void) printf("%s", areg[reg]);
#endif	/*  !COFF  */
		return	0;
	case  2:
#ifdef	COFF
		(void) printf("(%s)", areg[reg]);
#else	/*  !COFF  */
		(void) printf("%s@", areg[reg]);
#endif	/*  !COFF  */
		return	0;
	case  3:
#ifdef	COFF
		(void) printf("(%s)+", areg[reg]);
#else	/*  !COFF  */
		(void) printf("%s@+", areg[reg]);
#endif	/*  !COFF  */
		return	0;
	case  4:
#ifdef	COFF
		(void) printf("-(%s)", areg[reg]);
#else	/*  !COFF  */
		(void) printf("%s@-", areg[reg]);
#endif	/*  !COFF  */
		return	0;
	case  5:
		disp = gettw(&mainfile, pos, R_WORD);
#ifdef	COFF
		(void) prind(disp);
		(void) printf("(%s)", areg[reg]);
#else	/*  !COFF  */
		(void) printf("%s@(0x%x)", areg[reg], disp);
#endif	/*  !COFF  */
		return	2;
	case  6:
#ifdef	COFF
		piword(areg[reg], (unsigned) gettw(&mainfile, pos, R_WORD));
#else	/*  !COFF  */
		(void) printf("%s", areg[reg]);
		piword((unsigned) gettw(&mainfile, pos, R_WORD));
#endif	/*  !COFF  */
		return	2;
	default:
		switch  (reg)  {
		case  0:
			disp = gettw(&mainfile, pos, R_WORD);
#ifdef	COFF
			(void) prind(disp);
			(void) putchar('.');
			(void) putchar('w');
#else	/*  !COFF  */
			(void) printf("0x%x:w", disp);
#endif	/*  !COFF  */
			return	2;
		case  1:
			paddr(pos);
			return	4;
#ifdef	COFF
		case  2:{
			symbol symb;
			register int addr;
			disp = 
			    ((short) gettw(&mainfile, pos, R_WORD));
  			if ((addr=pos+disp) < 0  ||  
			    addr-mainfile.ef_tbase > mainfile.ef_tsize ||
			    (addr & 1) != 0)  
				goto skiplabs;
			gette(&mainfile, addr, &tent);
			if  (tent.t_relsymb != NULL)  {
				symb = tent.t_relsymb;
			}
			else if (tent.t_lab != NULL) {
				symb = tent.t_lab;
			}
			else {
		skiplabs:
				(void) prind(disp);
				(void) fputs("(%pc)", stdout);
				return	2;
			}
			if  (symb->s_lsymb != 0)
				(void) printf("L%%%u", symb->s_lsymb);
			else
				(void) fputs(symb->s_name, stdout);
			if  (tent.t_reldisp != 0)
				(void) printf("+0x%x", tent.t_reldisp);
			return 2;
			}
#else	/*  !COFF  */
		case  2:
			disp = gettw(&mainfile, pos, R_WORD);
			(void) printf("pc@(0x%x)", disp);
			return	2;
#endif	/*  !COFF  */
		case  3:
#ifdef	COFF
			piword("%pc", (unsigned)gettw(&mainfile, pos, R_WORD));
#else	/*  !COFF  */
			(void) printf("pc");
			piword((unsigned) gettw(&mainfile, pos, R_WORD));
#endif	/*  !COFF  */
			return	2;
		case  4:
#ifdef	COFF
			(void) putchar('&');
#else	/*  !COFF  */
			(void) printf("#");
#endif	/*  !COFF  */
			if  (sz < 4)
#ifdef	COFF
				(void) prind(gettw(&mainfile, pos, R_WORD));
#else	/*  !COFF  */
				(void) printf("0x%x", gettw(&mainfile, pos, R_WORD));
#endif	/*  !COFF  */
			else
				paddr(pos);
			return	sz;
		default:
			(void) fprintf(stderr, "Funny mode\n");
			exit(220);
		}
	}
	/*NOTREACHED*/
}
	
int	pmove(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  sz  =  2;
	unsigned  tc  =  te->t_contents;
	
	(void) printf("mov%s\t", optab[te->t_iindex].prarg);
	
	if  ((tc & 0xf000) == 0x2000)
		sz = 4;
	
	pos += prea(tc, pos, sz);
#ifdef	COFF
	(void) putchar(',');
#else	/*  !COFF  */
	putchar(',');
#endif	/*  !COFF  */
	(void) prea(((tc >> 9) & 0x7) | ((tc >> 3) & 0x38), pos, sz);
}

int	pcbch(te)
t_entry	*te;
{
	int	cc = ((te->t_contents >> 8) & 0xf) - 2;
	char	*msg;
	register  symbol  ts;
	
	if  (cc < 0)
		msg = cc < -1? "ra": "sr";
	else
		msg = cclist[cc];
	(void) printf("b%s", msg);
#ifdef	COFF
/* this specifically requests that 8 bit addressing be used, 
		but the unixpc assembler will do this automatically.	
	if  (te->t_lng < 2) {
		(void) putchar('.');
		(void) putchar('b');
	}
*/
#else	/*  !COFF  */
	if  (te->t_lng < 2)
		(void) printf("s");
#endif	/*  !COFF  */
	ts = te->t_relsymb;
	if  (ts->s_lsymb != 0)
#ifdef	COFF
		(void) printf("\tL%%%u", ts->s_lsymb);
#else	/*  !COFF  */
		(void) printf("\t%u$", ts->s_lsymb);
#endif	/*  !COFF  */
	else
#ifdef	COFF
	{
		(void) putchar('\t');
		(void) fputs(ts->s_name, stdout);
	}
#else	/*  !COFF  */
		(void) printf("\t%s", ts->s_name);
#endif	/*  !COFF  */
}

int	pdbcc(te)
t_entry	*te;
{
	unsigned  tc  =  te->t_contents;
	int	cc = ((tc >> 8) & 0xf) - 2;
	char	*msg;
	register  symbol  ts;
	
	if  (cc < 0)
		msg = cc < -1? "t": "f";
	else
		msg = cclist[cc];
	ts = te->t_relsymb;
#ifdef	COFF
	(void) printf("db%s\t%%d%d,", msg, tc & 0x7);
	if  (ts->s_lsymb)
		(void) printf("L%%%u", ts->s_lsymb);
	else
		(void) fputs(ts->s_name, stdout);
#else	/*  !COFF  */
	(void) printf("db%s\td%d,", msg, tc & 0x7);
	if  (ts->s_lsymb)
		(void) printf("%u$", ts->s_lsymb);
	else
		(void) printf("%s", ts->s_name);
#endif	/*  !COFF  */
}

int	pscc(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	int	cc = ((tc >> 8) & 0xf) - 2;
	char	*msg;
	
	if  (cc < 0)
		msg = cc < -1? "t": "f";
	else
		msg = cclist[cc];
	(void) printf("s%s\t", msg);
	(void) prea(tc, pos, 1);
}

int	pcs(te, pos)
t_entry	*te;
long	pos;
{
	long	disp  =  gettw(&mainfile, pos+2, R_WORD);
	
#ifdef	COFF
	(void) fputs(optab[te->t_iindex].prarg, stdout);
	if  ((te->t_contents & 0xc0) == 0){
	 	(void) fputs(".b\t&", stdout);
		(void) prind(disp);
		(void) fputs(",%cc", stdout);
	}
	else {
		(void) fputs(".w\t&", stdout);
		(void) prind(disp);
		(void) fputs(",%sr", stdout);
	}
#else	/*  !COFF  */
	(void) printf("%s", optab[te->t_iindex].prarg);
	if  ((te->t_contents & 0xc0) == 0)
		(void) printf("b\t#0x%x,cc", disp);
	else
		(void) printf("w\t#0x%x,sr", disp);
#endif	/*  !COFF  */
}

int	pmovc(te, pos)
t_entry	*te;
long	pos;
{
	int	disp = gettw(&mainfile, pos+2, R_WORD);
	int	ctrl = ((disp >> 10) & 2) | (disp & 1);

#ifdef	COFF
	(void) fputs("movc\t", stdout);
#else	/*  !COFF  */
	(void) printf("movec\t");
#endif	/*  !COFF  */
	if  ((te->t_contents & 1) == 0)
#ifdef	COFF
		(void) fputs(creg[ctrl], stdout);
	if  (disp & 0x8000)
	{
		(void) fputs(areg[(disp >> 12) & 7], stdout);
		(void) putchar(',');
	}
	else
		(void) printf("%%d%d,", disp >> 12);
	if  (te->t_contents & 1)
		(void) fputs(creg[ctrl], stdout);
#else	/*  !COFF  */
		(void) printf("%s,", creg[ctrl]);
	if  (disp & 0x8000)
		(void) printf("%s", areg[(disp >> 12) & 7]);
	else
		(void) printf("d%d", disp >> 12);
	if  (te->t_contents & 1)
		(void) printf(",%s", creg[ctrl]);
#endif	/*  !COFF  */
}

int	pimed(te, pos)
t_entry	*te;
long	pos;
{
	int	sz = findleng(te->t_contents);
#ifdef	COFF

	/* we need to swith the operands to compare instrucions. */
	if (strcmp (optab[te->t_iindex].prarg, "cmp")) {
		(void) printf("%s.%c\t&", optab[te->t_iindex].prarg, sz);
		if  (sz == 'l')  {
			paddr(pos+2);
			(void) putchar(',');
			(void) prea(te->t_contents, pos+4, 4);
		}
		else  {
			(void) prind(gettw(&mainfile, pos+2, R_WORD));
			(void) putchar(',');
			(void) prea(te->t_contents, pos+2, 2);
		}
#else	/*  !COFF  */
	
	(void) printf("%s%c\t#", optab[te->t_iindex].prarg, sz);
	if  (sz == 'l')  {
		paddr(pos+2);
		putchar(',');
		(void) prea(te->t_contents, pos+4, 4);
#endif	/*  !COFF  */
	}
#ifdef	COFF
	else {
		(void) printf("%s.%c\t", optab[te->t_iindex].prarg, sz);
		if  (sz == 'l')  {
			(void) prea(te->t_contents, pos+4, 4);
			(void) putchar(',');
			(void) putchar('&');
			paddr(pos+2);
		}
		else  {
			(void) prea(te->t_contents, pos+2, 2);
			(void) putchar(',');
			(void) putchar('&');
			(void) prind(gettw(&mainfile, pos+2, R_WORD));
		}
#else	/*  !COFF  */
	else  {
		(void) printf("0x%x,", gettw(&mainfile, pos+2, R_WORD));
		(void) prea(te->t_contents, pos+2, 2);
#endif	/*  !COFF  */
	}
}

int	pmovp(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	long	disp  =  gettw(&mainfile, pos+2, R_WORD);
	int	dreg = tc >> 9;
	char	*ar = areg[tc & 0x7];
	
#ifdef	COFF
	(void) fputs("movp.", stdout);
	if  (tc & (1 << 6))
		(void) putchar('l');
	else
		(void) putchar('w');
#else	/*  !COFF  */
	(void) printf("movep");
	if  (tc & (1 << 6))
		putchar('l');
	else
		putchar('w');
#endif	/*  !COFF  */

#ifdef	COFF
	if  (tc & (1 << 7)) {
		(void) printf("\t%%d%d,", dreg);
		(void) prind(disp);
		(void) printf("(%s)", ar);
	}
	else {
		(void) putchar('\t');
		(void) prind(disp);
		(void) printf("(%s),%%d%d", ar, dreg);
	}
#else	/*  !COFF  */
	if  (tc & (1 << 7))
		(void) printf("\td%d,%s@(0x%x)", dreg, ar, disp);
	else
		(void) printf("\t%s@(0x%x),d%d", ar, disp, dreg);
#endif	/*  !COFF  */
}

int	psbit(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	
#ifdef	COFF
	(void) printf("b%s\t&%d,", bittyp[(tc >> 6) & 0x3], gettw(&mainfile, pos+2, R_WORD));
#else	/*  !COFF  */
	(void) printf("b%s\t#%d,", bittyp[(tc >> 6) & 0x3], gettw(&mainfile, pos+2, R_WORD));
#endif	/*  !COFF  */
	(void) prea(tc, pos+2, 1);
}

/*ARGSUSED*/
int	pstop(te, pos)
t_entry	*te;
long	pos;
{
#ifdef	COFF
	(void) printf("stop\t&0x%x", gettw(&mainfile, pos+2, R_WORD));
#else	/*  !COFF  */
	(void) printf("stop\t#0x%x", gettw(&mainfile, pos+2, R_WORD));
#endif	/*  !COFF  */
}

int	pdbit(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	
#ifdef	COFF
	(void) printf("b%s\t%%d%d,", bittyp[(tc >> 6) & 0x3], (tc >> 9) & 0x7);
#else	/*  !COFF  */
	(void) printf("b%s\td%d,", bittyp[(tc >> 6) & 0x3], (tc >> 9) & 0x7);
#endif	/*  !COFF  */
	(void) prea(tc, pos, 1);
}

int	pcs2(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	
#ifdef	COFF
	(void) fputs("movw\t", stdout);
#else	/*  !COFF  */
	(void) printf("movw\t");
#endif	/*  !COFF  */
	if  ((tc & 0xffc0) == 0x40c0)  {
#ifdef	COFF
		(void) fputs("%sr,", stdout);
#else	/*  !COFF  */
		(void) printf("sr,");
#endif	/*  !COFF  */
		(void) prea(tc, pos, 2);
	}
	else  {
		(void) prea(tc, pos, 2);
#ifdef	COFF
		(void) putchar(',');
		(void) fputs(optab[te->t_iindex].prarg, stdout);
#else	/*  !COFF  */
		(void) printf(",%s", optab[te->t_iindex].prarg);
#endif	/*  !COFF  */
	}
}

int	pone(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	int	sz = findleng(tc);
	
#ifdef	COFF
	(void) printf("%s.%c\t", optab[te->t_iindex].prarg, sz);
#else	/*  !COFF  */
	(void) printf("%s%c\t", optab[te->t_iindex].prarg, sz);
#endif	/*  !COFF  */
	(void) prea(tc, pos, (unsigned)(sz == 'l'? 4: 2));
}

int	ppea(te, pos)	/*  nbcd, pea, tas, jmp, jsr  */
t_entry	*te;
long	pos;
{
#ifdef	COFF
	if (! strncmp(optab[te->t_iindex].prarg, "tas") && 
	    (te->t_contents & 0x3f) == 0x3c) {
		symbol symb;
		t_entry tstr;
		int counter = te->t_lng -2;
		int offset = (pos += 4);
		int dest;
		char * sw_label;

		(void) printf("swbeg\t&%d\n", counter);

		symb = textlab(pos, pos);
		printf("%s:\n", sw_label = symb->s_name);

		while (counter--) {
			gette(&mainfile, pos, &tstr);
			dest = tstr.t_contents + offset;
 			if (tstr.t_contents > 0 &&
			    dest < 0x290000 && 
			    ! (dest < mainfile.ef_tbase
 			     || dest >= mainfile.ef_tbase+mainfile.ef_tsize
 			     || (dest & 1) != 0 )) {
				if (symb = textlab(dest,offset))
					printf("\tshort\t%s-%s\n", 
						symb->s_name,sw_label);
				else
					printf("\tshort\t0x%x\t# Can't label destination.\n", 
							tstr.t_contents);
			}
			else 
				printf("\tshort\t0x%x\t# Illegal address\n", 
						tstr.t_contents);
			pos += 2;
		}
	}
	else {
		(void) fputs(optab[te->t_iindex].prarg, stdout);
		(void) putchar('\t');
		(void) prea(te->t_contents, pos, (unsigned)(te->t_lng>2?4:2));
	}
#else	/*  !COFF  */
	(void) printf("%s\t", optab[te->t_iindex].prarg);
	(void) prea(te->t_contents, pos, (unsigned)(te->t_lng > 2? 4: 2));
#endif	/*  !COFF  */
}


int	plea(te, pos)
t_entry	*te;
long	pos;
{
#ifdef	COFF
	(void) fputs("lea\t", stdout);
#else	/*  !COFF  */
	(void) printf("lea\t");
#endif	/*  !COFF  */
	(void) prea(te->t_contents, pos, 4);
#ifdef	COFF
	(void) putchar(',');
	(void) fputs(areg[(te->t_contents >> 9) & 0x7], stdout);
#else	/*  !COFF  */
	(void) printf(",%s", areg[(te->t_contents >> 9) & 0x7]);
#endif	/*  !COFF  */
}

int	pdreg(te)
t_entry	*te;
{
#ifdef	COFF
	(void) printf("%s\t%%d%d", optab[te->t_iindex].prarg, te->t_contents & 7);
#else	/*  !COFF  */
	(void) printf("%s\td%d", optab[te->t_iindex].prarg, te->t_contents & 7);
#endif	/*  !COFF  */
}


int	pmvml(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	register  unsigned  dw  =  gettw(&mainfile, pos+2, R_WORD);
	unsigned  sz = 4;
	int	sc = 'l';
	register  int	i;
	register  unsigned  bit;
	
#ifdef	COFF
	(void) fputs("movm.", stdout);
	if  ((tc & 0x40) == 0)  {
		sc = 'w';
		sz = 2;
	}
	(void) putchar(sc);	
	(void) putchar('\t');
	if  (tc & 0x400)  {
		(void) prea(tc, pos+2, sz);
		(void) printf(",&0x%x", dw);
	}
	else  {
		(void) printf("&0x%x,", dw);
		(void) prea(tc, pos+2, sz);
	}
	(void) printf("\t#\t");

#else	/*  !COFF  */
	(void) printf("movem");
	if  ((tc & 0x40) == 0)  {
		sc = 'w';
		sz = 2;
	}
	
	(void) printf("%c\t", sc);
	
	if  (tc & 0x400)  {
		(void) prea(tc, pos+2, sz);
		(void) printf(",#0x%x", dw);
	}
	else  {
		(void) printf("#0x%x,", dw);
		(void) prea(tc, pos+2, sz);
	}
	(void) printf("\t|");	
#endif	/*  !COFF  */
	
	(void) printf("\t|");
	
	if  ((tc & 0x38) == 0x20)  {
		bit = 0x8000;
		for  (i = 0;  i < 8;  i++)  {
			if  (dw & bit)
#ifdef	COFF
				(void) printf(" %%d%d", i);
#else	/*  !COFF  */
				(void) printf(" d%d", i);
#endif	/*  !COFF  */
			bit >>= 1;
		}
		for  (i = 0;  i < 8;  i++)  {
#ifdef	COFF
			if  (dw & bit) {
			        (void) putchar(' ');
				(void) fputs(areg[i], stdout);
			}
#else	/*  !COFF  */
			if  (dw & bit)
				(void) printf(" %s", areg[i]);
#endif	/*  !COFF  */
			bit >>= 1;
		}
	}
	else  {
		bit = 1;
		for  (i = 0;  i < 8;  i++)  {
			if  (dw & bit)
#ifdef	COFF
				(void) printf(" %%d%d", i);
#else	/*  !COFF  */
				(void) printf(" d%d", i);
#endif	/*  !COFF  */
			bit <<= 1;
		}
		for  (i = 0;  i < 8;  i++)  {
#ifdef	COFF
			if  (dw & bit) {
				(void) putchar(' ');
				(void) fputs(areg[i], stdout);
			}
#else	/*  !COFF  */
			if  (dw & bit)
				(void) printf(" %s", areg[i]);
#endif	/*  !COFF  */
			bit <<= 1;
		}
	}
}

int	ptrap(te)
t_entry	*te;
{
#ifdef	COFF
	(void) printf("trap\t&%d", te->t_contents & 0xf);
#else	/*  !COFF  */
	(void) printf("trap\t#0x%x", te->t_contents & 0xf);
#endif	/*  !COFF  */
}

int	plink(te, pos)
t_entry	*te;
long	pos;
{
#ifdef	COFF
	(void) printf("link\t%s,&%d", areg[te->t_contents & 0x7],
#else	/*  !COFF  */
	(void) printf("link\t%s,#0x%x", areg[te->t_contents & 0x7],
#endif	/*  !COFF  */
				gettw(&mainfile, pos+2, R_WORD));
}


int	pareg(te)
t_entry	*te;
{
	(void) printf(optab[te->t_iindex].prarg, areg[te->t_contents & 0x7]);
}

int	podreg(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	
	(void) printf("%s\t", optab[te->t_iindex].prarg);
	(void) prea(tc, pos, 2);
#ifdef	COFF
	(void) printf(",%%d%d", (tc >> 9) & 0x7);
#else	/*  !COFF  */
	(void) printf(",d%d", (tc >> 9) & 0x7);
#endif	/*  !COFF  */
}

int	pqu(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	int	sz  =  findleng(tc);
	int	amt = (tc >> 9) & 0x7;
	
	if  (amt == 0)
		amt = 8;
#ifdef	COFF
	(void) printf("%s.%c\t&%d,", optab[te->t_iindex].prarg, sz, amt);
#else	/*  !COFF  */
	(void) printf("%sq%c\t#%d,", optab[te->t_iindex].prarg, sz, amt);
#endif	/*  !COFF  */
	(void) prea(tc, pos, (unsigned)(sz == 'l'? 4: 2));
}

int	pmqu(te)
t_entry	*te;
{
	unsigned  tc  =  te->t_contents;

#ifdef	COFF
	(void) printf("mov.l\t&%d,%%d%d", (char)tc, (tc >> 9) & 0x7);
#else	/*  !COFF  */
	(void) printf("moveq\t#0x%x,d%d", (char)tc, (tc >> 9) & 0x7);
#endif	/*  !COFF  */
}

int	ptreg(te)
t_entry	*te;
{
	register  unsigned  tc  =  te->t_contents;
	int	rx = (tc >> 9) & 0x7;
	int	ry = tc & 0x7;

#ifdef	COFF
	(void) fputs(optab[te->t_iindex].prarg, stdout);
	(void) putchar('\t');
	if  (tc & 0x8)
		(void) printf("-(%s),-(%s)", areg[ry], areg[rx]);
	else
		(void) printf("%%d%d,%%d%d", ry, rx);
#else	/*  !COFF  */
	(void) printf("%s\t", optab[te->t_iindex].prarg);
	if  (tc & 0x8)
		(void) printf("%s@-,%s@-", areg[ry], areg[rx]);
	else
		(void) printf("d%d,d%d", ry, rx);
#endif	/*  !COFF  */

}

int	pcmpm(te)
t_entry	*te;
{
	register  unsigned  tc  =  te->t_contents;

#ifdef	COFF
	(void) printf("%s\t(%s)+,(%s)+", optab[te->t_iindex].prarg,
#else	/*  !COFF  */
	(void) printf("%s\t%s@+,%s@+", optab[te->t_iindex].prarg,
#endif	/*  !COFF  */
		areg[tc & 7], areg[(tc >> 9) & 7]);
}

int	pomode(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
#ifdef	COFF
	char	buf[5];
#else	/*  !COFF  */
	char	bef[4], aft[4];
#endif	/*  !COFF  */
	int	sz;
	int	reg = (tc >> 9) & 7;

#ifdef	COFF
	buf[0] = '\0';
#else	/*  !COFF  */
	bef[0] = aft[0] = '\0';
#endif	/*  !COFF  */
	
	switch  ((tc >> 6) & 7)  {
	case  0:
		sz = 'b';
		goto  toreg;
	case  1:
		sz = 'w';
		goto  toreg;
	case  2:
		sz = 'l';
	toreg:
#ifdef	COFF
 		(void) sprintf(buf, "%%d%d", reg);
		goto printaft;
#else	/*  !COFF  */
		(void) sprintf(aft, ",d%d", reg);
		break;
#endif	/*  !COFF  */
	case  3:
		sz = 'w';
		goto  toareg;
	case  7:
		sz = 'l';
	toareg:
#ifdef	COFF
		(void) sprintf(buf, "%s", areg[reg]);
	printaft:
		if (strcmp("cmp", optab[te->t_iindex].prarg)) {
			(void) printf("%s.%c\t", optab[te->t_iindex].prarg,sz);
			(void) prea(tc, pos, (unsigned)(sz == 'l'? 4: 2));
			(void) putchar(',');
			(void) fputs(buf, stdout);
		}
		else {
			(void) printf("%s.%c\t%s,", optab[te->t_iindex].prarg,
						    sz, buf);
			(void) prea(tc, pos, (unsigned)(sz == 'l'? 4: 2));
		}
#else	/*  !COFF  */
		(void) sprintf(aft, ",%s", areg[reg]);
#endif	/*  !COFF  */
		break;
	case  4:
		sz = 'b';
		goto  frreg;
	case  5:
		sz = 'w';
		goto  frreg;
	case  6:
		sz = 'l';
	frreg:
#ifdef	COFF
		(void) sprintf(buf, "%%d%d", reg);
		if (strcmp("cmp", optab[te->t_iindex].prarg)) {
			(void) printf("%s.%c\t%s,", optab[te->t_iindex].prarg, 
					sz, buf);
			(void) prea(tc, pos, (unsigned)(sz == 'l'? 4: 2));
		}
		else {
			(void) printf("%s.%c\t", optab[te->t_iindex].prarg,sz);
			(void) prea(tc, pos, (unsigned)(sz == 'l'? 4: 2));
			(void) putchar(',');
			(void) fputs(buf, stdout);
		}
#else	/*  !COFF  */
		(void) sprintf(bef, "d%d,", reg);
		break;
#endif	/*  !COFF  */
	}
#ifndef	COFF

	(void) printf("%s%c\t%s", optab[te->t_iindex].prarg, sz, bef);
	(void) prea(tc, pos, (unsigned)(sz == 'l'? 4: 2));
	(void) printf(aft);
#endif	/*  !COFF  */
}

int	pexg(te)
t_entry	*te;
{
	unsigned  tc  =  te->t_contents;
	int	r1 = (tc >> 9) & 7, r2 = tc & 7;

	(void) printf("exg\t");
	

#ifdef	COFF
	if  ((tc & 0x00f8) == 0x0048)	{
		(void) fputs(areg[r1], stdout);
		(void) putchar(',');
	}
	else
		(void) printf("%%d%d,", r1);
	if  (tc & 0x8)
		(void) fputs(areg[r2], stdout);
	else
		(void) printf("%%d%d", r2);
#else	/*  !COFF  */	
	if  ((tc & 0x00f8) == 0x0048)
		(void) printf("%s,", areg[r1]);
	else
		(void) printf("d%d,", r1);
	if  (tc & 0x8)
		(void) printf("%s", areg[r2]);
	else
		(void) printf("d%d", r2);
#endif	/*  !COFF  */

}
	
int	pmshf(te, pos)
t_entry	*te;
long	pos;
{
	unsigned  tc  =  te->t_contents;
	
#ifdef	COFF
	(void) printf("%s%c.w\t", shtype[(tc >> 9) & 3], tc & 0x100? 'l': 'r');
#else	/*  !COFF  */
	(void) printf("%s%cw\t", shtype[(tc >> 9) & 3], tc & 0x100? 'l': 'r');
#endif	/*  !COFF  */
	(void) prea(tc, pos, 2);
}

int	pshf(te)
t_entry	*te;
{
	unsigned  tc  =  te->t_contents;
	int	sz  =  findleng(tc);
	int	disp = (tc >> 9) & 7;

#ifdef	COFF
	(void) printf("%s%c.%c\t", shtype[(tc >> 3) & 3], tc & 0x100? 'l': 'r', sz);
	if  (tc & 0x20)
		(void) printf("%%d%d", disp);
	else
		(void) printf("&%d", disp == 0? 8: disp);
	(void) printf(",%%d%d", tc & 7);
#else	/*  !COFF  */
	(void) printf("%s%c%c\t", shtype[(tc >> 3) & 3], tc & 0x100? 'l': 'r', sz);
	if  (tc & 0x20)
		(void) printf("d%d", disp);
	else
		(void) printf("#%d", disp == 0? 8: disp);
	(void) printf(",d%d", tc & 7);
#endif	/*  !COFF  */

}

/*
 *	Find length etc of instruction.
 */

int	findinst(te, pos)
register  t_entry  *te;
long	pos;
{
	register  struct  opstr	*op;
	unsigned  tc  =  te->t_contents;
	int	lng = 0;
	register  int	i;

	te->t_type = T_BEGIN;
	te->t_bchtyp = T_NOBR;
	
	for  (op = &optab[0];  op->mask;  op++)  {
		if  ((tc & op->mask) == op->match)  {
			te->t_iindex = op - optab;
			lng = (op->opsize)(te, pos);
			break;
		}
	}

	for  (i = 1;  i < lng;  i++)  {
		t_entry	ctent;
		long	npos = pos+i+i;
		
		if  (npos >= endt)
			goto  clearem;
		gette(&mainfile, npos, &ctent);
#ifdef	COFF
		if (swbegflg) {
			ctent.t_type = T_UNKNOWN;
			putte(&mainfile, npos, &ctent);
		}
		else {
			if  (ctent.t_bdest || ctent.t_dref)  {
clearem:			for  (i--; i > 0; i--)  {
					npos = pos + i + i;
					gette(&mainfile, npos, &ctent);
					ctent.t_type = T_UNKNOWN;
					putte(&mainfile, npos, &ctent);
				}
				lng = 0;
				goto  ginv;
#else	/*  !COFF  */
		if  (ctent.t_bdest || ctent.t_dref)  {
clearem:		for  (i--; i > 0; i--)  {
				npos = pos + i + i;
				gette(&mainfile, npos, &ctent);
				ctent.t_type = T_UNKNOWN;
				putte(&mainfile, npos, &ctent);
#endif	/*  !COFF  */
			}
#ifdef	COFF
			ctent.t_type = T_CONT;
			putte(&mainfile, npos, &ctent);
#else	/*  !COFF  */
			lng = 0;
			goto  ginv;
#endif	/*  !COFF  */
		}
#ifndef	COFF
		ctent.t_type = T_CONT;
		putte(&mainfile, npos, &ctent);
#endif	/*  !COFF  */
	}
#ifdef	COFF
	swbegflg = 0;

#else	/*  !COFF  */
	
#endif	/*  !COFF  */
	if  (lng <= 0)  {
ginv:		te->t_vins = 0;
		te->t_lng = 1;
		te->t_type = T_UNKNOWN;
#ifdef	COFF
		te->t_bchtyp = T_NOBR;
#else	/*  !COFF  */
		te->t_bchtype = T_NOBR;
#endif	/*  !COFF  */
	}
	else
		te->t_lng = lng;
	return	lng;
}

/*
 *	Print instruction.
 */

void	prinst(te, pos)
t_entry	*te;
long	pos;
{
	putchar('\t');
	(optab[te->t_iindex].opprin)(te, pos);
	putchar('\n');
}
