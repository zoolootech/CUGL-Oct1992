/*
 * Lar - LU format library file maintainer
 * 	by Stephen C. Hemminger
 * 	linus!sch or sch @Mitre-Bedford MA
 *
 *	Lattice version T. Jennings 1 Dec 83
 *
 * DESCRIPTION
 *    Lar is a program to manipulate CP/M LU format libraries.
 *    The original CP/M library program LU is the product
 *    of Gary P. Novosielski. The primary use of lar is to combine several
 *    files together for upload/download to a personal computer.
 *
 * Usage: lar key library [files] ...
 *
 * Key functions are:
 * u - Update, add files to library	(also creates new libraries)
 * t - Table of contents
 * e - Extract files from library
 * p - Print files in library
 * d - Delete files in library
 * r - Reorganize library
 *
 * EXAMPLES:
 * lar t foo.lbr		list all files in FOO.LBR
 * lar e foo.lbr 1.c 2.c	extract files 1.c, 2.c from FOO.LBR
 * lar p foo.lbr 1.c		display 1.c from FOO.LBR
 * lar u foo.lbr 1.c 2.c 3.c 	add or replace files in FOO.LBR
 *
 * When creating a new library, you will be prompted for the maximum
 * number of entries it can contain. Assuming NEW.LBR doen't exist ...
 * lar u new.lbr		create an empty library
 * lar u new.lbr a.c,b.c,d.c	create NEW.LBR, add files.
 *
 * The Reorganize option causes <lbrfile>.tmp to be created, and
 * the contents of the old library to be copied into it.
 *
 * This program is public domain software, no warranty intended or
 * implied.
 *
 *
 * PORTABILITY
 *	The original version by Stephan C. Hemminger was set up for 
 *	Version 7 UNIX, and was not useable as is. It has been hacked
 *	to fit the Lattice C compiler for MSDOS and CP/M-86. The basic
 *	problems were: fread() and fwrite() incompatibility, no 
 *	text/binary differntiation problem in MSDOS, structure alignment
 *	problems, Lattice vs. UNIX vs. BDS. Lattice does not gaurentee
 *	structure element alignment (i.e. an int after a char is not 
 *	necessarily the next byte address) and therefore there is a serious
 *	kludge in the structure definition for the library.
 *
 *	Also, I have made random changes to the source merely to reflect
 *	my programming taste; the original code was quite good. I have also
 *	changed the wording of some errors, etc more in line with the current
 *	flavor of messages in the micro environment. The original Verbose
 *	flag option was removed, and made the default. No need to suppress
 *	what few messages and text there is.
 *
 *	As mentioned before, ther is no problem with text or binary files;
 * 	they are treated identically. Control-Z characters are added to the
 *	end of all files to round it up to a multiple of 128 bytes.
 *
 *	Note that all files are kept as multiples of 128 bytes, to be 
 *	compatible with the CP/M utility LU. This may present a problem
 *	with certain data files, but will be OK for text and .COM files
 *	anyways, and probably most other files.
 *
 * NOTES ON MSDOS VERSION
 *	There is an annoying problem with this program when run under MSDOS.
 *	If for some reason an update should fail (disk full, not enough slots)
 *	the file will not be closed, and therefore there will be orphan data
 *	blocks hanging around. Do CHKDSK\F to fix this. The problem is with
 *	the use of the error() function, which is called throughout and just
 *	exits to DOS after reporting the error, with out closing the file.
 *	Check your .LBR file after this happens; it may be bad. Do a reorganize
 *	to fix if necessary.
 *
 *	To compensate for this, I also fixed a bug in the original version. If
 *	an empty library was created, it could not be used. One had to be 
 *	created with at least one file. The problem was that the directory-
 *	getter function (getdir) always read 16 bytes more than the actual
 *	size of the directory; this was OK if there was files in the library.
 *
 * * Unix is a trademark of Bell Labs.
 * ** CP/M is a trademark of Digital Research.
 */

