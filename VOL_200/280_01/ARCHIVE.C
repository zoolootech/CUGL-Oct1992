/* [ARCHIVE.C of JUGPDS Vol.46] */
/*
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*       Modifird by Toshiya Oota   (JUG-CPM No.10)              *
*                   Sakae ko-po 205 				*
*		    5-19-6 Hosoda				*
*		    Katusikaku Tokyo 124			*
*								*
*		for MS-DOS Lattice C V3.1J & 80186/V20/V30	*
*								*
*	Compiler Option: -ccu -k0(1) -ms -n -v -w		*
*								*
*	Edited & tested by Y. Monma (JUG-CP/M Disk Editor)	*
*			&  T. Ota   (JUG-CP/M Sub Disk Editor)	*
*								*
*****************************************************************
*/

/* Library functions for Software Tools */

#include "stdio.h"
#include "dos.h"
#include "ctype.h"
#include "tools.h"

/* archive - file maintaner */

#define	UPD	'u'
#define	TBL	't'
#define	EXTR	'x'
#define	PRINT	'p'
#define	DEL	'd'

#define	MAXFILES	8
#define	NAMESIZE	16
#define	MAXCHARS	32000

#define	makhdr(name, head)	sprintf(head,"-h- %s %d\n", name, fsize(name))

char	fname[MAXFILES][NAMESIZE];
int	fstat[MAXFILES], nfiles, errcnt;

void	getfns(),update(),table(),extrac(),delete();
void	fskip(),acopy(),addfil(),amove(),fcopy(),cant(),notfnd();

void main(argc, argv)
int	argc;
char 	*argv[];

{
char	aname[NAMESIZE], cmd;

	if (argc < 3) {
		error("ARC901 Usage: archive {dptux} archname [files]");
		exit(1);
		}
	strcpy(aname, argv[2]);
	getfns(argc, argv);
	if ((cmd = tolower(argv[1][0])) == UPD)
		update(aname);
	else if (cmd == TBL)
		table(aname);
	else if (cmd == PRINT || cmd == EXTR)
		extrac(aname, cmd);
	else if (cmd == DEL)
		delete(aname);
	else
		error("ARC902 Option error: use only {dptux} .");
}

/* getfns -get file names, check for duplicates */

void getfns(argc, argv)
char *argv[];

{
int	i, j;

	errcnt = 0;
	nfiles = argc - 3;
	if (nfiles > MAXFILES)
		error("ARC911 Too many file names!");
	for (i = 0; i < nfiles; i++) {
		strcpy(fname[i], argv[i+3]);
		fstat[i] = NO;
	}
	for (i = 0; i < nfiles - 1; i++)
		for (j = i + 1; j < nfiles; j++)
			if (!strcmp(fname[i], fname[j]))
			    fprintf(STDERR,"ARC912 %s: duplicate file name."				                  ,fname[i]);
}

/* update - update existing files, add new ones at end */
void	update(aname)
char	*aname;

{
FILE	*afd,*tfd;
char	*tname;
int	i;
void	replac();

	tname = "archtemp.$$$";
	if ((afd = fopen(aname, "r")) == NO)
		cant(aname);
	if ((tfd = fopen(tname, "w")) == NO)
		cant(tname);
	replac(afd, tfd, UPD, &errcnt);
	for (i = 0; i < nfiles; i++)
		if (fstat[i] == NO) {
			addfil(fname[i], tfd, &errcnt);
			fstat[i] = YES;
		}
	fclose(afd);
	putc(CPMEOF, tfd);
	fclose(tfd);
	if (!errcnt)
		amove(tname, aname);
	else
		remark("ARC913 Fatal errors - archive not altered.");
	unlink(tname);
}

/* replac - replace or delete files */
void replac(afd, tfd, cmd, errcnt)
FILE	*afd, *tfd;
int	*errcnt;

{
char	in[MAXLINE], uname[NAMESIZE];
int	size;

	while (gethdr(afd, in, uname, &size) != EOF)
		if (filarg(uname) == YES) {
			if (cmd == UPD)	
				addfil(uname, tfd, errcnt);
			fskip(afd, size);
		} else {
			fputs(in, tfd);
			acopy(afd, tfd, size);
		}
}

/* gethdr - get header info from fd */
int	gethdr(fd, buf, name, size)
FILE	*fd;
char	buf[MAXLINE], name[NAMESIZE];
int	*size;

{
int	len, i;
char	temp[NAMESIZE];

	if (fgetlin(fd,buf,MAXLINE) == NULL)
		return (EOF);
	i = 0;
	len = getwrd(buf, &i, temp);
	if (strcmp(temp, "-h-"))
		error("ARC914 archive not in proper format.");
	len = getwrd(buf, &i, name);
	*size = atoi(buf+i);
	return YES;
}

/* fskip - skip n characters on file fd */
void fskip(fd, n)
FILE	*fd;
int	n;
{
int	c;

	while (n--)
		if ((c = getc(fd)) == EOF || c == CPMEOF)
			break;
}

