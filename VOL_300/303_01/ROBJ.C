/*
 *	SCCS:	%W%	%G%	%U%
 *	Read object files.
 *
 *EMACS_MODES:c
 *
 *	This particular module will obviously have to be munged beyond
 *	recognition for another object format.
 */

#include <stdio.h>
#include <a.out.h>
#ifdef	COFF
#include <ldfcn.h>
#include <string.h>
#endif	/*  COFF  */
#include "unc.h"

void	gette(), getde(), setde(), putte(), putde();
long	gettw(), getdw();
void	reallst(), lclash(), nomem(), unimpl();
void	addit();
char	*malloc();
long	lseek();

int	par_entry, par_round, nmods, donedrel, donebrel;
struct	commit	abstab, comtab, dreltab;
long	trelpos, drelpos, brelpos;

#ifdef	COFF
int *symord;	/* convert symbol index to symbol ordinal */

#endif	/*  COFF  */
ef_fids	mainfile;

symbol	lookup(), inventsymb(), getnsymb();

#ifdef	COFF
#define RWORD 1
#define RLONG 2
#endif	/*  COFF  */
#define	DBSIZE	100
#define	STINIT	20

/*
 *	Read text segment.  Return 0 if not ok.
 */

#ifdef	COFF
int	rtext(ldptr, outf)
  LDFILE *ldptr;		/*  a.out file (possibly in library)  */
  ef_fid	outf;		/*  Output file descriptor  */
#else	/*  !COFF  */
int	rtext(inf, offset, outf)
int	inf;		/*  a.out file (possibly in library)  */
long	offset;		/*  Offset from start of inf of a.out file  */
ef_fid	outf;		/*  Output file descriptor  */
#endif	/*  !COFF  */
{
#ifdef	COFF
   t_entry		tstr;
   struct	aouthdr	unixhdr;
   struct  scnhdr  sect;
   register  long	size;
   register  int	i, l;
   unsigned  short	inbuf[DBSIZE/2];
   
   /*
    *	Initialise fields in structure.
    */
   
   tstr.t_type = T_UNKNOWN;
   tstr.t_vins = 1;		/*  For the moment  */
   tstr.t_bdest = 0;
   tstr.t_gbdest = 0;
   tstr.t_lng = 1;
   tstr.t_reloc = R_NONE;
   tstr.t_rdisp = 0;
   tstr.t_isrel = 0;
   tstr.t_amap = 0;
#else	/*  !COFF  */
	t_entry		tstr;
	struct	bhdr	filhdr;
	register  long	size;
	register  int	i, l;
	unsigned  short	inbuf[DBSIZE/2];

	/*
	 *	Initialise fields in structure.
	 */
	
	tstr.t_type = T_UNKNOWN;
	tstr.t_vins = 1;		/*  For the moment  */
	tstr.t_bdest = 0;
	tstr.t_gbdest = 0;
	tstr.t_lng = 1;
	tstr.t_reloc = R_NONE;
	tstr.t_rdisp = 0;
	tstr.t_isrel = 0;
	tstr.t_amap = 0;
#endif	/*  !COFF  */
	tstr.t_dref = 0;
	tstr.t_relsymb = NULL;
	tstr.t_reldisp = 0;
	tstr.t_lab = NULL;
	tstr.t_lsymb = 0;
	tstr.t_refhi = 0;
	tstr.t_reflo = 0x7fffffff;
	tstr.t_match = 0;
	
	/*
	 *	Read a.out header.
	 */
#ifndef	COFF
	
	(void) lseek(inf, offset, 0);
#endif	/*  !COFF  */

#ifdef	COFF
	if (ldohseek(ldptr) == FAILURE) {	/* no optional header */
#else	/*  !COFF  */
	if  (read(inf, (char *)&filhdr, sizeof(filhdr)) != sizeof(filhdr))
		return	0;
#endif	/*  !COFF  */

#ifdef	COFF
	    outf->ef_entry = 0;
	    ldshread(ldptr,1,&sect);		/* text header */
	    outf->ef_tbase = sect.s_vaddr;
	    outf->ef_tsize = sect.s_size;
#else	/*  !COFF  */
	if  (filhdr.fmagic != FMAGIC  &&  filhdr.fmagic != NMAGIC)
		return	0;
#endif	/*  !COFF  */

#ifdef	COFF
	    ldshread(ldptr,2,&sect);		/* data header */
	    outf->ef_dbase = sect.s_vaddr;
	    outf->ef_dsize = sect.s_size;

	    ldshread(ldptr,3,&sect);		/* bss header */
	    outf->ef_bbase = sect.s_vaddr;
	    outf->ef_bsize = sect.s_size;
	    outf->ef_end = sect.s_vaddr + sect.s_size;
	} else {
	    FREAD((char *)&unixhdr,sizeof(struct aouthdr),1,ldptr);
#else	/*  !COFF  */
	/*
	 *	Warn user if entry point does not tie up.
	 */
#endif	/*  !COFF  */
	
#ifdef	COFF
	    if ( N_BADMAG(unixhdr) )
		return	0;
#else	/*  !COFF  */
	if  (filhdr.entry != par_entry)
		(void) fprintf(stderr, "Warning: File has -R%X\n", filhdr.entry);
#endif	/*  !COFF  */

#ifdef	COFF
	    outf->ef_entry = unixhdr.entry;
	    outf->ef_tbase = unixhdr.text_start;
	    outf->ef_dbase = unixhdr.data_start;
	    outf->ef_bbase = outf->ef_dbase + unixhdr.dsize;
	    outf->ef_end = outf->ef_bbase + unixhdr.bsize;
#else	/*  !COFF  */
	outf->ef_entry = filhdr.entry;
	outf->ef_tbase = filhdr.entry;
	outf->ef_dbase = filhdr.tsize + filhdr.entry;
#endif	/*  !COFF  */

#ifdef	COFF
	    outf->ef_tsize = unixhdr.tsize;
	    outf->ef_dsize = unixhdr.dsize;
	    outf->ef_bsize = unixhdr.bsize;
	}
#else	/*  !COFF  */
	if  (filhdr.fmagic == NMAGIC)
		outf->ef_dbase = (outf->ef_dbase + par_round) & (~par_round);

	outf->ef_bbase = outf->ef_dbase + filhdr.dsize;
	outf->ef_end = outf->ef_bbase + filhdr.bsize;

	outf->ef_tsize = filhdr.tsize;
	outf->ef_dsize = filhdr.dsize;
	outf->ef_bsize = filhdr.bsize;
#endif	/*  !COFF  */
	
#ifdef	COFF
	ldsseek(ldptr,1);	/* seek to text section */
#else	/*  !COFF  */
	tstr.t_data = outf->ef_dbase;

	(void) lseek(inf, offset + TEXTPOS, 0);
#endif	/*  !COFF  */
	
	size = outf->ef_tsize;
	
	while  (size > 1)  {
		l = size > DBSIZE? DBSIZE: size;
#ifdef	COFF
		if  (FREAD((char *)inbuf,1,l,ldptr) != l)
#else	/*  !COFF  */
		if  (read(inf, (char *)inbuf, l) != l)
#endif	/*  !COFF  */
			return	0;
		l /= 2;
		for  (i = 0;  i < l;  i++)  {
			tstr.t_contents = inbuf[i];
			(void) write(outf->ef_t, (char *)&tstr, sizeof(tstr));
		}
		size -= l + l;
	}
	
	/*
	 *	Extra one to cope with "etext".
	 */
	
	(void) write(outf->ef_t, (char *)&tstr, sizeof(tstr));
	return	1;
}
#ifndef	COFF

