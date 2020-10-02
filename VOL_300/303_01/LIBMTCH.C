/*
 *	SCCS:	%W%	%G%	%U%
 *	Read library files.
 *
 *EMACS_MODES:c
 */

#include <stdio.h>
#include <fcntl.h>
#ifdef	COFF
#include <string.h>
#endif	/*  COFF  */
#include <a.out.h>
#include <ar.h>
#include <setjmp.h>
#ifdef	COFF
#include <ldfcn.h>
#endif	/*  COFF  */
#include "unc.h"

#ifdef	COFF
long	atol();
#endif	/*  COFF  */
long	lseek();
void	bfopen(), bfclose(), nomem();
void	rrell2();
void	rrell2(), markmatch();
#ifdef	COFF
char	*malloc();
#else	/*  !COFF  */
char	*strchr(), *strrchr(), *strncpy(), *strcat(), *strcpy(), *malloc();
#endif	/*  !COFF  */
long	matchup();
int	matchup();
long	findstart();

char	verbose;		/*  Tell the world what we are doing  */
char	*tfnam;
char	*cfile;
ef_fids	mainfile;
struct	commit	dreltab;
int	donedrel, donebrel;
long	trelpos, drelpos, brelpos;
#ifdef	COFF
static	struct	libit	currlib = {NULL, NULL, ""};
#else	/*  !COFF  */
static	struct	libit	currlib = {-1, 0, -1, ""};
#endif	/*  !COFF  */

void	lclash(str)
char	*str;
{
	(void) fprintf(stderr, "Library scan failure - %s\n", str);
	(void) fprintf(stderr, "Searching %s\n", cfile);
	if  (currlib.lf_name[0])
		(void) fprintf(stderr, "Member is %s\n", currlib.lf_name);
	exit(255);
}

/*
 *	Find next member.
 */

#ifdef	COFF
long	nextmemb(filename,lfd)
char *filename;
#else	/*  !COFF  */
long	nextmemb(lfd)
#endif	/*  !COFF  */
register  struct  libit	 *lfd;
{
	struct	ar_hdr	arbuf;
	
#ifdef	COFF
	ldaclose(lfd->ldptr2);
	if (ldclose(lfd->ldptr != FAILURE))	/* end of archive */
	    return -1;
	lfd->ldptr = ldopen(filename,lfd->ldptr);
	ldahread(lfd->ldptr, (char *)&arbuf);
#else	/*  !COFF  */
	if  (lfd->lf_next < 0)
		return	-1;
	
	(void) lseek(lfd->lf_fd, lfd->lf_next, 0);
	if  (read(lfd->lf_fd, (char *)&arbuf, sizeof(arbuf)) != sizeof(arbuf))  {
		lfd->lf_next = -1;
		return	-1;
	}
#endif	/*  !COFF  */
	(void) strncpy(lfd->lf_name, arbuf.ar_name, sizeof(lfd->lf_name));
#ifdef	COFF
	return 1;
#else	/*  !COFF  */
	lfd->lf_offset = lfd->lf_next + sizeof(arbuf);
	lfd->lf_next = (lfd->lf_offset + arbuf.ar_size + 1) & ~1;
	return	lfd->lf_offset;
#endif	/*  !COFF  */
}

/*
 *	Decode a file name thus -
 *
 *	-lxxx decode as /lib/libxxx.a /usr/lib/libxxx.a etc
 *	-Lxxx forget "lib" ".a" bit thus -Lcrt0.o
 *	or read LDPATH environment var to give list of directories as sh
 *	(default /lib:/usr/lib).
 *
 *	Alternatively treat as normal pathname.
 *
 *	File names may be followed by (membername) if the file is an archive,
 *	thus
 *
 *		-lc(printf.o)
 *
 *	in which case the specified module is fetched.
 */