#include <stdio.h>
#include <ctype.h>

/* There is no tell(), but lseek() does the same thing. For some reason seek()
is called lseek. Fix this minor annoyance. */

long lseek();
#define tell(f) lseek(f,0L,1)
#define seek lseek

/* Library file status values: */

#define ACTIVE 0
#define UNUSED	0xff
#define	DELETED	0xfe
#define	CTRLZ	0x1a


#define	MAXFILES 256
#define	SECTOR 128
#define	DSIZE (sizeof(struct ludir))
#define	SLOTS_SEC (SECTOR/DSIZE)
#define	equal(s1, s2) (	strcmp(s1,s2) == 0 )
#define false 0
#define	true 1
#define	bool int

/* Globals */
char *fname[MAXFILES];
bool ftouched[MAXFILES];

/* This bullshit (or something like it) is necessary because the byte order
of structure elements is not garenteed in Lattice C, i.e. an INT right after
an CHAR is not necessarily &( CHAR + 1), i.e. there's a fill byte after the
CHAR to put it on a word boundary.

This is incredibly gross, and forces proper alignment for Lattice ONLY !!! */

struct ludir {			/* internal dir. stucture, 32 bytes */
	char l_l[12];		/* 12 byte filename: */
#define l_stat l_l[0]		/* status of file, */
#define l_name l_l[1]		/* filename, */
#define l_ext l_l[9]		/* extention, */
	int l_off;		/* offset in library, */
	int l_len;		/* length of file, */
	int l_fill[8];		/* 16 byte filler, */
} ldir[MAXFILES];

int errcnt, nfiles, nslots;

char *getname(), *sprintf();

main (argc, argv)
int argc;
char **argv;
{
char *flagp;
char *aname;

	if (argc < 3)
		help ();
	aname =	argv[2];	/* name of LBR file, */
	filenames (argc, argv);

	switch (tolower(*argv[1])) {
	case 'u': 
		update(aname);
		break;
	case 't': 
		table(aname);
		break;
	case 'e': 
		extract(aname);
		break;
	case 'p': 
		print(aname);
		break;
	case 'd': 
		delete(aname);
		break;
	case 'r': 
		reorg(aname);
		break;
		break;
	default: 
		help();
	}
	exit();
}

/* print error message and exit	*/
help ()	{
	printf ("Usage: LAR [utepdr] library [files] ...\n");
	printf ("Functions are:\n\tu - Update, add files to library\n");
	printf ("\tt - Table of contents\n");
 	printf ("\te - Extract files from library\n");
	printf ("\tp - Print files in library\n");
	printf ("\td - Delete files in library\n");
	printf ("\tr - Reorganize library\n");
	exit (1);
}

error (str)
char *str;
{
	printf ("LAR: %s\n", str);
	exit (1);
}

cant (name)
char *name;
{

	printf ("%s: File open error\n", name);
	exit (1);
}

/* Get file names, check for dups, and initialize */

filenames (ac, av)
char **av;
{
 register int i, j;

	errcnt = 0;
	for (i = 0; i < ac - 3; i++) {
	fname[i] = av[i + 3];
	ftouched[i] = false;
	if (i == MAXFILES)
		error ("Too many file names.");
	}
	fname[i] = NULL;
	nfiles = i;
	for (i = 0; i < nfiles; i++)
	for (j = i + 1; j < nfiles; j++)
		if (equal (fname[i], fname[j])) {
		printf ("%s", fname[i]);
		error (": duplicate file name");
		}
}

