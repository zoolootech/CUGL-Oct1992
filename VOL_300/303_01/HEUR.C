/*
 *	SCCS:	%W%	%G%	%U%
 *	Attempt to guess things about the file.
 *
 *EMACS_MODES:c
 */

#include <stdio.h>
#include <a.out.h>
#ifdef	COFF
#include <ldfcn.h>
#endif	/*  COFF  */
#include "unc.h"

#define	INITDAT	256
#define	INCDAT	128

#define	STRSCNT	3
#define	STRECNT	3

char	*malloc(), *realloc();

void	gette(), getde(), setde(), putte(), putde();
void	nomem();
long	getdw();
symbol	inventsymb();

long	endt;
ef_fids	mainfile;

/*
 *	Talk about implemented things.....
 */

void	unimpl(msg)
char	*msg;
{
	(void) fprintf(stderr, "Warning: handling of \"%s\" not implemented\n", msg);
}

/*
 *	Return 1 if string char, otherwise 0.
 */

int	possstr(x)
unsigned  x;
{
	if  (x >= ' '  &&  x <= '~')
		return	1;
	if  (x == '\n'  ||  x == '\t')
		return	1;
	return	0;
}

/*
 *	Guess things about data files.
 */

void	intudat(fid)
ef_fid  fid;
{
	register  int	i, j;
	int	lt, input, invcnt;
	long	offs, soffs, endd;
	d_entry	fdat;
	unsigned  char	*inbuf;
	int	ibsize;
	
	inbuf = (unsigned  char *)malloc(INITDAT);
	if  (inbuf == NULL)
		nomem();
	ibsize = INITDAT;
	
	offs = fid->ef_dbase;
	endd = fid->ef_bbase;

	while  (offs < endd)  {
		getde(fid, offs, &fdat);
		if  (fdat.d_type != D_BYTE)  {
			offs += fdat.d_lng;
			continue;
		}
		
		/*
		 *	Looks like general data.  Read in as much as possible.
		 */
		
		input = 0;
		soffs = offs;
		do  {
			if  (input >= ibsize)  {
				ibsize += INCDAT;
				inbuf = (unsigned  char *)
					realloc((char *)inbuf, (unsigned)ibsize);
				if  (inbuf == NULL)
					nomem();
			}
			inbuf[input++] = fdat.d_contents;
			offs++;
			if  (offs >= endd)
				break;
			getde(fid, offs, &fdat);
		}  while  (fdat.d_type == D_BYTE && fdat.d_lab == NULL);
		
		/*
		 *	Now split up the data.
		 */
		
		for  (i = 0;  i < input;  )  {
			
			/*
			 *	Might be a string.
			 */
			
			if  (possstr(inbuf[i]))  {
				lt = input;
				if  (i + STRSCNT < lt)
					lt = i + STRSCNT;
				for  (j = i + 1;  j < lt;  j++)  {
					if  (inbuf[j] == '\0')
						break;
					if  (!possstr(inbuf[j]))
						goto  notstr;
				}
				
				/*
				 *	Looks like a string then.
				 */
				
				invcnt = 0;
				for  (j = i + 1; j < input;  j++)  {
					if  (inbuf[j] == '\0')  {
						j++;
						break;
					}
					if  (possstr(inbuf[j]))
						invcnt = 0;
					else  {
						invcnt++;
						if  (invcnt >= STRECNT)  {
							j -= invcnt - 1;
							break;
						}
					}
				}

				setde(fid,
				      soffs+i,
				      (unsigned)(inbuf[j-1]=='\0'?D_ASCZ:D_ASC),
				      j - i);
				for  (i++;  i < j;  i++)
					setde(fid, soffs+i, D_CONT, 1); 
				continue;
			}

notstr:
			/*
			 *	If on odd boundary, treat as a byte.
			 */
			
			if  ((soffs + i) & 1  ||  i + 1 >= input)  {
				setde(fid, soffs + i, D_BYTE, 1);
				i++;
				continue;
			}

			/*
			 *	Treat as longs unless not enough.
			 */
			
			if  (i + 3 >= input)  {
				setde(fid, soffs + i, D_WORD, 2);
				setde(fid, soffs + i + 1, D_CONT, -1);
				i += 2;
				continue;
			}

			/*
			 *	Treat as a long but mark changable.
			 */
			
			setde(fid, soffs + i, D_LONG, 4);
			for  (j = 1;  j < 4;  j++)
				setde(fid, soffs + i + j, D_CONT, -j);
			i += 4;
		}
	}
	free((char *)inbuf);
	
	/*
	 *	Now zap bss segment.
	 */
	
	offs = fid->ef_bbase;
	endd = fid->ef_end;

	while  (offs < endd)  {
		getde(fid, offs, &fdat);
		if  (fdat.d_type != D_BYTE)  {
			offs += fdat.d_lng;
			continue;
		}

		soffs = offs;
		do  {
			offs++;
			if  (offs >= endd)
				break;
			getde(fid, offs, &fdat);
		}  while  (fdat.d_type == D_BYTE && fdat.d_lab == NULL);
		
		setde(fid, soffs, D_BYTE, (int)(offs-soffs));
		for  (i = -1, soffs++;  soffs < offs; i--, soffs++)
			setde(fid, soffs, D_CONT, i); 
	}
}