struct	libit	*getfnam(str)
#ifdef	COFF
char	*str;	/* will be expanded to full path name if necessary */
#else	/*  !COFF  */
char	*str;
#endif	/*  !COFF  */
{
	char	*bp, *ep = NULL, *pathb, *pathe, *fullpath = NULL;
	static	char	*pathn;
	extern	char	*getenv();
#ifdef	COFF
 	char	magic[8];
	struct	ar_hdr	arhdr;
	LDFILE *ldptr;
 	if  ((bp = strrchr(str, '(')) != NULL &&
 		 (ep = strrchr(str, ')')) != NULL)
#else	/*  !COFF  */
	long	magic;
	struct	ar_hdr	arhdr;
	int	fd;

	if  ((bp = strrchr(str, '(')) != NULL &&
		 (ep = strrchr(str, ')')) != NULL)
#endif	/*  !COFF  */

		*ep = *bp = '\0';

	if  (str[0] == '-'  &&  (str[1] == 'l' || str[1] == 'L'))  {
		if  (pathn == NULL)  {
			if  ((pathn = getenv("LDPATH")) == NULL)
				pathn = "/lib:/usr/lib";
		}
		fullpath = malloc((unsigned)(strlen(pathn) + strlen(str) + 1));
		if  (fullpath == NULL)
			nomem();
		pathb = pathn;
		do  {
#ifdef	COFF
 			pathe = strchr(pathb, ':');
#else	/*  !COFF  */
			pathe = strchr(pathb, ':');
#endif	/*  !COFF  */
			if  (*pathb == ':')
				fullpath[0] = '\0';
			else  {
				if  (pathe != NULL)
					*pathe = '\0';
				(void) strcpy(fullpath, pathb);
				(void) strcat(fullpath, "/");
				if  (pathe != NULL)
					*pathe = ':';
			}
			if  (str[1] == 'l')
				(void) strcat(fullpath, "lib");
			(void) strcat(fullpath, &str[2]);
			if  (str[1] == 'l')
				(void) strcat(fullpath, ".a");
#ifdef	COFF
			if  ((ldptr = ldopen(fullpath, NULL)) != NULL)
#else	/*  !COFF  */
			if  ((fd = open(fullpath, O_RDONLY)) >= 0)
#endif	/*  !COFF  */
				goto  found;
			pathb = pathe + 1;
		}   while  (pathe != NULL);
		
		(void) fprintf(stderr, "Unable to locate lib%s.a in %s\n",
			&str[2], pathn);
		exit(101);
	}
#ifdef	COFF
	else  if  ((ldptr = ldopen(str, NULL)) == NULL)  {
#else	/*  !COFF  */
	else  if  ((fd = open(str, O_RDONLY)) < 0)  {
#endif	/*  !COFF  */
		(void) fprintf(stderr, "Cannot open %s\n", str);
		exit(102);
	}
	
found:

#ifdef	COFF
	str = fullpath? fullpath: str;
 	if  (FREAD(magic, sizeof(magic),1,ldptr) != 1  ||
 		strcmp(magic, ARMAG) != 0)  {
		if  (ep != NULL)  {
			(void) fprintf(stderr, "%s is not library file\n", str);
#else	/*  !COFF  */
	if  ((read(fd, (char *) &magic, sizeof(magic)) != sizeof(magic)  ||
		magic != ARMAG))  {
		if  (ep != NULL)  {
			(void) fprintf(stderr, "%s is not library file\n",
					fullpath != NULL? fullpath: str);
#endif	/*  !COFF  */
			exit(103);
		}
#ifdef	COFF
		currlib.ldptr = ldptr;
		currlib.ldptr2 = ldaopen(str,ldptr);
#else	/*  !COFF  */
		if  (fullpath != NULL)
			free(fullpath);
		currlib.lf_fd = fd;
		currlib.lf_offset = 0;
		currlib.lf_next = -1;
#endif	/*  !COFF  */
		currlib.lf_name[0] = '\0';
		return  &currlib;
	}
	
	/*
	 *	It appears to be a library file - see if we want a specific
	 *	one.
	 */
	
	if  (ep != NULL)  {
#ifdef	COFF
 		char *cp;
 
		for  (;;)  {
			if  (ldahread(ldptr,&arhdr) == FAILURE)  {
				(void) fprintf(stderr, "Cannot find member %s in %s\n",
					bp+1, str);
#else	/*  !COFF  */
		currlib.lf_offset = sizeof(magic) + sizeof(struct ar_hdr);
		for  (;;)  {
			if  (read(fd, &arhdr, sizeof(arhdr)) != sizeof(arhdr))  {
				(void) fprintf(stderr, "Cannot find member %s in %s\n",
					bp+1, fullpath?fullpath: str);
#endif	/*  !COFF  */
				exit(103);
			}
#ifdef	COFF
 			for ( cp = arhdr.ar_name + sizeof(arhdr.ar_name) - 1;
 				*cp == ' ';
 				cp -- ) ;
 			if  (strncmp(bp+1, arhdr.ar_name, cp - arhdr.ar_name + 1) == 0)
#else	/*  !COFF  */
			if  (strncmp(bp+1, arhdr.ar_name, sizeof(arhdr.ar_name)) == 0)
#endif	/*  !COFF  */
				break;
#ifdef	COFF

			if (ldclose(ldptr) != FAILURE) {
				(void) fprintf(stderr, "Cannot find member %s in %s\n",
					bp+1, str);
				exit(103);
			}
			ldptr = ldopen(str,ldptr);
#else	/*  !COFF  */
			currlib.lf_offset += arhdr.ar_size + sizeof(arhdr) + 1;
			currlib.lf_offset &= ~ 1;
			(void) lseek(fd, (long)(currlib.lf_offset - sizeof(arhdr)), 0);
#endif	/*  !COFF  */
		}
#ifdef	COFF
		currlib.ldptr = ldptr;
		currlib.ldptr2 = ldaopen(str,ldptr);
#else	/*  !COFF  */
		if  (fullpath != NULL)
			free(fullpath);
		currlib.lf_fd = fd;
		currlib.lf_next = -1;
#endif	/*  !COFF  */
		currlib.lf_name[0] = '\0';
		*bp = '(';
		*ep = ')';
		return	&currlib;
	}
	
	/*
	 *	Otherwise point to 1st member in library.
	 */
	
#ifdef	COFF
	if  (ldahread(ldptr, &arhdr) == FAILURE)  {
		(void) fprintf(stderr, "Library %s empty\n", str);
#else	/*  !COFF  */
	if  (read(fd, &arhdr, sizeof(arhdr)) != sizeof(arhdr))  {
		(void) fprintf(stderr, "Library %s empty\n", fullpath? fullpath: str);
#endif	/*  !COFF  */
		exit(104);
	}
#ifdef	COFF
	currlib.ldptr = ldptr;
	currlib.ldptr2 = ldaopen(str,ldptr);
#else	/*  !COFF  */
	if  (fullpath != NULL)
		free(fullpath);
	currlib.lf_offset = sizeof(magic) + sizeof(arhdr);
	currlib.lf_next = currlib.lf_offset + arhdr.ar_size + 1;
	currlib.lf_next &= ~1;
	currlib.lf_fd = fd;
#endif	/*  !COFF  */
	(void) strncpy(currlib.lf_name, arhdr.ar_name, sizeof(currlib.lf_name));
	return	&currlib;
}

/*
 *	Process library files.
 */

#define	MINTEXT	6

void	lscan(nfiles, fnames)
int	nfiles;
char	**fnames;
{
	long	tstart = mainfile.ef_tbase;
	ef_fids	libfile;
	register  ef_fid  ll = &libfile;
	register  struct  libit	 *clf;
	extern	symbol	dolsymb();
	int	firstfile;
	
	for  (;  nfiles > 0;  fnames++, nfiles--)  {
		clf = getfnam(*fnames);
		cfile = *fnames;
		firstfile = 1;
		do  {
			bfopen(tfnam, ll);

			/*
			 *	If file is garbled, silently forget it and go
			 *	on to the next one.
			 */

#ifdef	COFF
			if  (!rtext(clf->ldptr, ll))
#else	/*  !COFF  */
			if  (rtext(clf->lf_fd, clf->lf_offset, ll) &&
			     ll->ef_tsize >= MINTEXT &&
			     rdata(clf->lf_fd, clf->lf_offset, ll) &&
			     rrell1(clf->lf_fd, clf->lf_offset, ll) >= 0 &&
			     (trelpos = matchup(&mainfile, ll, tstart))>=0)  {
			if  (!rtext(clf->lf_fd, clf->lf_offset, ll))
#endif	/*  !COFF  */
				goto  closeit;
				
				if  (!rsymb(clf->lf_fd, clf->lf_offset, dolsymb, ll))  {
					(void) fprintf(stderr, "Corrupt file %s\n",
							*fnames);
					exit(150);
				}
				donedrel = 0;
				donebrel = 0;
				rrell2(clf->lf_fd, clf->lf_offset, ll);
				if  (verbose)  {
					(void) fprintf(stderr, "Found: ");
					if  (clf->lf_name[0])
						(void) fprintf(stderr, "%.14s\n",
							clf->lf_name);
					else
						(void) fprintf(stderr, "%s\n", *fnames);
				}
				if  (libfile.ef_stvec != NULL)  {
					free(libfile.ef_stvec);
					libfile.ef_stvec = NULL;
					libfile.ef_stcnt = 0;
				}
				dreltab.c_int = 0;
			if  (ll->ef_tsize < MINTEXT)
				goto  closeit;
				
#ifdef	COFF
			if  (!rdata(clf->ldptr, ll))
#else	/*  !COFF  */
				/*
				 *	Start looking next time round
				 *	where last one left off.
				 */
			if  (!rdata(clf->lf_fd, clf->lf_offset, ll))
#endif	/*  !COFF  */
				goto  closeit;
				
#ifdef	COFF
			if  (rrell1(clf->ldptr, ll) < 0)
#else	/*  !COFF  */
				tstart = trelpos + libfile.ef_tsize;
			if  (rrell1(clf->lf_fd, clf->lf_offset, ll) < 0)
#endif	/*  !COFF  */
				goto  closeit;
				
			/*
			 *	If first file in library, find it from
			 *	beginning of main file.
			 */
			
			if  (firstfile)  {
				if  ((trelpos = findstart(&mainfile, ll)) < 0)
					goto  closeit;
				firstfile = 0;
			}
			else   if  (!matchup(&mainfile, ll, trelpos))
					goto  closeit;
			
			/*
			 *	Found a match.
			 */
			
#ifdef	COFF
			if  (!rsymb(clf->ldptr, dolsymb, ll))  {
#else	/*  !COFF  */
			if  (!rsymb(clf->lf_fd, clf->lf_offset, dolsymb, ll))  {
#endif	/*  !COFF  */
				(void) fprintf(stderr, "Corrupt file %s\n",
							*fnames);
				exit(150);
			}
			
			donedrel = 0;
			donebrel = 0;
#ifdef	COFF
			rrell2(clf->ldptr, clf->ldptr2, ll);
#else	/*  !COFF  */
			rrell2(clf->lf_fd, clf->lf_offset, ll);
#endif	/*  !COFF  */
			if  (verbose)  {
				(void) fprintf(stderr, "Found: ");
				if  (clf->lf_name[0])
					(void) fprintf(stderr, "%.14s in ",
							clf->lf_name);
				(void) fprintf(stderr, "%s\n", *fnames);
			}
			if  (libfile.ef_stvec != NULL)  {
				free(libfile.ef_stvec);
				libfile.ef_stvec = NULL;
				libfile.ef_stcnt = 0;
			}
			dreltab.c_int = 0;
				
			/*
			 *	Start looking next time round
			 *	where last one left off.
			 */
			
			markmatch(&mainfile, ll, trelpos);
			trelpos += libfile.ef_tsize;
closeit:
			bfclose(ll);
#ifdef	COFF
		}  while  (nextmemb(cfile,clf) >= 0);
#else	/*  !COFF  */
		}  while  (nextmemb(clf) >= 0);
#endif	/*  !COFF  */
	}
}