table (lib)
char *lib;
{
int lfd;
register int i, total;
int active = 0, unused = 0, deleted = 0;
char *uname;

	if ((lfd= open(lib,0x8002)) == -1)
		cant (lib);

	getdir (lfd);
	total =	ldir[0].l_len;
	printf("Name         Index    Length (sectors)\n");
	printf("Directory    %4u    %6u\n", total, total);

	for (i = 1; i < nslots;	i++)
		switch(ldir[i].l_stat) {
		case ACTIVE:
			active++;
			uname = getname(&ldir[i].l_name, &ldir[i].l_ext);
			total += ldir[i].l_len;
			printf ("%-12s %4u   %7u\n", uname,ldir[i].l_off,ldir[i].l_len);
			break;
		case UNUSED:
			unused++;
			break;
		default:
			deleted++;
	}
	printf("-----------------------------\n");
	printf("Total sectors       %7u\n", total);
	printf("\nLibrary %s has %u slots, %u deleted, %u active, %u unused\n",
		lib, nslots, deleted, active, unused);
	close (lfd);
	not_found ();
}

getdir (f)
int f;
{
int cnt;

	seek(f,0L,0);
	if (read(f,&ldir[0],DSIZE) != DSIZE)	/* read 1st entry to find */
		error ("No directory\n");	/* number of slots, */

	nslots = ldir[0].l_len * SLOTS_SEC;
	cnt= DSIZE * (nslots - 1);		/* already read one slot, */

	if (read(f,&ldir[1],cnt) != cnt)
		error ("Can't read directory - is it a library?");
}

putdir (f)
int f;
{

	seek(f,0L,0);
	if (write(f,&ldir,nslots * DSIZE) != (nslots * DSIZE))
		error ("Can't write directory - library may be botched");
}

initdir	(f)
int f;
{
register int i;
int numsecs;
char line[80];

	for (;;) {
		cprintf ("Number of slots to allocate: ");
		getstring(line);
		cprintf("\r\n");
		nslots = atoi (line);
		if (nslots < 1)
			printf ("Must have at least one!\n");
		else if (nslots > MAXFILES)
			printf	("Too many slots\n");
		else
			break;
	}

	numsecs	= nslots / SLOTS_SEC;
	nslots = numsecs * SLOTS_SEC;

	for (i = 0; i < nslots;	i++) {
		ldir[i].l_stat= UNUSED;
		blank_fill(&ldir[i].l_name,8);
		blank_fill(&ldir[i].l_ext,3);
	}
	ldir[0].l_stat = ACTIVE;
	ldir[0].l_len= numsecs;

	putdir (f);
}
/* Fill	an array with blanks, no trailing null.	*/
blank_fill(s,n)
char *s;
int n;
{
	while (n--) *s++= ' ';
}
/* convert nm.ex to a Unix style string */

char *getname (nm, ex)
char *nm, *ex;
{
static char namebuf[14];
int i,j;

	for (i= 0; (i < 8) && (nm[i] != ' '); i++)
		namebuf[i]= tolower(nm[i]);
	j= i;
	namebuf[j++]= '.';

	for (i= 0; (i < 3) && (ex[i] != ' '); i++)
		namebuf[j++]= tolower(ex[i]);
	namebuf[j]= '\0';

	return namebuf;
}

putname (cpmname, unixname)
char *cpmname, *unixname;
{

	cvt_to_fcb(unixname,cpmname);
}

/* filarg - check if name matches argument list */
filarg (name)
char *name;
{
register int i;

	if (nfiles <= 0)
	return 1;

	for (i = 0; i < nfiles;	i++)
	if (equal (name, fname[i])) {
		ftouched[i] = true;
		return	1;
	}

	return 0;
}

not_found () {
register int i;

	for (i = 0; i < nfiles;	i++)
	if (!ftouched[i]) {
		printf ("%s: not in library.\n", fname[i]);
		errcnt++;
	}
}

extract(name)
char *name;
{
	getfiles(name, false);
}

print(name)
char *name;
{
	getfiles(name, true);
}

