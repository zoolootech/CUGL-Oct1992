/*
 *	SCCS:	%W%	%G%	%U%
 *	Allocate space etc.
 *
 *EMACS_MODES:c
 */

#include <stdio.h>
#include <a.out.h>
#include <setjmp.h>
#ifdef	COFF
#include <ldfcn.h>
#endif	/*  COFF  */
#include "unc.h"

#define	STINC	10

char	*malloc(), *realloc();
char	*strncpy();
void	gette(), getde(), setde(), putte(), putde();
void	unimpl();
long	gettw();

ef_fids	mainfile;

/*
 *	Oops! out of memory.....
 */

void	nomem()
{
	(void) fprintf(stderr, "Sorry - run out of memory\n");
	exit(255);
}

/*
 *	Look up hash value of symbol.
 */

unsigned  shash(str)
register  char	*str;
{
	register  unsigned  result = 0;
	register  int	cnt = 0;
	
	while  (*str  &&  cnt < MAXCHARS)  {
		result += *str++;
		cnt++;
	}
	return  result % HASHMOD;
}

/*
 *	Look up hash value of symbol, possibly allocating a new symbol.
 */

symbol	lookup(str)
char	*str;
{
	register  symbol  res, *pp;
	register  int	len;
	
	pp = &symbhash[shash(str)];
	res = *pp;
	while  (res != NULL)  {
		if  (strncmp(res->s_name, str, MAXCHARS) == 0)
			return	res;
		pp = &res->s_next;
		res = *pp;
	}
	for  (len = 0;  len < MAXCHARS;  len++)
		if  (str[len] == '\0')
			break;
	len++;
	res = (symbol) malloc(sizeof(struct symstr) + len);
	if  (res == NULL)
		nomem();
	*pp = res;
	res->s_next = NULL;
	(void) strncpy(res->s_name, str, len);
	res->s_name[len] = '\0';		/*  Null-terminate  */
	res->s_newsym = 1;
	res->s_glob = 0;
	res->s_invent = 0;
	res->s_link = NULL;
	res->s_used = 0;
	res->s_defs = 0;
	res->s_lsymb = 0;
	return  res;
}

/*
 *	Invent a symbol, making sure that we don't know it.
 */

symbol	inventsymb(prefix)
char	*prefix;
{
	static	int  nsymb = 0;
	char	schars[10];
	register  symbol  res;
	
	do	(void) sprintf(schars, "%s%d", prefix, ++nsymb);
	while  (!(res = lookup(schars))->s_newsym);
	
	res->s_newsym = 0;
	res->s_invent = 1;
	return	res;
}
	 
/*
 *	Reallocate symbol table.
 */

void	reallst(outf)
register  ef_fid  outf;
{
	outf->ef_stmax += STINC;
	if  (outf->ef_stvec == NULL)
		outf->ef_stvec = (symbol *) malloc(outf->ef_stmax * sizeof(symbol));
	else
		outf->ef_stvec = (symbol *) realloc(outf->ef_stvec,
					outf->ef_stmax * sizeof(symbol));
	if  (outf->ef_stvec == NULL)
		nomem();
}

/*
 *	Search through existing symbol table for symbol with given
 *	value.  Invent a new one if needed.
 */

