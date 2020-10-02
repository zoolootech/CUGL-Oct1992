/* [ARCHIVE.C of JUGPDS Vol.17]
*****************************************************************
*								*
*       Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*       Edited & tested by Y. Monma (JUG-C/M Disk Editor)       * 
*								*
*****************************************************************
*/
/* archive - file maintaner */

#include	"stdio.h"

#define	UPD	'u'
#define	TBL	't'
#define	EXTR	'x'
#define	PRINT	'p'
#define	DEL	'd'

#define	MAXFILES	8
#define	NAMESIZE	16
#define	MAXCHARS	32000

#define	makhdr(name, head)	sprintf(head,"-h- %s %d\n", name, fsize(name))

main(argc, argv)
int	argc;
char *argv[];

{
	char	aname[NAMESIZE], cmd;

	if (argc < 3) {
		error("Usage: archive {dptux} archname [files]");
		exit();
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
		error("Usage: archive {dptux} archname [files]");
}

/* getfns -get file names, check for duplicates */
char	fname[MAXFILES][NAMESIZE];
int	fstat[MAXFILES], nfiles, errcnt;

getfns(argc, argv)
char *argv[];

{
	int	i, j;

	errcnt = 0;
	nfiles = argc - 3;
	if (nfiles > MAXFILES)
		error("Too many file names!");
	for (i = 0; i < nfiles; i++) {
		strcpy(fname[i], argv[i+3]);
		fstat[i] = NO;
	}
	for (i = 0; i < nfiles - 1; i++)
		for (j = i + 1; j < nfiles; j++)
			if (!strcmp(fname[i], fname[j]))
				fprintf(STDERR,"%s: duplicate file name.",
						fname[i]);
}

/* update - update existing files, add new ones at end */
update(aname)
char	*aname;

{
	FILE	afd, tfd;
	char	*tname;
	int	i;

	tname = "archtemp";
	if (fopen(aname, &afd) == ERROR) {
		if (fcreat(aname, &afd) == ERROR)
			cant(aname);
		}
	if (fcreat(tname, &tfd) == ERROR)
		cant(tname);
	replac(&afd, &tfd, UPD, &errcnt);
	for (i = 0; i < nfiles; i++)
		if (fstat[i] == NO) {
			addfil(fname[i], &tfd, &errcnt);
			fstat[i] = YES;
		}
	fclose(&afd);
	putc(CPMEOF, &tfd);
	fclose(&tfd);
	if (!errcnt)
		amove(tname, aname);
	else
		remark("Fatal errors - archive not altered.");
	unlink(tname);
}

/* replac - replace or delete files */
replac(afd, tfd, cmd, errcnt)
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
gethdr(fd, buf, name, size)
FILE	*fd;
char	buf[MAXLINE], name[NAMESIZE];
int	*size;

{
	int	len, i;
	char	temp[NAMESIZE];

	if (getlin(buf, fd) == NULL)
		return (EOF);
	i = 0;
	len = getwrd(buf, &i, temp);
	if (strcmp(temp, "-h-"))
		error("archive not in proper format.");
	len = getwrd(buf, &i, name);
	*size = atoi(buf+i);
	return YES;
}

/* fskip - skip n characters on file fd */
fskip(fd, n)
FILE	*fd;

{
	int	c;

	while (n--)
		if ((c = getc(fd)) == EOF || c == CPMEOF)
			break;
}

/* filarg - check if namematches argument list */
filarg(name)
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
acopy(fdi, fdo, size)
FILE	*fdi, *fdo;

{
	int	c;

	while (size--) {
		if ((c = getc(fdi)) == EOF || c == CPMEOF)
	                break;
		putc(c, fdo);
		}
}

/* addfil - add file "name" to archive */
addfil(name, fd, errcnt)
char	*name;
FILE	*fd;
int	*errcnt;

{
	char	head[MAXLINE];
	FILE	nfd;

	if (fopen(name, &nfd) == ERROR) {
		fprintf(STDERR, "%s:  can't add.", name);
		(*errcnt)++;
	}
	if (!*errcnt) {
		makhdr(name, head);
		fputs(head, fd);
		fcopy(&nfd, fd);
		fclose(&nfd);
	}
}

/* amove - move file1 to file2 */
amove(file1, file2)
char *file1, *file2;

{
	FILE	fp1, fp2;

	if (fopen(file1, &fp1) == ERROR)
		cant(file1);
	else if (fcreat(file2, &fp2) == ERROR)
		cant(file2);

	fcopy(&fp1, &fp2);

	fclose(&fp1);
	putc(CPMEOF,&fp2);
	fclose(&fp2);
}

/* fcpy - copy file fp1 to fle fp2 */
fcopy(fp1, fp2)
FILE *fp1, *fp2;

{
	int c;

	while ((c = getc(fp1)) != EOF && c != CPMEOF)
		putc(c,fp2);
}

/* cant - can't open file, exit to CP/M */
cant(fname)
char *fname;

{
	fprintf(STDERR, " can't open %s\n", fname);
	exit(1);
}

/* fsize - size of file in characters */
fsize(name)
char	*name;

{
	int	size;
	FILE	fp;
	int	c;

	if (fopen(name, &fp) == ERROR)
		return ERROR;
	else {
		for (size = 0; (c = getc(&fp)) != EOF && c != CPMEOF; size++)
			;
		fclose(&fp);
		return size;
	}
}

/* getwrd - get non-blank word from in[i] into out, increment i */
getwrd(in, i, out)
char in[], out[];
int  *i;

{
	int	j;

	while (in[(*i)] == ' ' || in[(*i)] == '\t')
	        (*i)++;
	j = 0;
	while (	in[(*i)] != '\0' && in[(*i)] != ' ' &&
		in[(*i)] != '\t' && in[(*i)] != NEWLINE)
		out[j++] = in[(*i)++];
	out[j] = '\0';
	return j;
}

/* getlin - get next line from fp */
getlin(s, fp)
FILE	*fp;
char	*s;

{
	char	*p;
	int	c;

	p = s;
	while((c = getc(fp)) != EOF && c != CPMEOF && c != NEWLINE)
		*s++ = c;
	if(c == NEWLINE)
		*s++ = c;
	*s = '\0';
	return(s-p);
}

/* table - print table of archive contents */
#define	tprint(buf)	(fputs(buf, STDOUT))

table(aname)
char	*aname;

{
	FILE	afd;
	char	in[MAXLINE], lname[NAMESIZE];
	int	size;

	if (fopen(aname, &afd) == ERROR)
		cant(aname);
	while (gethdr(&afd, in, lname, &size) != EOF) {
		if (filarg(lname) == YES)
			tprint(in);
		fskip(&afd, size);
	}
	notfnd();
	fclose(&afd);
}

/* extract - extract files from archive */ 
extrac(aname, cmd)
char	*aname;

{
	FILE	afd, efd;
	char	in[MAXLINE], ename[NAMESIZE];
	int	size, fd;

	if (fopen(aname, &afd) == ERROR)
		cant(aname);
	if (cmd == PRINT)
		fd = STDOUT;
	else
		fd =  ERROR;
	while (gethdr(&afd, in, ename, &size) != EOF)
		if (filarg(ename) == NO)
			fskip(&afd, size);
		else {
			if (fd != STDOUT)
				fd = fcreat(ename, &efd);
			if (fd == ERROR) {
				fprintf(STDERR,"%s:  can't create.", ename);
				errcnt++;
				fskip(&afd, size);
				}
			else {
				if (fd == STDOUT)
					acopy(&afd, STDOUT, size);
				else
					acopy(&afd, &efd, size);
				if (fd != STDOUT)
					fclose(&efd);
				}
			}
	notfnd();
}

/* delete - delete files from archive */
delete(aname)
char	*aname;

{
	FILE	afd, tfd;
	char	*tname;


	tname = "archtemp";

	if (nfiles <= 0)
		error("delete by name only.");
	if (fopen(aname, &afd) == ERROR)
		cant(aname);
	if (fcreat(tname, &tfd) == ERROR)
		cant(tname);
	replac(&afd, &tfd, DEL, &errcnt);
	notfnd();
	fclose(&afd);
	putc(CPMEOF, &tfd);
	fclose(&tfd);
	if (errcnt == 0)
		amove(tname, aname);
	else
		remark("Fatal errors - archive not altered.");
	unlink(tname);
}

/* notfnd - not found */ 
notfnd()

{
	int	i;

	for (i = 0; i < nfiles; i++)
		if (fstat[i] == NO) {
			fprintf(STDERR,"%s:  not in archive.", fname[i]);
			errcnt++;
			}
}
