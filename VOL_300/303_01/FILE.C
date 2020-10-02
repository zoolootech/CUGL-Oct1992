/*
 *	SCCS:	%W%	%G%	%U%
 *	Various operations on files.
 *
 *EMACS_MODES:c
 */

#include <stdio.h>
#include <a.out.h>
#ifdef	COFF
#include <ldfcn.h>
#endif	/*  COFF  */
#include "unc.h"
	
long	lseek();
void	unimpl();

/*
 *	Validate addr and get text entry corresponding to it from the given
 *	file.
 */

void	gette(fid, addr, te)
register  ef_fid  fid;
register  long	addr;
t_entry	*te;
{
	addr -= fid->ef_tbase;
#ifdef	COFF
	if  (addr < 0x300000 && 
	     (addr < 0  ||  addr > fid->ef_tsize  || (addr & 1) != 0))  
	 {
	    (void) fprintf(stderr, "Invalid text address %lx\n", addr);
	    exit(200);
	 }
#else	/*  !COFF  */
	if  (addr < 0  ||  addr > fid->ef_tsize  || (addr & 1) != 0)  {
		(void) fprintf(stderr, "Invalid text address %lx\n", addr);
		exit(200);
	}
#endif	/*  !COFF  */
	(void) lseek(fid->ef_t, (long)(addr * sizeof(t_entry)/2), 0);
	if  (read(fid->ef_t, (char *) te, sizeof(t_entry)) != sizeof(t_entry))  {
		(void) fprintf(stderr, "Trouble reading text at %lx\n", addr);
		exit(201);
	}
}

/*
 *	Store a text entry.
 */

void	putte(fid, addr, te)
register  ef_fid  fid;
register  long	addr;
t_entry	*te;
{
	addr -= fid->ef_tbase;
#ifdef	COFF
	if  (addr < 0x300000 &&
	     (addr < 0  ||  addr > fid->ef_tsize  ||  (addr & 1) != 0))
	 {
	    (void) fprintf(stderr, "Invalid text address %lx\n", addr);
	    exit(200); 
	 }
#else	/*  !COFF  */
	if  (addr < 0  ||  addr > fid->ef_tsize  ||  (addr & 1) != 0)  {
		(void) fprintf(stderr, "Invalid text address %lx\n", addr);
		exit(200);
	}
#endif	/*  !COFF  */
	(void) lseek(fid->ef_t, (long)(addr * sizeof(t_entry)/2), 0);
	(void) write(fid->ef_t, (char *) te, sizeof(t_entry));
}

/*
 *	Validate addr and get data entry corresponding to it from the given
 *	file.
 */

void	getde(fid, addr, de)
register  ef_fid  fid;
register  long	addr;
d_entry	*de;
{
#ifdef	COFF
	if  (addr < 0x300000 && (addr < fid->ef_dbase  ||  addr > fid->ef_end))
	 {
	    (void) fprintf(stderr, "Invalid data address %lx\n", addr);
	    exit(200);
	 }
#else	/*  !COFF  */
	if  (addr < fid->ef_dbase  ||  addr > fid->ef_end)  {
		(void) fprintf(stderr, "Invalid data address %lx\n", addr);
		exit(200);
	}
#endif	/*  !COFF  */
	addr -= fid->ef_dbase;
	(void) lseek(fid->ef_d, (long)(addr * sizeof(d_entry)), 0);
	if  (read(fid->ef_d, (char *) de, sizeof(d_entry)) != sizeof(d_entry))  {
		(void) fprintf(stderr, "Trouble reading data at %lx\n", addr);
		exit(201);
	}
}

/*
 *	Store a data entry.
 */

void	putde(fid, addr, de)
register  ef_fid  fid;
register  long	addr;
d_entry	*de;
{
#ifdef	COFF
	if  (addr < 0x300000 &&
	     (addr < fid->ef_dbase  ||  addr > fid->ef_end))
	 {
	    (void) fprintf(stderr, "Invalid data address %lx\n", addr);
	    exit(200);
	 }
#else	/*  !COFF  */
	if  (addr < fid->ef_dbase  ||  addr > fid->ef_end)  {
		(void) fprintf(stderr, "Invalid data address %lx\n", addr);
		exit(200);
	}
#endif	/*  !COFF  */
	addr -= fid->ef_dbase;
	(void) lseek(fid->ef_d, (long)(addr * sizeof(d_entry)), 0);
	(void) write(fid->ef_d, (char *) de, sizeof(d_entry));
}

/*
 *	Set type and length of given data entry.
 */

void	setde(fid, addr, type, lng)
ef_fid	fid;
long	addr;
unsigned  type;
int	lng;
{
	d_entry	dat;

	if  (addr > fid->ef_end)
		return;
	getde(fid, addr, &dat);
	if  (type == D_CONT  &&  dat.d_reloc != R_NONE)  {
		char	obuf[30];
		(void) sprintf(obuf, "overlapped reloc 0x%x", addr);
		unimpl(obuf);
	}
	dat.d_type = type;
	dat.d_lng = lng;
	putde(fid, addr, &dat);
}
	
/*
 *	Get a word of data file, size as requested.
 */

long	getdw(fid, pos, size)
register  ef_fid  fid;
long	pos;
int	size;
{
	d_entry	dat;
	register  long	res;
	register  int	i, lt;
	
	getde(fid, pos, &dat);
	
	switch  (size)  {
	case  R_BYTE:
		return	dat.d_contents;
		
	case  R_LONG:
		lt = 4;
		goto  rest;
		
	case  R_WORD:
		lt = 2;
	rest:
		res = dat.d_contents;
		for  (i = 1;  i < lt; i++)  {
			getde(fid, pos+i, &dat);
			res = (res << 8) + dat.d_contents;
		}
		return	res;
		
	default:
		(void) fprintf(stderr, "Data word size error\n");
		exit(20);
	}
	/*NOTREACHED*/
}

/*
 *	Get a word of text file.
 */

long	gettw(fid, pos, size)
register  ef_fid  fid;
long	pos;
int	size;
{
	t_entry	tex;
	long	res;
	
	gette(fid, pos, &tex);
	
	switch  (size)  {
	case  R_BYTE:
		return	tex.t_contents >> 8;
		
	case  R_WORD:
		return	tex.t_contents;
		
	case  R_LONG:
		res = tex.t_contents;
		gette(fid, pos+2, &tex);
		return	(res << 16) + tex.t_contents;
	default:
#ifdef	COFF
		(void) fprintf(stderr, "Text< word size error\n");
#else	/*  !COFF  */
		(void) fprintf(stderr, "Text word size error\n");
#endif	/*  !COFF  */
		exit(20);
	}
	/*NOTREACHED*/
}
