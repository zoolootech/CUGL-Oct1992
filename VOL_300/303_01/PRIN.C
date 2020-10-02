/*
 *	SCCS:	%W%	%G%	%U%
 *	Print stuff.
 *
 *EMACS_MODES:c
 */

#include <stdio.h>
#include <a.out.h>
#ifdef	COFF
#include <ldfcn.h>
#endif	/*  COFF  */
#include "unc.h"

#define	LINELNG	70

void	gette(), getde();
long	gettw(), getdw();
void	prinst();

char	noabs;			/*  No non-global absolutes  */
int	rel;			/*  File being analysed is relocatable  */
int	lpos;

struct	commit	abstab, comtab;
#ifdef	COFF
extern char shlibout;
#endif	/*  COFF  */

/*
 *	Print absolute and common values.
 */

void	pabs()
{
	register  int	i;
	register  symbol  cs;

	for  (i = 0;  i < abstab.c_int;  i++)
	
	for  (i = 0;  i < abstab.c_int;  i++)  {
		cs = abstab.c_symb[i];
#ifdef	COFF
		if (cs->s_value >= 0x300000 && ! shlibout)
			continue;
#endif	/*  COFF  */
		if  (cs->s_glob)
#ifdef	COFF
			(void) printf("#\tglobal\t%s\n", cs->s_name);
		else  if  (noabs)
			continue;
		(void) printf("# %s\t= 0x%lx\n", cs->s_name, cs->s_value);
	}
	for  (i = 0;  i < comtab.c_int;  i++)  {
		cs = comtab.c_symb[i];
		(void) printf("\tcomm\t%s,%d\n", cs->s_name, cs->s_value);
#else	/*  !COFF  */
			(void) printf("\t.globl\t%s\n", cs->s_name);
		else  if  (noabs)
			continue;
		(void) printf("%s\t=\t0x%lx\n", cs->s_name, cs->s_value);
	}
	for  (i = 0;  i < comtab.c_int;  i++)  {
		cs = comtab.c_symb[i];
		(void) printf("\t.comm\t%s,%d\n", cs->s_name, cs->s_value);
#endif	/*  !COFF  */
	}
}

/*
 *	Print out labels.
 */

void	plabs(ls, seg)
register  symbol  ls;
int	seg;
{
	for  (; ls != NULL;  ls = ls->s_link)  {
		if  (ls->s_type != seg)
			continue;
		if  (ls->s_lsymb)  {
#ifdef	COFF
			(void) printf("L%%%u:\n", ls->s_lsymb);
#else	/*  !COFF  */
			(void) printf("%u$:\n", ls->s_lsymb);
#endif	/*  !COFF  */
			return;		/*  Set last  */
		}
		if  (ls->s_glob)
#ifdef	COFF
			(void) printf("\n\tglobal\t%s", ls->s_name);
		if (ls->s_name[0] == '.')
			(void) printf("\n# %s:\n", ls->s_name);
		else
			(void) printf("\n%s:\n", ls->s_name);
#else	/*  !COFF  */
			(void) printf("\n\t.globl\t%s", ls->s_name);
		(void) printf("\n%s:\n", ls->s_name);
#endif	/*  !COFF  */
	}
}

/*
 *	Print out text.
 */

void	ptext(fid)
register  ef_fid  fid;
{
	register  long	tpos, endt;
	t_entry	tstr;

#ifdef	COFF
	(void) fputs("\ttext\n", stdout);
#else	/*  !COFF  */
	(void) printf(".text\n");
#endif	/*  !COFF  */
	
	tpos = fid->ef_tbase;
	endt = tpos + fid->ef_tsize;
contin:	
	for  (;  tpos < endt;  tpos += tstr.t_lng * 2)  {
		gette(fid, tpos, &tstr);
#ifdef	COFF
		plabs(tstr.t_lab, S_TEXT);
#else	/*  !COFF  */
		plabs(tstr.t_lab, TEXT);
#endif	/*  !COFF  */
		if  (tstr.t_type == T_BEGIN)
			prinst(&tstr, tpos);
		else  if  (tstr.t_relsymb != NULL)  {
#ifdef	COFF
			(void) printf("\tlong\t%s", tstr.t_relsymb->s_name);
			if  (tstr.t_relsymb->s_type!=S_TEXT &&
				tstr.t_relsymb->s_type!=S_DATA)
#else	/*  !COFF  */
			(void) printf("\t.long\t%s", tstr.t_relsymb->s_name);
			if  (tstr.t_relsymb->s_type!=TEXT &&
				tstr.t_relsymb->s_type!=DATA)
#endif	/*  !COFF  */
				(void) printf("+0x%x", gettw(fid, tpos, R_LONG));
#ifdef	COFF
			(void) putchar('\n');
#else	/*  !COFF  */
			putchar('\n');
#endif	/*  !COFF  */
			tpos += 4;
			goto  contin;
		}
		else
#ifdef	COFF
			(void) printf("\tshort\t0x%x\n", tstr.t_contents);
#else	/*  !COFF  */
			(void) printf("\t.word\t0x%x\n", tstr.t_contents);
#endif	/*  !COFF  */
	}

	/*
	 *	Print out any trailing label.
	 */
	
	gette(fid, tpos, &tstr);
#ifdef	COFF
	plabs(tstr.t_lab, S_TEXT);
#else	/*  !COFF  */
	plabs(tstr.t_lab, TEXT);
#endif	/*  !COFF  */
}

