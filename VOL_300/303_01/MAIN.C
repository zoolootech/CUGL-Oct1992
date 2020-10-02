/*
 *	SCCS:	%W%	%G%	%U%
 *	Main routine etc.
 *
 *EMACS_MODES:c
 */

#include <stdio.h>
#include <fcntl.h>
#include <a.out.h>
#ifdef	COFF
#include <ldfcn.h>
#else	/*  !COFF  */
#include <sys/var.h>
#endif	/*  !COFF  */
#include "unc.h"

#define	LINELNG	70

void	inturdat(), intutext(), intudat(), intlsym();
void	ptext(), pdata(), pabs(), pbss(), lscan();

ef_fids	mainfile;

#ifndef	COFF
int	par_entry, par_round;	/*  68000 parameters  */
#endif	/*  !COFF  */
int	nmods;			/*  Number of modules it looks like  */

char	*tfnam = "split";

char	lsyms;			/*  Generate local symbols  */
char	verbose;		/*  Tell the world what we are doing  */
char	noabs;			/*  No non-global absolutes  */
int	rel;			/*  File being analysed is relocatable  */
int	lpos;
#ifdef	COFF
char    shlibout;		/*  output values for shlib constants */
#endif	/*  COFF  */

symbol	dosymb();
struct	libit	*getfnam();

/*
 *	Get hex characters, also allowing for 'k' and 'm'.
 */

int	ghex(str)
register  char	*str;
{
	register  int	result = 0;
	register  int	lt;

	for  (;;)  {
		lt = *str++;
		switch  (lt)  {
		default:
err:			(void) fprintf(stderr, "Invalid hex digit \'%c\'\n", lt);
			exit(1);
			
		case '\0':
			return	result;
			
		case '0':case '1':case '2':case '3':case '4':
		case '5':case '6':case '7':case '8':case '9':
			result = (result << 4) + lt - '0';
			continue;
			
		case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
			result = (result << 4) + lt - 'a' + 10;
			continue;

		case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
			result = (result << 4) + lt - 'A' + 10;
			continue;
		
		case 'k':case 'K':
			if  (*str != '\0')
				goto  err;
			return  result << 10;
			
		case 'm':case 'M':
			if  (*str != '\0')
				goto  err;
			return  result << 20;
		}
	}
}

/*
 *	Process entry line options.  Return number dealt with.
 */

int	doopts(av)
char	*av[];
{
	register  int	cnt = 0, lt;
	register  char	*arg;
#ifndef	COFF
	struct	var	vs;
#endif	/*  !COFF  */
	
#ifndef	COFF
	uvar(&vs);
	par_entry = vs.v_ustart;
	par_round = vs.v_txtrnd - 1;
	
#endif	/*  !COFF  */
	for  (;;)  {
		arg = *++av;
		if  (*arg++ != '-')
			return	cnt;
		cnt++;
		
nx:		switch  (lt = *arg++)  {
		default:
			(void) fprintf(stderr, "Bad option -%c\n", lt);
			exit(1);
			
		case  '\0':
			continue;
			
		case  'l':	/*  A file name  */
		case  'L':
			return	cnt - 1;
			
		case  's':
			lsyms++;
			goto  nx;
			
		case  'v':
			verbose++;
			goto  nx;
#ifdef	COFF

		case  'V':
			shlibout++;
			goto  nx;

#else	/*  !COFF  */
			
#endif	/*  !COFF  */
		case  'a':
			noabs++;
			goto  nx;

#ifdef	COFF
		case  't':
#else	/*  !COFF  */
		case  'R':
		case  'N':
#endif	/*  !COFF  */
			if  (*arg == '\0')  {
				cnt++;
				arg = *++av;
#ifdef	COFF
				if  (arg == NULL) {
#else	/*  !COFF  */
				if  (arg == NULL)  {
#endif	/*  !COFF  */
bo:					(void) fprintf(stderr,"Bad -%c option\n",lt);
					exit(1);
#ifdef	COFF
 				      }
#else	/*  !COFF  */
				}
#endif	/*  !COFF  */
			}
#ifndef	COFF
			if  (lt == 'R')
				par_entry = ghex(arg);
			else
				par_round = ghex(arg) - 1;
			continue;
			
		case  't':
			if  (*arg == '\0')  {
				cnt++;
				arg = *++av;
				if  (arg == NULL)
					goto  bo;
			}
#endif	/*  !COFF  */
			tfnam = arg;
			continue;
			
		case  'o':
			if  (*arg == '\0')  {
				cnt++;
				arg = *++av;
				if  (arg == NULL)
					goto  bo;
			}
			if  (freopen(arg, "w", stdout) == NULL)  {
				(void) fprintf(stderr, "Help! cant open %s\n", arg);
				exit(20);
			}
			continue;
		}
	}
}
	
/*
 *	Open binary files.  Arrange to erase them when finished.
 */