/* filarg - check if namematches argument list */
int	filarg(name)
char	*name;

{
int	i;

	if (nfiles <= 0)
	      return (YES);
	for (i = 0; i < nfiles; i++) {
		if (!strcmp(name, fname[i])) {
			fstat[i] = YES;
			return (YES);
	         }
	}
	return (NO);
}

/* acopy - copy size characters from fdi to fdo */ 
void	acopy(fdi, fdo, size)
FILE	*fdi, *fdo;
int	size;
{
int	c;

	while (size--) {
		if ((c = getc(fdi)) == EOF || c == CPMEOF)
	                break;
		putc(c, fdo);
		}
}

/* addfil - add file "name" to archive */
void	addfil(name, fd, errcnt)
char	*name;
FILE	*fd;
int	*errcnt;

{
char	head[MAXLINE];
FILE	*nfd;

	if ((nfd = fopen(name, "r")) == NO) {
		fprintf(STDERR, "ARC915 %s:  can't add.", name);
		(*errcnt)++;
	}
	if (!*errcnt) {
		makhdr(name, head);
		fputs(head, fd);
		fcopy(nfd, fd);
		fclose(nfd);
	}
}

/* amove - move file1 to file2 */
void amove(file1, file2)
char *file1, *file2;

{
FILE	*fp1, *fp2;

	if ((fp1 = fopen(file1,"r" )) == NO)
		cant(file1);
	else if ((fp2 = fopen(file2, "w")) == NO)
		cant(file2);

	fcopy(fp1, fp2);

	fclose(fp1);
	putc(CPMEOF,fp2);
	fclose(fp2);
}

/* fcopy - copy file fp1 to fle fp2 */
void fcopy(fp1, fp2)
FILE *fp1, *fp2;

{
int c;

	while ((c = getc(fp1)) != EOF && c != CPMEOF)
		putc(c,fp2);
}

/* cant - can't open file, exit to CP/M */
void cant(fname)
char *fname;

{
	fprintf(STDERR, "ARC916 can't open %s\n", fname);
	exit(1);
}

/* fsize - size of file in characters */
int	fsize(name)
char	*name;

{
int	size;
FILE	*fp;
int	c;

	if ((fp = fopen(name,"r")) == NO)
		return ERROR;
	else {
		for (size = 0; (c = getc(fp)) != EOF && c != CPMEOF; size++)
			;
		fclose(fp);
		return size;
	}
}

/* table - print table of archive contents */
#define	tprint(buf)	(fputs(buf, STDOUT))

void	table(aname)
char	*aname;

{
FILE	*afd;
char	in[MAXLINE], lname[NAMESIZE];
int	size;

	if ((afd = fopen(aname,"r")) == NO)
		cant(aname);
	while (gethdr(afd, in, lname, &size) != EOF) {
		if (filarg(lname) == YES)
			tprint(in);
		fskip(afd, size);
	}
	notfnd();
	fclose(afd);
}

/* extract - extract files from archive */ 
void	extrac(aname, cmd)
char	*aname;

{
FILE	*afd, *efd ,*fd;
char	in[MAXLINE], ename[NAMESIZE];
int	size;

	if ((afd =fopen(aname,"r")) == NO)
		cant(aname);
	if (cmd == PRINT)
		fd =  STDOUT;
	else
		fd =  NO;
	while (gethdr(afd, in, ename, &size) != EOF)
		if (filarg(ename) == NO)
			fskip(afd, size);
		else {
			if (fd != STDOUT)
				efd = fd = fopen(ename,"w+");
			if (fd == NO) {
				fprintf(STDERR,"ARC917 %s:  can't create."
					      , ename);
				errcnt++;
				fskip(afd, size);
				}
			else {
				if (fd == STDOUT) {
					acopy(afd, STDOUT, size);
				}
				else
					acopy(afd, efd, size);
				if (fd != STDOUT)
					fclose(efd);
				}
			}
	notfnd();
}

/* delete - delete files from archive */
void	delete(aname)
char	*aname;

{
FILE	*afd, *tfd;
char	*tname;


	tname = "archtemp";

	if (nfiles <= 0)
		error("ARC918 delete by name only.");
	if ((afd = fopen(aname,"r")) == NO)
		cant(aname);
	if ((tfd = fopen(tname,"w+")) == NO)
		cant(tname);
	replac(afd, tfd, DEL, &errcnt);
	notfnd();
	fclose(afd);
	putc(CPMEOF, tfd);
	fclose(tfd);
	if (errcnt == 0)
		amove(tname, aname);
	else
		remark("ARC919 Fatal errors - archive not altered.");
	unlink(tname);
}

/* notfnd - not found */ 
void notfnd()

{
int	i;

	for (i = 0; i < nfiles; i++)
		if (fstat[i] == NO) {
			fprintf(STDERR,"ARC920 %s:  not in archive."
				      , fname[i]);
			errcnt++;
			}
}