symbol	getnsymb(fid, seg, pos)
register  ef_fid  fid;
unsigned  seg;
long	pos;
{
	register  int	i;
	register  symbol  res;
	
	/***********  MACHINE DEPENDENT  ******************************
	 *	Convert relocation segment type (argument) to symbol type
	 *	(as remembered in symbol table).  Don't ask me why they
	 *	have to be different.....
	 */
	
#ifndef	COFF
	seg += TEXT - RTEXT;
	
#endif	/*  !COFF  */
	/*
	 *	See if the reference is to an external symbol.
	 *	If so, use that.
	 */
	
	for  (i = 0;  i < fid->ef_stcnt;  i++)  {
		res = fid->ef_stvec[i];
		if  (res->s_type == seg  &&  res->s_value == pos)
			return	res;
	}
	
	/*
	 *	Invent a symbol and use that.
	 */
	
	res = inventsymb("RS");
	if  (fid->ef_stcnt >= fid->ef_stmax)
		reallst(fid);
	fid->ef_stvec[fid->ef_stcnt++] = res;
	res->s_type = seg;
	res->s_value = pos;
#ifdef	COFF
	if  (seg == S_TEXT)  {
#else	/*  !COFF  */
	if  (seg == TEXT)  {
#endif	/*  !COFF  */
		t_entry	tent;
		gette(fid, pos, &tent);
		tent.t_bdest = 1;
		tent.t_lab = res;
		putte(fid, pos, &tent);
	}
#ifdef	COFF
	else  if  (seg == S_DATA  ||  seg == S_BSS)  {
#else	/*  !COFF  */
	else  if  (seg == DATA  ||  seg == BSS)  {
#endif	/*  !COFF  */
		d_entry dent;
		getde(fid, pos, &dent);
		dent.d_lab = res;
		putde(fid, pos, &dent);
	}

	return	res;
}

/*
 *	Assuming address given is in text segment, find its label, or invent
 *	one.  Also set where refered from.
 */

symbol	textlab(loc, refpos)
long	loc, refpos;
{
	t_entry	tent;

	gette(&mainfile, loc, &tent);
	if  (tent.t_type == T_CONT)
		return	NULL;
	if  (tent.t_lab == NULL)  {
		tent.t_lab = inventsymb("TS");
#ifdef	COFF
		tent.t_lab->s_type = S_TEXT;
#else	/*  !COFF  */
		tent.t_lab->s_type = TEXT;
#endif	/*  !COFF  */
		tent.t_lab->s_value = loc;
		tent.t_bdest = 1;
		putte(&mainfile, loc, &tent);
	}
	else
		tent.t_lab->s_used++;
	if  (tent.t_refhi < refpos)  {
		tent.t_refhi = refpos;
		putte(&mainfile, loc, &tent);
	}
	if  (tent.t_reflo > refpos)  {
		tent.t_reflo = refpos;
		putte(&mainfile, loc, &tent);
	}
	return	tent.t_lab;
}

/*
 *	Note references to data.
 */

void	mkdref(tpos, size)
long	tpos;
unsigned  size;
{
	t_entry	tent;
	d_entry	dent;
	register  symbol  ds;
	int	dchng = 0;
	int	wsize;
	long	dpos;
	
	gette(&mainfile, tpos, &tent);
	if  (tent.t_relsymb != NULL)
		return;
		
	dpos = gettw(&mainfile, tpos, R_LONG);
	if  (dpos < mainfile.ef_dbase  ||  dpos > mainfile.ef_end)
		return;
	
	switch  (size)  {
	default:
		wsize = D_BYTE;
		break;
	case  2:
		wsize = D_WORD;
		break;
	case  4:
		wsize = D_LONG;
		break;
	}
	
	getde(&mainfile, dpos, &dent);
	if  ((ds = dent.d_lab) == NULL)  {
		if  (dpos >= mainfile.ef_bbase)  {
			ds = inventsymb("BS");
#ifdef	COFF
			ds->s_type = S_BSS;
#else	/*  !COFF  */
			ds->s_type = BSS;
#endif	/*  !COFF  */
		}
		else  {
			ds = inventsymb("DS");
#ifdef	COFF
			ds->s_type = S_DATA;
#else	/*  !COFF  */
			ds->s_type = DATA;
#endif	/*  !COFF  */
		}
		ds->s_value = dpos;
		dent.d_lab = ds;
		dchng++;
	}
	else
		ds->s_used++;

	if  (dent.d_type != D_BYTE)  {
		if  (dent.d_type != wsize)  {
			if  (dent.d_type == D_ADDR)  {
				if  (wsize != D_LONG)
					unimpl("Addr word usage");
			}
			else  if  (dent.d_type > wsize)  {
				dchng++;
				dent.d_type = wsize;
				dent.d_lng = size;
			}
		}
	}
	else  {
		dent.d_type = wsize;
		dent.d_lng = size;
		dchng++;
	}
	if  (dchng)  {
		putde(&mainfile, dpos, &dent);
		for  (dchng = 1;  dchng < size; dchng++)
			setde(&mainfile, dpos+dchng, D_CONT, 1);
	}
		
	tent.t_relsymb = ds;
	putte(&mainfile, tpos, &tent);
}

/*
 *	Add item to common or abs list.
 */

#define	COMINC	10

void	addit(cp, symb)
register  struct  commit  *cp;
symbol	symb;
{
	if  (cp->c_int >= cp->c_max)  {
		cp->c_max += COMINC;
		if  (cp->c_symb == NULL)
			cp->c_symb = (symbol *) malloc(COMINC*sizeof(symbol));
		else
			cp->c_symb = (symbol *)
					realloc(cp->c_symb,
						cp->c_max * sizeof(symbol));
		if  (cp->c_symb == NULL)
			nomem();
	}
	cp->c_symb[cp->c_int++] = symb;
}