void	bfopen(nam, fid)
char	*nam;
ef_fid	fid;
{
	char	fnam[80];
	
	(void) sprintf(fnam, "%s.tx", nam);
	if  ((fid->ef_t = open(fnam, O_RDWR|O_CREAT, 0666)) < 0)  {
efil:		(void) fprintf(stderr, "Help could not open %s\n", fnam);
		exit(4);
	}
	(void) unlink(fnam);
	(void) sprintf(fnam, "%s.dt", nam);
	if  ((fid->ef_d = open(fnam, O_RDWR|O_CREAT, 0666)) < 0)
		goto  efil;
	(void) unlink(fnam);
}

/*
 *	Close binary files.  They should get zapped anyway.
 */

void	bfclose(fid)
ef_fid	fid;
{
	(void) close(fid->ef_t);
	(void) close(fid->ef_d);
}

/*
 *	Main routine.
 */

main(argc, argv)
int	argc;
char	*argv[];
{
	int	i;
	char	*progname = argv[0];
	char	*msg;
	register  struct  libit  *lfd;
	
#ifdef	COFF
	setbuf(stdout,NULL);
	setbuf(stderr,NULL);

#endif	/*  COFF  */
	i = doopts(argv);
	argc -= i;
	argv += i;
	
	if  (argc < 2)  {
		(void) fprintf(stderr, "Usage: %s [ options ] file\n", progname);
		exit(1);
	}
	
	lfd = getfnam(argv[1]);
#ifdef	COFF
	if  (TYPE(lfd->ldptr) == ARTYPE)  {
#else	/*  !COFF  */
	if  (lfd->lf_next > 0)  {
#endif	/*  !COFF  */
		(void) fprintf(stderr, "Main file (%s) cannot be library\n", argv[1]);
		exit(2);
	}
	
	bfopen(tfnam, &mainfile);
	if  (verbose)
		(void) fprintf(stderr, "Scanning text\n");
#ifdef	COFF
	if  (!rtext(lfd->ldptr, &mainfile))  {
#else	/*  !COFF  */
	if  (!rtext(lfd->lf_fd, lfd->lf_offset, &mainfile))  {
#endif	/*  !COFF  */
		msg = "text";
bf:		(void) fprintf(stderr, "Bad format input file - reading %s\n", msg);
		exit(5);
	}
	if  (verbose)
		(void) fprintf(stderr, "Scanning data\n");
#ifdef	COFF
	if  (!rdata(lfd->ldptr, &mainfile))  {
#else	/*  !COFF  */
	if  (!rdata(lfd->lf_fd, lfd->lf_offset, &mainfile))  {
#endif	/*  !COFF  */
		msg = "data";
		goto  bf;
	}
	if  (verbose)
		(void) fprintf(stderr, "Scanning symbols\n");
#ifdef	COFF
	if  (!rsymb(lfd->ldptr, dosymb, &mainfile))  {
#else	/*  !COFF  */
	if  (!rsymb(lfd->lf_fd, lfd->lf_offset, dosymb, &mainfile))  {
#endif	/*  !COFF  */
		msg = "symbols";
		goto  bf;
	}
	if  (verbose)
		(void) fprintf(stderr, "Scanning for relocation\n");
#ifdef	COFF
	if  ((rel = rrel(lfd->ldptr, lfd->ldptr2, &mainfile)) < 0)  {
#else	/*  !COFF  */
	if  ((rel = rrel(lfd->lf_fd, lfd->lf_offset, &mainfile)) < 0)  {
#endif	/*  !COFF  */
		msg = "reloc";
		goto  bf;
	}
	
	if  (rel)  {
		if  (verbose)
			(void) fprintf(stderr, "File is relocatable\n");
		if  (argc > 2)
			(void) fprintf(stderr, "Sorry - no scan on reloc files\n");
	}
	else
		lscan(argc - 2, &argv[2]);

	if  (verbose)
		(void) fprintf(stderr, "End of input\n");
	
#ifdef	COFF
	ldaclose(lfd->ldptr2);
	ldclose(lfd->ldptr);
#else	/*  !COFF  */
	(void) close(lfd->lf_fd);
#endif	/*  !COFF  */
	if  (nmods > 0)
		(void) fprintf(stderr, "Warning: at least %d merged modules\n",
			nmods + 1);

	if  (mainfile.ef_stvec != NULL)  {
		free(mainfile.ef_stvec);
		mainfile.ef_stvec = NULL;
		mainfile.ef_stcnt = 0;
	}
	
	if  (verbose)
		(void) fprintf(stderr, "Text anal 1\n");
	intutext();
	if  (verbose)
		(void) fprintf(stderr, "Data anal 1\n");
	intudat(&mainfile);
	if  (!rel)  {
		if  (verbose)
			(void) fprintf(stderr, "Data anal 2\n");
		inturdat(&mainfile);
	}
	if  (lsyms)  {
		if  (verbose)
			(void) fprintf(stderr, "Local symbol scan\n");
		intlsym();
	}
	pabs();
	ptext(&mainfile);
	pdata(&mainfile);
	pbss(&mainfile);
	bfclose(&mainfile);
	exit(0);
}