getfiles (name,	pflag)
char *name;
bool pflag;
{
int lfd, ofd;
register int i;
char *unixname;

	if ((lfd= open(name,0x8002)) == -1)
		cant (name);
	getdir (lfd);

	for (i = 1; i < nslots;	i++) {
	if(ldir[i].l_stat != ACTIVE)
		continue;
	unixname = getname (&ldir[i].l_name, &ldir[i].l_ext);
	if (!filarg (unixname))
		continue;
	printf("Extracting %s\n", unixname);
	if (pflag)
		ofd= open("CON",0x8002);
	else
		ofd= creat(unixname,0x8002);
	if (ofd == -1) {
		printf (" - can't create output file\n");
		errcnt++;
	}
	else {
		seek (lfd, (long) ldir[i].l_off * SECTOR,0);
		acopy (lfd, ofd, ldir[i].l_len);
		if (!pflag)
			close (ofd);
	}
	}
	close (lfd);
	not_found ();
}

acopy (fdi, fdo, nsecs)
int fdi, fdo;
unsigned nsecs;
{
char buf[SECTOR];

	while( nsecs-- != 0) {
		if (read(fdi,buf,SECTOR) != SECTOR)
			error("Can't read");
		if (write(fdo,buf,SECTOR) != SECTOR)
			error ("write error (acopy)");
	}
}

update (name)
char *name;
{
int lfd;
register int i;

	if ((lfd = open (name,0x8002)) == -1) {
		if ((lfd = creat(name, 2)) == -1)
			cant (name);
		initdir (lfd);

/* We shouldnt have to do this, but writes to the (new) file fail otherwise. */

		close(lfd);		/* close it, */
		lfd= open(name,0x8002);	/* reopen it, */
	} 
	getdir (lfd);			/* read directory, */

	for (i = 0; (i < nfiles) && (errcnt == 0); i++)
		addfil (fname[i], lfd);
	if (errcnt != 0)
		printf ("fatal errors - last file may be bad\n");
	putdir (lfd);
	close (lfd);
}

addfil (name, lfd)
char *name;
int lfd;
{
int ifd;
register int secoffs, numsecs;
register int i;

	if ((ifd= open(name,0x8002)) == -1) {
		printf ("%s: can't find to add\n",name);
		errcnt++;
		return;
	}
	for (i = 0; i < nslots;	i++) {
		if (equal( getname (&ldir[i].l_name, &ldir[i].l_ext), name) ) {
			printf("Updating existing file %s\n",name);
			break;
		}
		if (ldir[i].l_stat != ACTIVE) {
			printf("Adding new file %s\n",name);
			break;
		}
	}
	if (i >= nslots) {
		printf("Can't add %s, library is full\n",name);
		errcnt++;
		return;
	}

	ldir[i].l_stat = ACTIVE;
	putname	(&ldir[i].l_name, name);
	seek(lfd, 0L, 2);		/* append to end */
	secoffs	= tell(lfd) / SECTOR;

	seek(lfd,0L,2);			/* clear write error? */

	ldir[i].l_off= secoffs;
	numsecs = fcopy (ifd, lfd);
	ldir[i].l_len= numsecs;
	close (ifd);
}

fcopy (ifd, ofd)
int ifd, ofd;
{
int total = 0;
int i,n;
char sectorbuf[SECTOR];


	while (n= read(ifd,sectorbuf,SECTOR) ) {
		if (n != SECTOR) {
			for (i	= n; i < SECTOR; i++)
				sectorbuf[i] = CTRLZ;
		}
		if (write(ofd,sectorbuf,SECTOR) != SECTOR)
			error("write error (fcopy)");
		++total;
	}
	return total;
}

delete (lname)
char *lname;
{
int f;
register int i;

	if ((f= open(lname,0x8002)) == -1)
		cant (lname);

	if (nfiles <= 0)
		error("delete by name only");

	getdir (f);
	for (i = 0; i < nslots;	i++) {
		if (!filarg ( getname (&ldir[i].l_name, &ldir[i].l_ext)))
			continue;
		ldir[i].l_stat = DELETED;
	}

	not_found();
	if (errcnt > 0)
		printf ("errors - library not updated\n");
	else
		putdir (f);
	close (f);
}