/*
 *	For non relocatable files, try to identify address pointers in
 *	the data.
 */

void	inturdat(fid)
ef_fid	fid;
{
	register  long	offs = fid->ef_dbase;
	register  int	i;
	register  symbol  ds;
	long  endd = fid->ef_bbase;
	long  cont;
	d_entry	dent, refdent;

	while  (offs < endd)  {
		getde(fid, offs, &dent);
		if  (dent.d_type != D_LONG)
			goto  endit;
		cont = getdw(fid, offs, R_LONG);
		if  (cont < fid->ef_dbase || cont > fid->ef_end)
			goto  endit;
		getde(fid, cont, &refdent);
		if  (refdent.d_type == D_CONT)  {
			d_entry	pdent;
			int	siz;
			
			if  (refdent.d_lng >= 0)
				goto  endit;
			getde(fid, cont+refdent.d_lng, &pdent);
			i = -refdent.d_lng;
			refdent.d_lng += pdent.d_lng;
			pdent.d_lng = i;
			if  (pdent.d_type == D_LONG  &&  i == 2)
				siz = D_WORD;
			else
				siz = D_BYTE;
			refdent.d_type = siz;
			pdent.d_type = siz;
			putde(fid, cont - i, &pdent);
			for  (i = 1;  i < refdent.d_lng;  i++)
				setde(fid, cont+i, D_CONT, -i);
		}
		if  ((ds = refdent.d_lab) == NULL)  {
			if  (cont >= fid->ef_bbase)  {
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
			ds->s_value = cont;
			refdent.d_lab = ds;
			putde(fid, cont, &refdent);
		}
		else
			ds->s_used++;
		dent.d_type = D_ADDR;
		dent.d_relsymb = ds;
		dent.d_rptr = ds->s_type;
		putde(fid, offs, &dent);
		for  (i = 1;  i < 4;  i++)
			setde(fid, offs+i, D_CONT, 1);
endit:
		offs += dent.d_lng;
	}
}

/*
 *	Recursively follow through the code, stopping at unconditional
 *	branches and invalid instructions.
 */

void	follseq(pos)
long	pos;
{
	t_entry	tent;
	int	lng;
	long	npos;

	while  (pos < endt)  {
		gette(&mainfile, pos, &tent);
		if  (tent.t_amap)	/*  Been here  */
			return;
		tent.t_amap = 1;
		lng = findinst(&tent, pos);
		npos = pos + lng*2;
		if  (npos > endt)  {
			tent.t_vins = 0;
			tent.t_lng = 1;
			tent.t_type = T_UNKNOWN;
			lng = 0;
			npos = endt;
		}
		putte(&mainfile, pos, &tent);
		pos = npos;
		
		if  (lng <= 0)
			return;

		switch  (tent.t_bchtyp)  {
		case  T_UNBR:
			if  (tent.t_relsymb == NULL)
				return;
			pos = tent.t_relsymb->s_value;
			continue;
		case  T_JSR:
			if  (tent.t_relsymb != NULL)
				follseq(tent.t_relsymb->s_value);
			continue;
		case  T_CONDBR:
			follseq(tent.t_relsymb->s_value);
		default:
			continue;
		}
	}
}
			
			
/*
 *	Try to work out things about text files.
 */

void	intutext()
{
	long	pos;
	t_entry	tent;
	int	lng;
	
	endt = mainfile.ef_tbase + mainfile.ef_tsize;
	pos = mainfile.ef_entry;
nextv:
	for  (;  pos < endt;)  {
		gette(&mainfile, pos, &tent);
		if  (!tent.t_amap && tent.t_vins)  {
			follseq(pos);
			pos += 2;
			goto  nextiv;
		}
		pos += tent.t_lng * 2;
		if  (tent.t_bchtyp == T_UNBR)
			goto  nextiv;
	}
	goto	dorest;
nextiv:
	for  (;  pos < endt;  pos += 2)  {
		gette(&mainfile, pos, &tent);
		if  (tent.t_bdest)
			goto  nextv;
	}
dorest:
	/*
	 *	Deal with unmapped instructions.
	 */
	
#ifdef	COFF
 	for  (pos = mainfile.ef_tbase;  pos < endt;)  {
#else	/*  !COFF  */
	for  (pos = 0;  pos < endt;)  {
	for  (pos = mainfile.ef_entry;  pos < endt;)  {
#endif	/*  !COFF  */
		gette(&mainfile, pos, &tent);
		switch  (tent.t_type)  {
		case  T_BEGIN:
			pos += tent.t_lng * 2;
			continue;
		case  T_UNKNOWN:
			if  (tent.t_vins)  {
				lng = findinst(&tent, pos);
				putte(&mainfile, pos, &tent);
				if  (lng > 0)  {
					pos += lng * 2;
					continue;
				}
			}
		default:
			pos += 2;
			continue;
		}
	}
}

/*
 *	Invent local symbols.
 */

void	intlsym()
{
	long	bpos, epos, hiref, hipos;
	unsigned  llnum;
	t_entry	tent;
	register  symbol  tl;
	
	endt = mainfile.ef_tbase + mainfile.ef_tsize;
	epos = mainfile.ef_entry;
	for  (;;)  {
		bpos = epos;
		hiref = bpos;
		if  (epos >= endt)
			return;
		gette(&mainfile, epos, &tent);
		epos += tent.t_lng * 2;
		for  (;  epos < endt;)  {
			gette(&mainfile, epos, &tent);
			if  (tent.t_gbdest  ||  tent.t_dref)
				break;
			if  (tent.t_reflo < bpos)
				break;
			if  (tent.t_refhi > hiref)  {
				hiref = tent.t_refhi;
				hipos = epos;
			}
			epos += tent.t_lng * 2;
		}
		if  (hiref > epos)
			epos = hipos;
		llnum = 0;
		for  (hipos = bpos;  hipos < epos;)  {
			gette(&mainfile, hipos, &tent);
			if  (!tent.t_gbdest && !tent.t_dref &&
			 tent.t_reflo >= bpos && tent.t_refhi < epos &&
			 (tl = tent.t_lab) != NULL)
				tl->s_lsymb = ++llnum;
			hipos += tent.t_lng * 2;
		}
	}
}

/*
 *	Given the main file, and a possible candidate for matching in the
 *	file, see where we can get.  Return text offset where match occured
 *	or -1 if no match.
 *	Given the main file, a possible candidate for matching in the
 *	file and an offset, see if text matches.  Return 1 if matches,
 *	or 0 if no match.
 */

#define	BFSIZ	30		/*  Buffer size  */

long	matchup(mf, lf, startpos)
ef_fid	mf, lf;
int	matchup(mf, lf, startpos)
register  ef_fid  mf, lf;
long	startpos;
{
	register  int	i, lbcnt;
	unsigned  short	 lbufw[BFSIZ], startw;
	register  int	i, matches = 0;
	t_entry	ltent, mtent;
	long	lfoffs, lfend, lcurr, bgpos, pos, startmain, endmain;
	
	lfoffs = lf->ef_tbase;
	lfend = lfoffs + lf->ef_tsize;

	while  (lfoffs < lfend)  {
		gette(lf, lfoffs, &ltent);
		if  (!ltent.t_isrel)
			goto  gotstart;
		lfoffs += sizeof(unsigned  short);
	}
	return	-1;	/*  Either empty or all relocatable  */
	
gotstart:
	if  (lf->ef_tsize > mf->ef_tsize - startpos + mf->ef_tbase)
		return	0;	/*  At end - can't fit  */

	/*
	 *	Read in a wodge of words from the library file, stopping
	 *	at the end of the file or if we hit a relocatable bit.
	 */
	
	lbcnt = 1;
	startw = lbufw[0] = ltent.t_contents;
	lcurr = lfoffs + 2;

	do  {
		if  (lcurr >= lfend)
			break;
		gette(lf, lcurr, &ltent);
	for  (i = 0;  i < lf->ef_tsize;  i += 2)  {
		gette(lf, lf->ef_tbase + i, &ltent);
		if  (ltent.t_isrel)
			break;
		lbufw[lbcnt++] = ltent.t_contents;
		lcurr += 2;
	}  while  (lbcnt < BFSIZ);
			continue;
		gette(mf, startpos + i, &mtent);
		if  (mtent.t_contents != ltent.t_contents)
			return	0;
		matches++;
	}
	
	/*
	 *	Having got that far, read the text file looking for the start
	 *	word.
	 *	Give up on zero length or all relocatable files.
	 */
	
	startmain = mf->ef_tbase + lfoffs - lf->ef_tbase;
	endmain = startmain + mf->ef_tsize - lf->ef_tsize+lfoffs-lf->ef_tbase+2;
	
	bgpos = startpos + lfoffs - lf->ef_tbase;
	
	for  (pos = bgpos;  pos < endmain;  pos += 2)  {
		gette(mf, pos, &mtent);
		if  (mtent.t_contents != startw)
			continue;
		/*
		 *	Matched first word, try rest.
		 */
	return	matches > 0;
}

		for  (i = 1;  i < lbcnt;  i++)  {
			gette(mf, pos+i+i, &mtent);
			if  (mtent.t_contents != lbufw[i])
				goto  nomatch1;
		}
/*
 *	Scan through main file looking for a match.
 */

		pos += lbcnt*2;
		for  (;  lcurr < lfend;  lcurr += 2, pos += 2)  {
			gette(lf, lcurr, &ltent);
			if  (ltent.t_isrel)
				continue;
			gette(mf, pos, &mtent);
			if  (mtent.t_contents != ltent.t_contents)
				goto  nomatch1;
		}
		
		/*
		 *	Hit the jackpot folks!
		 */
		
		return	pos - lf->ef_tsize;
nomatch1:
		;
	}
long	findstart(mf, lf)
register  ef_fid  mf, lf;
{
	register  long	res = mf->ef_tbase;
	long	lim = mf->ef_tbase + mf->ef_tsize - lf->ef_tsize;
	t_entry	tent;
	
	/*
	 *	Wrap around file. The next bit of code is the sort of
	 *	situation which annoys me where you have to choose between
	 *	having a routine with 200 parameters, or put all sorts of
	 *	fancy tests at each loop. The code, of course is identical.
	 */
	
	for  (pos = startmain;  pos < bgpos;  pos += 2)  {
		gette(mf, pos, &mtent);
		if  (mtent.t_contents != startw)
			continue;
		/*
		 *	Matched first word, try rest.
		 */
		for  (i = 1;  i < lbcnt;  i++)  {
			gette(mf, pos+i+i, &mtent);
			if  (mtent.t_contents != lbufw[i])
				goto  nomatch2;
restart:
	for  (;  res <= lim;  res += 2)  {
		gette(mf, res, &tent);
		if  (tent.t_match != 0)  {
			res += tent.t_match;
			goto  restart;
		}
		pos += lbcnt*2;
		for  (;  lcurr < lfend;  lcurr += 2, pos += 2)  {
			gette(lf, lcurr, &ltent);
			if  (ltent.t_isrel)
				continue;
			gette(mf, pos, &mtent);
			if  (mtent.t_contents != ltent.t_contents)
				goto  nomatch2;
		}
		
		/*
		 *	Hit the jackpot folks!
		 */
		
		return	pos - lf->ef_tsize;
nomatch2:
		;
		if  (matchup(mf, lf, res))
			return	res;
	}

	return	-1;
}

/*
 *	Mark the head of a matched module to save searching.
 */

void	markmatch(mf, lf, pos)
ef_fid	mf, lf;
long	pos;
{
	t_entry	tent;
	
	gette(mf, pos, &tent);
	tent.t_match = (unsigned) lf->ef_tsize;
	putte(mf, pos, &tent);
}