/*
 *	Print out data.
 */

void	pdata(fid)
register  ef_fid  fid;
{
	register  long	dpos, endd;
#ifdef	COFF
	register  int	lng, ccnt;
#else	/*  !COFF  */
	register  int	lng;
#endif	/*  !COFF  */
	unsigned  ctyp;
	int	had, par, inc;
	char	*msg;
	d_entry	dstr;
	
#ifdef	COFF
	(void) fputs("\n\tdata\n", stdout);
#else	/*  !COFF  */
	(void) printf("\n.data\n");
#endif	/*  !COFF  */
	
	dpos = fid->ef_dbase;
	endd = dpos + fid->ef_dsize;

	while  (dpos < endd)  {
		
		getde(fid, dpos, &dstr);
#ifdef	COFF
		plabs(dstr.d_lab, S_DATA);
#else	/*  !COFF  */
		plabs(dstr.d_lab, DATA);
#endif	/*  !COFF  */
			
		switch  (dstr.d_type)  {
		case  D_CONT:
			(void) fprintf(stderr, "Data sync error\n");
#ifdef	COFF
			dpos++;
			break;
#else	/*  !COFF  */
			exit(200);
#endif	/*  !COFF  */
			
		case  D_ASC:
		case  D_ASCZ:
			ctyp = dstr.d_type;
			lng = dstr.d_lng;
#ifdef	COFF
		nextline:
			(void) fputs("\tbyte\t", stdout);
			ccnt=0;
#else	/*  !COFF  */
			(void) printf("\t.asci");
			if  (ctyp == D_ASC)
				(void) printf("i\t\"");
			else  {
				(void) printf("z\t\"");
				lng--;
			}
				
#endif	/*  !COFF  */
			while  (lng > 0)  {
#ifdef	COFF
				if (ccnt) (void) putchar(',');
#endif	/*  COFF  */
				getde(fid, dpos, &dstr);
				switch  (dstr.d_contents)  {
				default:
					if  (dstr.d_contents < ' ' ||
						dstr.d_contents > '~')
#ifdef	COFF
						ccnt += printf("0x%x", dstr.d_contents);
					else {
						(void) putchar('\'');
						(void)putchar(dstr.d_contents);
						ccnt += 2;
					}
#else	/*  !COFF  */
						(void) printf("\\%.3o", dstr.d_contents);
					else
						putchar(dstr.d_contents);
#endif	/*  !COFF  */
					break;
#ifndef	COFF
				case  '\"':
				case  '\'':
#endif	/*  !COFF  */
				case  '\\':
#ifdef	COFF
					(void) putchar('\'');
					(void) putchar('\\');
					(void) putchar('\\');
					ccnt+=2;
#else	/*  !COFF  */
				case  '|':
					(void) printf("\\%c", dstr.d_contents);
#endif	/*  !COFF  */
					break;
				case  '\b':
#ifdef	COFF
					(void) putchar('\'');
					(void) putchar('\\');
					(void) putchar('b');
					ccnt+=2;
#else	/*  !COFF  */
					(void) printf("\\b");
#endif	/*  !COFF  */
					break;
				case  '\n':
#ifdef	COFF
					(void) putchar('\'');
					(void) putchar('\\');
					(void) putchar('n');
					ccnt+=2;
#else	/*  !COFF  */
					(void) printf("\\n");
#endif	/*  !COFF  */
					break;
				case  '\r':
#ifdef	COFF
					(void) putchar('\'');
					(void) putchar('\\');
					(void) putchar('r');
					ccnt+=2;
#else	/*  !COFF  */
					(void) printf("\\r");
#endif	/*  !COFF  */
					break;
#ifdef	COFF
				case  '\f':
					(void) putchar('\'');
					(void) putchar('\\');
					(void) putchar('f');
					ccnt+=2;
					break;
				case  '\t':
					(void) putchar('\'');
					(void) putchar('\\');
					(void) putchar('t');
					ccnt+=2;
					break;
#endif	/*  COFF  */
				}
#ifdef	COFF

#else	/*  !COFF  */
				
#endif	/*  !COFF  */
				lng--;
				dpos++;
#ifdef	COFF
				if (++ccnt > 40 && lng > 0) {
					(void) putchar('\n');
					goto nextline;
				}
#endif	/*  COFF  */
			}
#ifdef	COFF
			(void) putchar('\n');
#else	/*  !COFF  */
			(void) printf("\"\n");
			if  (ctyp == D_ASCZ)
				dpos++;
#endif	/*  !COFF  */
			break;

		case  D_BYTE:
			msg = "byte";
			par = R_BYTE;
			inc = 1;
			goto  wrest;
			
		case  D_WORD:
#ifdef	COFF
			msg = "short";
#else	/*  !COFF  */
			msg = "word";
#endif	/*  !COFF  */
			par = R_WORD;
			inc = 2;
			goto  wrest;
			
		case  D_LONG:
			msg = "long";
			par = R_LONG;
			inc = 4;
		wrest:
#ifdef	COFF
			(void) putchar('\t');
			(void) fputs(msg, stdout);
			(void) putchar('\t');
#else	/*  !COFF  */
			(void) printf("\t.%s\t", msg);
#endif	/*  !COFF  */
			lng = dstr.d_lng;
			lpos = 16;
			had = 0;
			while  (lng > 0)  {
#ifdef	COFF
				if  (lpos > LINELNG) {
					(void) putchar('\n');
					(void) putchar('\t');
					(void) fputs(msg, stdout);
					(void) putchar('\t');
#else	/*  !COFF  */
				if  (lpos > LINELNG)  {
					(void) printf("\n\t.%s\t", msg);
#endif	/*  !COFF  */
					lpos = 16;
				}
#ifdef	COFF
				else  if  (had) {
					(void) putchar(',');
					(void) putchar(' ');
					lpos += 2;
				}
#else	/*  !COFF  */
				else  if  (had)
					lpos += printf(", ");

#endif	/*  !COFF  */
				lpos += printf("0x%x", getdw(fid, dpos, par));
				lng -= inc;
				dpos += inc;
				had++;
			}
#ifdef	COFF
			(void) putchar('\n');
#else	/*  !COFF  */
			putchar('\n');
#endif	/*  !COFF  */
			break;

		case  D_ADDR:
#ifdef	COFF
			(void) fputs("\tlong\t", stdout);
#else	/*  !COFF  */
			(void) printf("\t.long\t");
#endif	/*  !COFF  */
			lng = dstr.d_lng;
			lpos = 16;
			had = 0;
			while  (lng > 0)  {
				if  (lpos > LINELNG)  {
#ifdef	COFF
					(void) fputs("\n\tlong\t", stdout);
#else	/*  !COFF  */
					(void) printf("\n\t.long\t");
#endif	/*  !COFF  */
					lpos = 16;
				}
#ifdef	COFF
				else  if  (had) {
					(void) putchar(',');
					(void) putchar(' ');
					lpos += 2;
				}
#else	/*  !COFF  */
				else  if  (had)
					lpos += printf(", ");
#endif	/*  !COFF  */

				getde(fid, dpos, &dstr);
				lpos += printf("%s", dstr.d_relsymb->s_name);
				lng -= sizeof(long);
				dpos += sizeof(long);
				had++;
			}
#ifdef	COFF
			(void) putchar('\n');
#else	/*  !COFF  */
			putchar('\n');
#endif	/*  !COFF  */
			break;
		}
	}
	
	/*
	 *	Print trailing label.
	 */
	
	getde(fid, dpos, &dstr);
#ifdef	COFF
	plabs(dstr.d_lab, S_DATA);
#else	/*  !COFF  */
	plabs(dstr.d_lab, DATA);
#endif	/*  !COFF  */
}

void	pbss(fid)
register  ef_fid  fid;
{
	register  long	bpos = fid->ef_bbase;
	long	endb = fid->ef_end;
	d_entry	bstr;
	
#ifdef	COFF
	(void) fputs("\n# .bss\n", stdout);
	while  (bpos < endb)  {
		getde(fid, bpos, &bstr);
		plabs(bstr.d_lab, S_BSS);
		(void) printf("\tspace\t%d\n", bstr.d_lng);
		bpos += bstr.d_lng;
	}
	getde(fid, endb, &bstr);
	plabs(bstr.d_lab, S_BSS);
#else	/*  !COFF  */
	(void) printf("\n.bss\n");
	
	while  (bpos < endb)  {
		getde(fid, bpos, &bstr);
		plabs(bstr.d_lab, BSS);
		(void) printf("\t.space\t%d\n", bstr.d_lng);
		bpos += bstr.d_lng;
	}
	
	getde(fid, endb, &bstr);
	plabs(bstr.d_lab, BSS);
#endif	/*  !COFF  */
	
}
#ifdef	COFF


#endif	/*  COFF  */