#endif	/*  !COFF  */
/*
 *	Same sort of thing for the data segment.
 */

#ifdef	COFF
int	rdata(ldptr, outf)
LDFILE *ldptr;		/*  a.out file (possibly in library)  */
#else	/*  !COFF  */
int	rdata(inf, offset, outf)
int	inf;		/*  a.out file (possibly in library)  */
long	offset;		/*  Offset from start of inf of a.out file  */
#endif	/*  !COFF  */
ef_fid	outf;		/*  Output file descriptor  */
{
	d_entry		dstr;
#ifndef	COFF
	struct	bhdr	filhdr;
#endif	/*  !COFF  */
	register  long	size;
	register  int	i, l;
	unsigned  char	inbuf[DBSIZE];

	/*
	 *	Initialise fields in structure.
	 */
	
	dstr.d_type = D_BYTE;
	dstr.d_reloc = R_NONE;
	dstr.d_lng = 1;
	dstr.d_relsymb = NULL;
	dstr.d_reldisp = 0;
	dstr.d_lab = NULL;
	
#ifdef	COFF
	ldsseek(ldptr,2);	/* seek to data section */
#else	/*  !COFF  */
	/*
	 *	Read a.out header.
	 */
#endif	/*  !COFF  */
	
#ifndef	COFF
	(void) lseek(inf, offset, 0);

	if  (read(inf, (char *)&filhdr, sizeof(filhdr)) != sizeof(filhdr))
		return	0;

	(void) lseek(inf, offset + DATAPOS, 0);
	
#endif	/*  !COFF  */
	size = outf->ef_dsize;
	
	while  (size > 0)  {
		l = size > DBSIZE? DBSIZE: size;
#ifdef	COFF
		if  (FREAD((char *)inbuf,1,l,ldptr) != l)
#else	/*  !COFF  */
		if  (read(inf, (char *)inbuf, l) != l)
#endif	/*  !COFF  */
			return	0;
		for  (i = 0;  i < l;  i++)  {
			dstr.d_contents = inbuf[i];
			(void) write(outf->ef_d, (char *)&dstr, sizeof(dstr));
		}
		size -= l;
	}
	
	/*
	 *	Repeat for BSS segment.
	 */

	dstr.d_contents = 0;
	for  (size = outf->ef_bsize;  size > 0;  size--)
		(void) write(outf->ef_d, (char *)&dstr, sizeof(dstr));
	
	/*
	 *	Extra one to cope with "end".
	 */
	
	(void) write(outf->ef_d, (char *)&dstr, sizeof(dstr));
	return	1;
}

/*
 *	Process symbol table segment.
 */