reorg (name)
char *name;
{
int olib, nlib;
int oldsize;
register int i, j;
struct ludir odir[MAXFILES];
char tmpname[SECTOR];

	for (i= 0; (i < 8) && (name[i] != '.'); i++) /* copy filename, */
		tmpname[i]= name[i];		/* strip off extention, */
	tmpname[i]= '\0';
	strcat(tmpname,".tmp");			/* make new name, */

	if ((olib= open(name,0x8002)) == -1)
		cant(name);

	if ((nlib= creat(tmpname,0x8002)) == -1)
		cant(tmpname);

	getdir(olib);
	printf("Old library has %d slots\n", oldsize = nslots);
	for(i = 0; i < nslots ; i++)
		copymem( (char	*) &odir[i], (char *) &ldir[i],
			sizeof(struct ludir));
	initdir(nlib);
	errcnt = 0;

	for (i = j = 1; i < oldsize; i++)
	if( odir[i].l_stat == ACTIVE ) {
		printf("Copying: %-8.8s.%3.3s\n",&odir[i].l_name, &odir[i].l_ext);
		copyentry( &odir[i], olib, &ldir[j], nlib);
		if (++j >= nslots) {
			errcnt++;
			printf("Not enough room in new library\n");
			break;
		}
	}

	close(olib);
	putdir(nlib);
	close (nlib);

	if (errcnt == 0) {
		unlink(name);			/* delete orig file, */
		link(tmpname,name);		/* rename it, */
	} else {
		printf("Errors, library not updated\n");
		unlink(tmpname);
	}
}

copyentry( old, of, new, nf )
struct ludir *old, *new;
int of, nf;
{

	register int secoffs, numsecs;
	char buf[SECTOR];

	new->l_stat = ACTIVE;
	copymem(&new->l_name, &old->l_name, 8);
	copymem(&new->l_ext, &old->l_ext, 3);
	seek(of, (long) (old->l_off * SECTOR), 0);
	seek(nf, 0L, 2);
	secoffs = tell(nf) / SECTOR;

	seek(nf,0L,2);		/* clear write error? */

	new->l_off= secoffs;
	numsecs	= old->l_len;
	new->l_len= numsecs;

	while(numsecs--	!= 0) {
		if (read(of,buf,SECTOR) != SECTOR)
			error("read error");
		if (write(nf,buf,SECTOR) != SECTOR)
			error("write error (copyentry)");
	}
}

copymem(dst, src, n)
register char *dst, *src;
register unsigned int n;
{
	while(n-- != 0)
		*dst++ = *src++;
}
/* ATOI() function missing from Lattice C. From Kernighan and Richie. */

atoi(s)
char s[];
{
int i,n;

	n= 0;
	for (i= 0; s[i] >= '0' && s[i] <= '9'; ++i) 
		n= 10*n + s[i]-'0';
	return(n);
}
/* Missing Lattice C function: File rename

	error= link(oldname,newname); */

link(old,new)
char *old,*new;
{
char fcb[36];
int i;
char *p;

	for (i= 0; i < sizeof(fcb); i++)	/* clear it out first, */
		fcb[i]= '\0';

	cvt_to_fcb(old,&fcb[1]);		/* first name, */
	cvt_to_fcb(new,&fcb[17]);		/* new name, */
	return(bdos(23,&fcb) == 0xff);		/* do it. */
}

/* Convert a normal asciz string to MSDOS/CPM FCB format. Make the filename
portion 8 characters, extention 3 maximum. */

cvt_to_fcb(inname,outname)
char *inname;
char outname[];
{
char c;
int i;

	for (i= 0; i < 11; i++)
		outname[i]= ' ';		/* clear out name, */
	for (i= 0; i < 11; i++) {
		if (*inname == '\0')		/* if null, */
			outname[i]= ' ';	/* pad with blanks, */
		else if (*inname == '.') {	/* if a dot, */
			++inname;		/* skip it, */
			i= 7;			/* skip to extention, */
		} else {
			outname[i]= toupper(*inname);
			++inname;
		}
	}
	return;
}