#ifdef	COFF
int	rsymb(ldptr, dproc, outf)
LDFILE *ldptr;		/*  a.out file (possibly in library)  */
#else	/*  !COFF  */
int	rsymb(inf, offset, dproc, outf)
int	inf;		/*  a.out file (possibly in library)  */
long	offset;		/*  Offset from start of inf of a.out file  */
#endif	/*  !COFF  */
symbol	(*dproc)();
register  ef_fid  outf;	/*  Output file descriptor  */
{
#ifdef	COFF
#define SYMLENGTH 256
#endif	/*  COFF  */
	register  symbol  csym;
#ifdef	COFF
 	struct	syment	isym;
	register  int   nsyms,symindex;
	unsigned long   stroff;
 	char	inbuf[SYMLENGTH+1], *cp;
	int ord;
#else	/*  !COFF  */
	struct	bhdr	filhdr;
	struct	sym	isym;
	register  long	size;
	register  int	i, l;
	char	inbuf[SYMLENGTH+1];
#endif	/*  !COFF  */

#ifdef	COFF
	nsyms = HEADER(ldptr).f_nsyms;
	stroff = HEADER(ldptr).f_symptr + nsyms*sizeof(struct syment);
#else	/*  !COFF  */
	/*
	 *	Read a.out header.
	 */
	
	(void) lseek(inf, offset, 0);
#endif	/*  !COFF  */

#ifdef	COFF
	if  (nsyms <= 0)
		nsyms = STINIT;
#else	/*  !COFF  */
	if  (read(inf, (char *)&filhdr, sizeof(filhdr)) != sizeof(filhdr))
		return	0;
#endif	/*  !COFF  */

#ifdef	COFF
	outf->ef_stvec = (symbol *) malloc(nsyms * sizeof(symbol));
	symord = (int *) malloc(nsyms * sizeof(int));
#else	/*  !COFF  */
	offset += SYMPOS;
	size = filhdr.ssize;
	if  (size <= 0)
		return	1;

	/*
	 *	Guesstimate symbol table vector size.
	 */

	l = size / (sizeof(struct sym) + 4);
	if  (l <= 0)
		l = STINIT;

	outf->ef_stvec = (symbol *) malloc(l * sizeof(symbol));
#endif	/*  !COFF  */
	if  (outf->ef_stvec == NULL)
		nomem();

	outf->ef_stcnt = 0;
#ifdef	COFF
	outf->ef_stmax = nsyms;
	ord = 0;
#else	/*  !COFF  */
	outf->ef_stmax = l;
#endif	/*  !COFF  */
	
#ifdef	COFF
 	for  (symindex=0; symindex<nsyms; symindex++)  {
		ldtbread(ldptr,symindex,&isym);
		if (isym.n_zeroes == 0) {	/* get from string table */
		    FSEEK(ldptr,stroff + isym.n_offset,0);
		    cp = inbuf;
		    do {
 			if (FREAD(cp,1,1,ldptr) != 1)/* Read symbol chars 1-by-1 */
 				return 0;
 			if ( cp - inbuf >= SYMLENGTH )/* Check against buffer overflow */
 				return 0;
		    } while (*cp++ != '\0');/* Terminate on null byte */
		} else {			/* get from symbol field */
		    strncpy(inbuf,isym.n_name,8);
		    inbuf[8] = '\0';
		}
 		csym = (*dproc)(lookup(inbuf), convtosun(&isym),
				isym.n_value, outf);
 		if (outf->ef_stcnt >= outf->ef_stmax)
#else	/*  !COFF  */
	while  (size > sizeof(struct sym))  {
		(void) lseek(inf, offset, 0);
		if  (read(inf, (char *)&isym, sizeof(isym)) != sizeof(isym))
			return	0;
		size -= sizeof(isym);
		l = SYMLENGTH;
		if  (l > size)
			l = size;
		if  (read(inf, inbuf, l) != l)
			return	0;
		inbuf[l] = '\0';
		for  (i = 0; inbuf[i] != '\0';  i++)
			;
		size -= i + 1;
		offset += sizeof(isym) + i + 1;
		csym = (*dproc)(lookup(inbuf), isym.stype, isym.svalue, outf);
		if  (outf->ef_stcnt >= outf->ef_stmax)
#endif	/*  !COFF  */
			reallst(outf);
		outf->ef_stvec[outf->ef_stcnt++] = csym;
#ifdef	COFF
		symord[symindex] = ord++;		/* record ordinal */
		symindex += isym.n_numaux;		/* skip aux entries */
#endif	/*  COFF  */
	}
	return	1;
}

/*
 *	Process relocation stuff.  -1 error, 0 no relocation, 1 relocation.
 */

#ifdef	COFF
int	rrel(ldptr, ldptr2, outf)
LDFILE *ldptr,*ldptr2;	/*  a.out file (possibly in library)  */
#else	/*  !COFF  */
int	rrel(inf, offset, outf)
int	inf;		/*  a.out file (possibly in library)  */
long	offset;		/*  Offset from start of inf of a.out file  */
#endif	/*  !COFF  */
ef_fid	outf;		/*  Output file descriptor  */
{
#ifdef	COFF
 	struct	reloc	crel;
	struct scnhdr tsect,dsect;
	struct syment isym;
#else	/*  !COFF  */
	struct	bhdr	filhdr;
	struct	reloc	crel;
#endif	/*  !COFF  */
	t_entry	tstr;
	d_entry	dstr;
#ifdef	COFF
	register  int	nreloc;
#else	/*  !COFF  */
	register  long	size;
#endif	/*  !COFF  */
	long	cont, pos;

#ifdef	COFF
	ldshread(ldptr,1,&tsect);
	ldshread(ldptr,2,&dsect);
 	if  (tsect.s_nreloc <= 0  &&  dsect.s_nreloc <= 0)
#else	/*  !COFF  */
	/*
	 *	Read a.out header.
	 */
	
	(void) lseek(inf, offset, 0);

	if  (read(inf, (char *)&filhdr, sizeof(filhdr)) != sizeof(filhdr))
		return	-1;
	if  (filhdr.rtsize <= 0  &&  filhdr.rdsize <= 0)
#endif	/*  !COFF  */
		return	0;

#ifdef	COFF
	nreloc = tsect.s_nreloc;
#else	/*  !COFF  */
	size  =  filhdr.rtsize;
#endif	/*  !COFF  */

#ifdef	COFF
	ldrseek(ldptr,1);
 	while  (nreloc-- > 0)  {
		if  (FREAD((char *)&crel, sizeof(crel),1,ldptr) != 1)
#else	/*  !COFF  */
	(void) lseek(inf, RTEXTPOS + offset, 0);
	while  (size >= sizeof(struct reloc))  {
		if  (read(inf, (char *)&crel, sizeof(crel)) != sizeof(crel))
#endif	/*  !COFF  */
			return	-1;

#ifdef	COFF
 		pos = crel.r_vaddr;
#else	/*  !COFF  */
		pos = crel.rpos + outf->ef_tbase;
#endif	/*  !COFF  */
		gette(outf, pos, &tstr);
#ifdef	COFF
		if (crel.r_type == R_ABS)
		    tstr.t_reloc = R_NONE;
		else
		    tstr.t_reloc = R_LONG;	/* what about PC-relative? */
		ldtbread(ldptr2,crel.r_symndx,&isym);
		if (isym.n_sclass == C_EXT) {
 			tstr.t_relsymb = outf->ef_stvec[symord[crel.r_symndx]];
 			tstr.t_reldisp = gettw(outf, pos, (int)tstr.t_reloc);
#else	/*  !COFF  */
		tstr.t_reloc = crel.rsize + 1;	/*  Fiddle!  YUK!!!  */
		tstr.t_rdisp = crel.rdisp;
		tstr.t_rptr = crel.rsegment;
		if  (crel.rsegment == REXT)  {
			if  (crel.rsymbol >= outf->ef_stcnt)
				return  -1;
			tstr.t_relsymb = outf->ef_stvec[crel.rsymbol];
			tstr.t_reldisp = gettw(outf, pos, (int)crel.rsize+1);
#endif	/*  !COFF  */
		}
		else  {
#ifdef	COFF
 			cont = gettw(outf, pos, (int)tstr.t_reloc);
 			tstr.t_relsymb = getnsymb(outf, convtosun(&isym), cont);
#else	/*  !COFF  */
			cont = gettw(outf, pos, (int)crel.rsize+1);
			tstr.t_relsymb = getnsymb(outf, crel.rsegment, cont);
#endif	/*  !COFF  */
		}
		tstr.t_relsymb->s_used++;
		putte(outf, pos, &tstr);
#ifndef	COFF
		size -= sizeof(crel);
#endif	/*  !COFF  */
	}
	
	/*
	 *	And now repeat all that for data relocations.
	 */
	
#ifdef	COFF
	nreloc = dsect.s_nreloc;
#else	/*  !COFF  */
	size  =  filhdr.rdsize;
#endif	/*  !COFF  */
	
#ifdef	COFF
	ldrseek(ldptr,2);
 	while  (nreloc-- > 0)  {
		if (FREAD((char *)&crel, sizeof(crel),1,ldptr) != 1)
#else	/*  !COFF  */
	(void) lseek(inf, RDATAPOS + offset, 0);
	while  (size >= sizeof(struct reloc))  {
		if  (read(inf, (char *)&crel, sizeof(crel)) != sizeof(crel))
#endif	/*  !COFF  */
			return	-1;

#ifdef	COFF
 		pos = crel.r_vaddr;
#else	/*  !COFF  */
		pos = crel.rpos + outf->ef_dbase;
#endif	/*  !COFF  */
		getde(outf, pos, &dstr);
#ifdef	COFF
		if (crel.r_type == R_ABS)
		    dstr.d_reloc = R_NONE;
		else
		    dstr.d_reloc = R_LONG;	/* what about PC-relative? */
#else	/*  !COFF  */
		dstr.d_reloc = crel.rsize + 1;	/*  Fiddle!  YUK!!!  */
		dstr.d_rptr = crel.rsegment;
#endif	/*  !COFF  */

#ifdef	COFF
		ldtbread(ldptr2,crel.r_symndx,&isym);
		if (isym.n_sclass == C_EXT) {
 			dstr.d_relsymb = outf->ef_stvec[symord[crel.r_symndx]];
 			dstr.d_reldisp = getdw(outf, pos, (int)dstr.d_reloc);
#else	/*  !COFF  */
		if  (crel.rsegment == REXT)  {
			if  (crel.rsymbol >= outf->ef_stcnt)
				return  -1;
			dstr.d_relsymb = outf->ef_stvec[crel.rsymbol];
			dstr.d_reldisp = getdw(outf, pos, (int)crel.rsize+1);
#endif	/*  !COFF  */
		}
		else  {
#ifdef	COFF
 			cont = getdw(outf, pos, (int)dstr.d_reloc);
 			dstr.d_relsymb = getnsymb(outf, convtosun(&isym), cont);
 			if  (dstr.d_relsymb->s_type == S_TEXT)  {
#else	/*  !COFF  */
			cont = getdw(outf, pos, (int)crel.rsize+1);
			dstr.d_relsymb = getnsymb(outf, crel.rsegment, cont);
			if  (dstr.d_relsymb->s_type == TEXT)  {
#endif	/*  !COFF  */
				gette(outf, cont, &tstr);
				tstr.t_dref = 1;
				putte(outf, cont, &tstr);
			}
		}
#ifdef	COFF
 		switch  (dstr.d_reloc)  {
#else	/*  !COFF  */
		switch  (crel.rsize)  {
#endif	/*  !COFF  */
		default:
			unimpl("Data byte relocation");
			break;
#ifdef	COFF
		case  R_WORD:
#else	/*  !COFF  */
		case  RWORD:
#endif	/*  !COFF  */
			unimpl("data word reloc");
			dstr.d_type = D_WORD;
			dstr.d_lng = 2;
			setde(outf, pos+1, D_CONT, 1);
			break;
#ifdef	COFF
		case  R_LONG:
#else	/*  !COFF  */
		case  RLONG:
#endif	/*  !COFF  */
			dstr.d_type = D_ADDR;
			dstr.d_lng = 4;
			setde(outf, pos+1, D_CONT, 1);
			setde(outf, pos+2, D_CONT, 1);
			setde(outf, pos+3, D_CONT, 1);
			break;
		}
		dstr.d_relsymb->s_used++;
		putde(outf, pos, &dstr);
#ifndef	COFF
		size -= sizeof(crel);
#endif	/*  !COFF  */
	}
	return 1;
}

/*
 *	Process a symbol.
 */

symbol	dosymb(sy, type, val, fid)
register  symbol  sy;
int	type;
long	val;
ef_fid	fid;
{
	t_entry	tstr;
	d_entry	dstr;
	
	if  (!sy->s_newsym)  {
#ifdef	COFF
 		if  (type & S_EXT)  {
#else	/*  !COFF  */
		if  (type & EXTERN)  {
#endif	/*  !COFF  */
			(void) fprintf(stderr, "Duplicate symbol %s\n", sy->s_name);
#ifdef	COFF
			/* exit(10);  temporary? */
#else	/*  !COFF  */
			exit(10);
#endif	/*  !COFF  */
		}
		if  (++sy->s_defs > nmods)
			nmods = sy->s_defs;
		sy = inventsymb("DUP");
	}

	sy->s_value = val;
	
	switch  (type)  {
	default:
		return	NULL;
		
#ifdef	COFF
 	case  S_EXT|S_UNDF:
#else	/*  !COFF  */
	case  EXTERN|UNDEF:
#endif	/*  !COFF  */
		if  (val != 0)  {
#ifdef	COFF
 			sy->s_type = S_COMM;
#else	/*  !COFF  */
			sy->s_type = COMM;
#endif	/*  !COFF  */
			addit(&comtab, sy);
		}
		else
#ifdef	COFF
			sy->s_type = S_UNDF;
#else	/*  !COFF  */
			sy->s_type = N_UNDF;
#endif	/*  !COFF  */
		sy->s_glob = 1;
		break;
		
#ifdef	COFF
 	case  S_EXT|S_ABS:
		sy->s_type = S_ABS;
#else	/*  !COFF  */
	case  EXTERN|ABS:
		sy->s_type = N_ABS;
#endif	/*  !COFF  */
		sy->s_glob = 1;
		addit(&abstab, sy);
		break;
		
#ifdef	COFF
 	case  S_ABS:
		sy->s_type = S_ABS;
#else	/*  !COFF  */
	case  ABS:
		sy->s_type = N_ABS;
#endif	/*  !COFF  */
		addit(&abstab, sy);
		break;
		
#ifdef	COFF
 	case  S_EXT|S_TEXT:
 	case  S_TEXT:
		sy->s_type = S_TEXT;
#else	/*  !COFF  */
	case  EXTERN|TEXT:
	case  TEXT:
		sy->s_type = N_TEXT;
#endif	/*  !COFF  */
		gette(fid, val, &tstr);
		tstr.t_bdest = 1;
#ifdef	COFF
 		if  (type & S_EXT)  {
#else	/*  !COFF  */
		if  (type & EXTERN)  {
#endif	/*  !COFF  */
			tstr.t_gbdest = 1;
			sy->s_glob = 1;
		}
		sy->s_link = tstr.t_lab;
		tstr.t_lab = sy;
		putte(fid, val, &tstr);
		break;
		
#ifdef	COFF
 	case  S_BSS:
 	case  S_EXT|S_BSS:
		sy->s_type = S_BSS;
#else	/*  !COFF  */
	case  BSS:
	case  EXTERN|BSS:
		sy->s_type = N_BSS;
#endif	/*  !COFF  */
		goto	datrest;
#ifdef	COFF
 	case  S_DATA:
 	case  S_EXT|S_DATA:
		sy->s_type = S_DATA;
#else	/*  !COFF  */
	case  DATA:
	case  EXTERN|DATA:
		sy->s_type = N_DATA;
#endif	/*  !COFF  */
	datrest:
		getde(fid, val, &dstr);
#ifdef	COFF
 		if  (type & S_EXT)
#else	/*  !COFF  */
		if  (type & EXTERN)
#endif	/*  !COFF  */
			sy->s_glob = 1;
		sy->s_link = dstr.d_lab;
		dstr.d_lab = sy;
		putde(fid, val, &dstr);
		break;
	}
	
	sy->s_newsym = 0;
	return	sy;
}

#ifdef	COFF

#endif	/*  COFF  */
/*
 *	Process relocation stuff in putative library modules.
 *	The main function of all this is to mark which bits of the text
 *	not to look at as I compare the stuff.
 *
 *	As with "rrel", return -1 error, 0 no relocation, 1 relocation.
 */

#ifdef	COFF
int	rrell1(ldptr, outf)
LDFILE *ldptr;		/*  a.out file (possibly in library)  */
#else	/*  !COFF  */
int	rrell1(inf, offset, outf)
int	inf;		/*  a.out file (possibly in library)  */
long	offset;		/*  Offset from start of inf of a.out file  */
#endif	/*  !COFF  */
ef_fid	outf;		/*  Output file descriptor  */
{
#ifdef	COFF
 	struct	reloc	crel;
	struct scnhdr tsect,dsect;
#else	/*  !COFF  */
	struct	bhdr	filhdr;
	struct	reloc	crel;
#endif	/*  !COFF  */
	t_entry	tstr;
#ifdef	COFF
	register  int	nreloc;
#else	/*  !COFF  */
	register  long	size;
#endif	/*  !COFF  */
	long	pos;

#ifdef	COFF
	ldshread(ldptr,1,&tsect);
	ldshread(ldptr,2,&dsect);
 	if  (tsect.s_nreloc <= 0  &&  dsect.s_nreloc <= 0)
#else	/*  !COFF  */
	/*
	 *	Read a.out header.
	 */
	
	(void) lseek(inf, offset, 0);

	if  (read(inf, (char *)&filhdr, sizeof(filhdr)) != sizeof(filhdr))
		return	-1;
	if  (filhdr.rtsize <= 0  &&  filhdr.rdsize <= 0)
#endif	/*  !COFF  */
		return	0;

#ifdef	COFF
	nreloc = tsect.s_nreloc;
#else	/*  !COFF  */
	size  =  filhdr.rtsize;
#endif	/*  !COFF  */

#ifdef	COFF
	ldrseek(ldptr,1);
 	while  (nreloc-- > 0)  {
		if  (FREAD((char *)&crel, sizeof(crel),1,ldptr) != 1)
#else	/*  !COFF  */
	(void) lseek(inf, RTEXTPOS + offset, 0);
	while  (size >= sizeof(struct reloc))  {
		if  (read(inf, (char *)&crel, sizeof(crel)) != sizeof(crel))
#endif	/*  !COFF  */
			return	-1;

#ifdef	COFF
 		pos = crel.r_vaddr;
#else	/*  !COFF  */
		pos = crel.rpos + outf->ef_tbase;
#endif	/*  !COFF  */
		gette(outf, pos, &tstr);
#ifdef	COFF
		if (crel.r_type == R_ABS)
		    tstr.t_reloc = R_NONE;
		else
		    tstr.t_reloc = R_LONG;	/* what about PC-relative? */
#else	/*  !COFF  */
		tstr.t_reloc = crel.rsize + 1;	/*  Fiddle!  YUK!!!  */
		tstr.t_rdisp = crel.rdisp;
		tstr.t_rptr = crel.rsegment;
#endif	/*  !COFF  */
		tstr.t_isrel = 1;
		putte(outf, pos, &tstr);
#ifdef	COFF
 		if  (tstr.t_reloc == R_LONG)  {
#else	/*  !COFF  */
		if  (crel.rsize == RLONG)  {
#endif	/*  !COFF  */
			gette(outf, pos+2, &tstr);
			tstr.t_isrel = 1;
			putte(outf, pos+2, &tstr);
		}
#ifndef	COFF
		size -= sizeof(crel);
#endif	/*  !COFF  */
	}
	
	/*
	 *	Dont bother with data relocation at this stage. We'll
	 *	tie that up later.
	 */
	
	return 1;
}

/*
 *	Process a symbol in library file.  The extern variable trelpos gives
 *	the place in the main file where the library module is relocated.
 *	We don't know the data position until we do the final merge, perhaps
 *	not even then.
 */
#ifdef	COFF
/* trelpos ??? */
#endif	/*  COFF  */

symbol	dolsymb(sy, type, val, fid)
register  symbol  sy;
int	type;
long	val;
ef_fid	fid;
{
	t_entry	tstr;
	
	switch  (type)  {
	default:
		return	NULL;
		
#ifdef	COFF
 	case  S_EXT|S_UNDF:
#else	/*  !COFF  */
	case  EXTERN|UNDEF:
#endif	/*  !COFF  */
		if  (!sy->s_newsym)
			return	sy;
		sy->s_value = val;
		if  (val != 0)  {
#ifdef	COFF
 			sy->s_type = S_COMM;
#else	/*  !COFF  */
			sy->s_type = COMM;
#endif	/*  !COFF  */
			addit(&dreltab, sy);
		}
		else
#ifdef	COFF
			sy->s_type = S_UNDF;
#else	/*  !COFF  */
			sy->s_type = N_UNDF;
#endif	/*  !COFF  */
		sy->s_glob = 1;
		break;
		
#ifdef	COFF
 	case  S_EXT|S_ABS:
#else	/*  !COFF  */
	case  EXTERN|ABS:
#endif	/*  !COFF  */
		if  (!sy->s_newsym)  {
#ifdef	COFF
			if  (sy->s_type != S_ABS || sy->s_value != val)
#else	/*  !COFF  */
			if  (sy->s_type != N_ABS || sy->s_value != val)
#endif	/*  !COFF  */
				lclash("abs");
		}
#ifdef	COFF
		sy->s_type = S_ABS;
#else	/*  !COFF  */
		sy->s_type = N_ABS;
#endif	/*  !COFF  */
		sy->s_value = val;
		sy->s_glob = 1;
		addit(&abstab, sy);
		break;
		
#ifdef	COFF
 	case  S_EXT|S_TEXT:
		sy->s_type = S_TEXT;
#else	/*  !COFF  */
	case  EXTERN|TEXT:
		sy->s_type = N_TEXT;
#endif	/*  !COFF  */
		val += trelpos - fid->ef_tbase;
		if  (!sy->s_newsym)  {
			if  (val != sy->s_value)
				lclash("tsym");
			return	sy;
		}
		sy->s_value = val;
		gette(&mainfile, val, &tstr);
		tstr.t_bdest = 1;
		tstr.t_gbdest = 1;
		sy->s_glob = 1;
		sy->s_link = tstr.t_lab;
		tstr.t_lab = sy;
		putte(&mainfile, val, &tstr);
		break;

#ifdef	COFF
 	case  S_EXT|S_BSS:
#else	/*  !COFF  */
	case  EXTERN|BSS:
#endif	/*  !COFF  */
		if  (!sy->s_newsym)
			return	sy;
#ifdef	COFF
		sy->s_type = S_BSS;
#else	/*  !COFF  */
		sy->s_type = N_BSS;
#endif	/*  !COFF  */
		sy->s_value = val - fid->ef_bbase;
		goto	datrest;

#ifdef	COFF
 	case  S_EXT|S_DATA:
#else	/*  !COFF  */
	case  EXTERN|DATA:
#endif	/*  !COFF  */
		if  (!sy->s_newsym)
			return	sy;
#ifdef	COFF
		sy->s_type = S_DATA;
#else	/*  !COFF  */
		sy->s_type = N_DATA;
#endif	/*  !COFF  */
		sy->s_value = val - fid->ef_dbase;
	datrest:
		sy->s_glob = 1;
		addit(&dreltab, sy);
		break;
	}
	
	sy->s_newsym = 0;
	return	sy;
}

/*
 *	Change definition of undefined symbol as we define it.
 */

void	reassign(sy, val)
register  symbol  sy;
long	val;
{
	sy->s_value = val;

	if  (val < mainfile.ef_tbase)  {
#ifdef	COFF
		sy->s_type = S_ABS;
#else	/*  !COFF  */
		sy->s_type = N_ABS;
#endif	/*  !COFF  */
		addit(&abstab, sy);
	}
	else  if  (val < mainfile.ef_dbase)  {
		t_entry	tstr;
		
#ifdef	COFF
		sy->s_type = S_TEXT;
#else	/*  !COFF  */
		sy->s_type = N_TEXT;
#endif	/*  !COFF  */
		gette(&mainfile, val, &tstr);
		tstr.t_bdest = 1;
		tstr.t_gbdest = 1;
		sy->s_glob = 1;
		sy->s_link = tstr.t_lab;
		tstr.t_lab = sy;
		putte(&mainfile, val, &tstr);
	}
	else  {
		d_entry dstr;
		
#ifdef	COFF
		sy->s_type = val < mainfile.ef_bbase? S_DATA: S_BSS;
#else	/*  !COFF  */
		sy->s_type = val < mainfile.ef_bbase? N_DATA: N_BSS;
#endif	/*  !COFF  */
		getde(&mainfile, val, &dstr);
		sy->s_link = dstr.d_lab;
		dstr.d_lab = sy;
		putde(&mainfile, val, &dstr);
	}
}

/*
 *	When we discover where bss or data come, reallocate the table.
 */

void	zapdat(seg, inc)
int	seg;
long	inc;
{
	register  int	i;
	register  symbol  csymb;
	d_entry	dent;
	
	for  (i = 0;  i < dreltab.c_int;  i++) {
		csymb = dreltab.c_symb[i];
		if  (csymb->s_type != seg)
			continue;
		csymb->s_value += inc;
		getde(&mainfile, csymb->s_value, &dent);
		csymb->s_link = dent.d_lab;
		dent.d_lab = csymb;
		putde(&mainfile, csymb->s_value, &dent);
	}
}

/*
 *	Process relocation stuff in library module which we are inserting.
 *	Horrors if something goes wrong.
 */
#ifdef	COFF
/* trelpos, drelpos ??? */
#endif	/*  COFF  */

#ifdef	COFF
rrell2(ldptr, ldptr2, outf)
LDFILE *ldptr,*ldptr2;	/*  a.out file (possibly in library)  */
#else	/*  !COFF  */
void	rrell2(inf, offset, outf)
int	inf;		/*  a.out file (possibly in library)  */
long	offset;		/*  Offset from start of inf of a.out file  */
#endif	/*  !COFF  */
ef_fid	outf;		/*  Output file descriptor  */
{
#ifdef	COFF
 	struct	reloc	crel;
#else	/*  !COFF  */
	struct	bhdr	filhdr;
	struct	reloc	crel;
#endif	/*  !COFF  */
	t_entry	mtstr;
	d_entry	mdstr;
#ifdef	COFF
	struct scnhdr tsect,dsect;
	struct syment isym;
	int nreloc;
	unsigned rtype;
#endif	/*  COFF  */
	register  long	size;
	register  symbol  csymb;
	long	pos, mpos, mval, lval;
	int	dhere = 0;		/*  Mark whether bss done  */

#ifdef	COFF
	ldshread(ldptr,1,&tsect);
	ldshread(ldptr,2,&dsect);
 	if  (tsect.s_nreloc <= 0  &&  dsect.s_nreloc <= 0)
		return	0;
#else	/*  !COFF  */
	/*
	 *	Read a.out header.
	 */
	
	(void) lseek(inf, offset, 0);
#endif	/*  !COFF  */

#ifdef	COFF
	nreloc = tsect.s_nreloc;
#else	/*  !COFF  */
	if  (read(inf, (char *)&filhdr, sizeof(filhdr)) != sizeof(filhdr))
		return;
	if  (filhdr.rtsize <= 0  &&  filhdr.rdsize <= 0)
		return;
#endif	/*  !COFF  */

#ifdef	COFF
	ldrseek(ldptr,1);
 	while  (nreloc-- > 0)  {
		if  (FREAD((char *)&crel, sizeof(crel),1,ldptr) != 1)
#else	/*  !COFF  */
	size  =  filhdr.rtsize;

	(void) lseek(inf, RTEXTPOS + offset, 0);
	for  (;  size >= sizeof(struct reloc);  size -= sizeof(crel))  {
		if  (read(inf, (char *)&crel, sizeof(crel)) != sizeof(crel))
#endif	/*  !COFF  */
			lclash("rd trel");

#ifdef	COFF
 		pos = crel.r_vaddr;
 		mpos = crel.r_vaddr + trelpos;
#else	/*  !COFF  */
		pos = crel.rpos + outf->ef_tbase;
		mpos = crel.rpos + trelpos;
#endif	/*  !COFF  */
		gette(&mainfile, mpos, &mtstr);
#ifdef	COFF
		if (crel.r_type == R_ABS)
		    rtype = R_NONE;
		else
		    rtype = R_LONG;	/* what about PC-relative? */
		ldtbread(ldptr2,crel.r_symndx,&isym);
 		lval = gettw(outf, pos, (int)rtype);
 		mval = gettw(&mainfile, mpos, (int)rtype);
#else	/*  !COFF  */
		lval = gettw(outf, pos, (int)crel.rsize+1);
		mval = gettw(&mainfile, mpos, (int)crel.rsize+1);
#endif	/*  !COFF  */
		
#ifdef	COFF
		if ( isym.n_sclass != C_EXT ) {
		switch (convtosun(&isym)) {
 		case  S_TEXT:
#else	/*  !COFF  */
		switch  (crel.rsegment)  {
		case  RTEXT:
#endif	/*  !COFF  */
			if  (lval + trelpos - outf->ef_tbase != mval)
				lclash("Trel");
			continue;
#ifdef	COFF
 		case  S_DATA:
#else	/*  !COFF  */
		case  RDATA:
#endif	/*  !COFF  */
			if  (donedrel)  {
				if  (lval + drelpos - outf->ef_dbase != mval)
					lclash("Drel");
			}
			else  {
				donedrel++;
				drelpos = mval - lval + outf->ef_dbase;
			}
			continue;
#ifdef	COFF
 		case  S_BSS:
#else	/*  !COFF  */
		case  RBSS:
#endif	/*  !COFF  */
			if  (donebrel)  {
				if  (lval + brelpos - outf->ef_bbase != mval)
					lclash("brel");
			}
			else  {
				donebrel++;
				brelpos = mval - lval + outf->ef_bbase;
			}
			continue;
#ifdef	COFF
 		      }
 	      } else {
 			if  (crel.r_symndx >= outf->ef_stcnt)
#else	/*  !COFF  */
		case  REXT:
			if  (crel.rsymbol >= outf->ef_stcnt)
#endif	/*  !COFF  */
				lclash("Bad sy no");
#ifdef	COFF
 			csymb = outf->ef_stvec[symord[crel.r_symndx]];
#else	/*  !COFF  */
			csymb = outf->ef_stvec[crel.rsymbol];
#endif	/*  !COFF  */
			if  (csymb == NULL)
				continue;
			switch  (csymb->s_type)  {
#ifdef	COFF
			case  S_UNDF:
#else	/*  !COFF  */
			case  N_UNDF:
#endif	/*  !COFF  */
				reassign(csymb, mval - lval);
				break;
#ifdef	COFF
			case  S_ABS:
#else	/*  !COFF  */
			case  N_ABS:
#endif	/*  !COFF  */
				if  (lval + csymb->s_value != mval)
					lclash("abs rel");
				break;
#ifdef	COFF
			case  S_TEXT:
#else	/*  !COFF  */
			case  N_TEXT:
#endif	/*  !COFF  */
				if  (lval + csymb->s_value != mval)
					lclash("text rel");
				break;
#ifdef	COFF
			case  S_DATA:
#else	/*  !COFF  */
			case  N_DATA:
#endif	/*  !COFF  */
				if  (lval + csymb->s_value != mval)
					lclash("data rel");
				break;
#ifdef	COFF
			case  S_BSS:
#else	/*  !COFF  */
			case  N_BSS:
#endif	/*  !COFF  */
				if  (lval + csymb->s_value != mval)
					lclash("bss rel");
				break;
#ifdef	COFF
 			case  S_COMM:
#else	/*  !COFF  */
			case  COMM:
#endif	/*  !COFF  */
				reassign(csymb, mval - lval);
				break;
			}
			mtstr.t_relsymb = csymb;
			mtstr.t_reldisp = lval;
#ifndef	COFF
			break;
#endif	/*  !COFF  */
		}
	}
	
	/*
	 *	Relocate data and bss if possible.
	 */
	
	if  (donebrel)  {
#ifdef	COFF
		zapdat(S_BSS, brelpos);
#else	/*  !COFF  */
		zapdat(N_BSS, brelpos);
#endif	/*  !COFF  */
		dhere++;
	}
	
	if  (!donedrel)
		return;
		

#ifdef	COFF
	zapdat(S_DATA, drelpos);
#else	/*  !COFF  */
	zapdat(N_DATA, drelpos);
#endif	/*  !COFF  */
	
	/*
	 *	And now repeat all that for data relocations if possible
	 */
	
#ifdef	COFF
	nreloc = tsect.s_nreloc;

	ldrseek(ldptr,2);
#else	/*  !COFF  */
	size  =  filhdr.rdsize;
#endif	/*  !COFF  */
	
#ifdef	COFF
	while (nreloc-- > 0) {
		if  (FREAD((char *)&crel, sizeof(crel),1,ldptr) != 1)
#else	/*  !COFF  */
	(void) lseek(inf, RDATAPOS + offset, 0);
	for  (;  size >= sizeof(struct reloc); size -= sizeof(crel))  {
		if  (read(inf, (char *)&crel, sizeof(crel)) != sizeof(crel))
#endif	/*  !COFF  */
			lclash("Rd drel");

#ifdef	COFF
 		if  (crel.r_type == R_ABS)
#else	/*  !COFF  */
		if  (crel.rsize != RLONG)
#endif	/*  !COFF  */
			continue;

#ifdef	COFF
 		pos = crel.r_vaddr;
 		mpos = crel.r_vaddr + drelpos;
#else	/*  !COFF  */
		pos = crel.rpos + outf->ef_dbase;
		mpos = crel.rpos + drelpos;
#endif	/*  !COFF  */
		getde(&mainfile, mpos, &mdstr);
#ifdef	COFF
		rtype = R_LONG;		/* what about PC-relative? */
		ldtbread(ldptr2,crel.r_symndx,&isym);

 		lval = getdw(outf, pos, (int)rtype);
 		mval = getdw(&mainfile, mpos, (int)rtype);
		if ( isym.n_sclass != C_EXT ) {
		switch (convtosun(&isym)) {
 		case  S_TEXT:
#else	/*  !COFF  */
		lval = getdw(outf, pos, (int)crel.rsize+1);
		mval = getdw(&mainfile, mpos, (int)crel.rsize+1);
		switch  (crel.rsegment)  {
		case  RTEXT:
#endif	/*  !COFF  */
			if  (lval + trelpos - outf->ef_tbase != mval)
				lclash("Trel-d");
			continue;
#ifdef	COFF
 		case  S_DATA:
#else	/*  !COFF  */
		case  RDATA:
#endif	/*  !COFF  */
			if  (lval + drelpos - outf->ef_dbase != mval)
				lclash("Drel-d");
			continue;
#ifdef	COFF
 		case  S_BSS:
#else	/*  !COFF  */
		case  RBSS:
#endif	/*  !COFF  */
			if  (donebrel)  {
				if  (lval + brelpos - outf->ef_bbase != mval)
					lclash("brel");
			}
			else  {
				donebrel++;
				brelpos = mval - lval + outf->ef_bbase;
			}
			continue;
#ifdef	COFF
 		      }
 	      } else { 
 			if  (crel.r_symndx >= outf->ef_stcnt)
#else	/*  !COFF  */
		case  REXT:
			if  (crel.rsymbol >= outf->ef_stcnt)
#endif	/*  !COFF  */
				lclash("Bad sy no");
#ifdef	COFF
 			csymb = outf->ef_stvec[symord[crel.r_symndx]];
#else	/*  !COFF  */
			csymb = outf->ef_stvec[crel.rsymbol];
#endif	/*  !COFF  */
			if  (csymb == NULL)
				continue;
			switch  (csymb->s_type)  {
#ifdef	COFF
			case  S_UNDF:
#else	/*  !COFF  */
			case  N_UNDF:
#endif	/*  !COFF  */
				reassign(csymb, mval - lval);
				break;
#ifdef	COFF
			case  S_ABS:
#else	/*  !COFF  */
			case  N_ABS:
#endif	/*  !COFF  */
				if  (lval + csymb->s_value != mval)
					lclash("abs rel");
				break;
#ifdef	COFF
			case  S_TEXT:
#else	/*  !COFF  */
			case  N_TEXT:
#endif	/*  !COFF  */
				if  (lval + csymb->s_value != mval)
					lclash("text rel");
				break;
#ifdef	COFF
			case  S_DATA:
#else	/*  !COFF  */
			case  N_DATA:
#endif	/*  !COFF  */
				if  (lval + csymb->s_value != mval)
					lclash("data rel");
				break;
#ifdef	COFF
			case  S_BSS:
#else	/*  !COFF  */
			case  N_BSS:
#endif	/*  !COFF  */
				if  (lval + csymb->s_value != mval)
					lclash("bss rel");
				break;
#ifdef	COFF
 			case  S_COMM:
#else	/*  !COFF  */
			case  COMM:
#endif	/*  !COFF  */
				reassign(csymb, mval - lval);
				break;
			}
			mtstr.t_relsymb = csymb;
			mtstr.t_reldisp = lval;
#ifndef	COFF
			break;
#endif	/*  !COFF  */
		}
	}

	if  (dhere || !donebrel)
		return;

#ifdef	COFF
	zapdat(S_BSS, brelpos);
#else	/*  !COFF  */
	zapdat(N_BSS, brelpos);
#endif	/*  !COFF  */
}
