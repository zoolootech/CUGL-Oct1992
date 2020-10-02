/*
 * The routines in this file read and write ASCII files from the disk. All of
 * the knowledge about files are here.
 */

#include        <stdio.h>
#include	"estruct.h"
#include        "edef.h"

#if	USG | BSD | (MSDOS & MSC)
#include	<sys/types.h>
#endif

#if	USG | BSD | (MSDOS & (MSC | TURBO))
#include	<sys/stat.h>
#endif

#if	C86
#include <stat.h>
#endif

#if	VMS
#include <stat.h>
#include <fab.h>
#endif

#ifndef	O_RDONLY
#define O_RDONLY	0
#endif

/* Make getc and putc macroes if they are missing */

#ifndef getc
#define	getc(f)	fgetc(f)
#endif

#ifndef putc
#define	putc(c,f)	fputc(c,f)
#endif

FILE	*ffp;		/* File pointer, all functions. */
int eofflag;		/* end-of-file flag */

/*
 * Open a file for reading.
 */
ffropen(fn, getstat)
char    *fn;
{
#if	VMS | USG | BSD
	int	ffd;		/* File descriptor */
	struct stat s;

	ffd = open(fn, O_RDONLY);
	if (getstat && fstat(ffd, &s) == 0) {
		curbp->b_fmode = s.st_mode;
#if	VMS
		curbp->b_fab_rfm = s.st_fab_rfm;
		curbp->b_fab_rat = s.st_fab_rat;
#endif
	}
	if (ffd < 0 || (ffp=fdopen(ffd, "r")) == NULL)
#else
#if MSDOS
	if ((ffp=fopen(fn, ((curbp->b_mode&MDBINARY)? "rb": "r"))) == NULL)
#else
	if ((ffp=fopen(fn, "r")) == NULL)
#endif
#endif
                return (FIOFNF);
	eofflag = FALSE;
        return (FIOSUC);
}

#if	VMS
/* names for record attributes and record formats */
int rms_maxrat = 3;
char *rms_rat[] = {"ftn", "cr", "prn", "blk"};
int rms_maxrfm = 6;
char *rms_rfm[] = /* {"udf",  "fix",  "var", "vfc", "stm", "stmlf", "stmcr"} */
		     {"stmlf","stmlf","var", "vfc", "stm", "stmlf", "stmcr"};
#endif

/*
 * Open a file for writing. Return TRUE if all is well, and FALSE on error
 * (cannot create).
 */
ffwopen(fn)
char    *fn;
{
#if	VMS | USG | BSD
	int	ffd;		/* File descriptor */

#if     VMS
	int i, n;
	char ratstr[80], rfmstr[80];

	n = curbp->b_fab_rfm;
	if (n < 0 || n > rms_maxrfm) n = 2;
	sprintf(rfmstr, "rfm=%s", rms_rfm[ n ]);

	n = 0;
	strcpy(ratstr, "rat=");
	for(i = 0; i <= rms_maxrat; i++)
		if (curbp->b_fab_rat & (1 << i))	{
			if (n++ > 0) strcat(ratstr, ",");
			strcat(ratstr, rms_rat[i]);
		}
	if (n == 0) strcat(ratstr, rms_rat[1]);

	ffd = creat(fn, curbp->b_fmode, rfmstr, ratstr);
#else
	ffd = creat(fn, curbp->b_fmode);
#endif
	if (ffd < 0 || (ffp=fdopen(ffd, "w")) == NULL) {
#else
#if MSDOS
	if ((ffp=fopen(fn, ((curbp->b_mode&MDBINARY)? "wb": "w"))) == NULL) {
#else
	if ((ffp=fopen(fn, "w")) == NULL) {
#endif
#endif
                mlwrite("Cannot open file for writing");
                return (FIOERR);
        }
        return (FIOSUC);
}

/*
 * Close a file. Should look at the status in all systems.
 */
ffclose()
{
	/* free this since we do not need it anymore */
	if (fline) {
		free(fline);
		fline = NULL;
	}

#if	MSDOS & CTRLZ
	putc(26, ffp);		/* add a ^Z at the end of the file */
#endif
	
#if     V7 | USG | BSD | (MSDOS & (LATTICE | MSC | TURBO | C86))
        if (fclose(ffp) != FALSE) {
                mlwrite("Error closing file");
                return(FIOERR);
        }
        return(FIOSUC);
#else
        fclose(ffp);
        return (FIOSUC);
#endif
}

/*
 * Write a line to the already opened file. The "buf" points to the buffer,
 * and the "nbuf" is its length, less the free newline. Return the status.
 * Check only at the newline.
 */
ffputline(buf, nbuf)
char    buf[];
{
        register int    i;
#if	CRYPT
	char c;		/* character to translate */

	if (cryptflag) {
	        for (i = 0; i < nbuf; ++i) {
			c = buf[i] & 0xff;
			crypt(&c, 1);
			putc(c, ffp);
		}
	} else
	        for (i = 0; i < nbuf; ++i)
        	        putc(buf[i]&0xFF, ffp);
#else
        for (i = 0; i < nbuf; ++i)
                putc(buf[i]&0xFF, ffp);
#endif

#if	ST520
        putc('\r', ffp);
#endif        
        putc('\n', ffp);

        if (ferror(ffp)) {
                mlwrite("Write I/O error");
                return (FIOERR);
        }

        return (FIOSUC);
}

/*
 * Read a line from a file, and store the bytes in the supplied buffer. The
 * "nbuf" is the length of the buffer. Complain about long lines and lines
 * at the end of the file that don't have a newline present. Check for I/O
 * errors too. Return status.
 */
ffgetline()

{
        register int c;		/* current character read */
        register int i;		/* current index into fline */
	register char *tmpline;	/* temp storage for expanding line */

	frlen = 0;

	/* if we are at the end...return it */
	if (eofflag)
		return(FIOEOF);

	/* dump fline if it ended up too big */
	if (flen > 2 * NSTRING && fline != NULL) {
		free(fline);
		fline = NULL;
	}

	/* if we don't have an fline, allocate one */
	if (fline == NULL)
		if ((fline = malloc(flen = NSTRING)) == NULL)
			return(FIOMEM);

	/* read the line in */
        i = 0;
        while ((c = getc(ffp)) != EOF && c != '\n') {
                fline[i++] = c;
		/* if it's longer, get more room */
                if (i >= flen) {
                	if ((tmpline = malloc(flen+NSTRING)) == NULL)
                		return(FIOMEM);
			for(i=0; i<flen; i++) tmpline[i] = fline[i];
			i = flen;
                	flen += NSTRING;
                	free(fline);
                	fline = tmpline;
                }
        }

#if	ST520
	if(fline[i-1] == '\r')
		i--;
#endif

	/* test for any errors that may have occured */
        if (c == EOF) {
                if (ferror(ffp)) {
                        mlwrite("File read error");
                        return(FIOERR);
                }

                if (i != 0)
			eofflag = TRUE;
		else
			return(FIOEOF);
        }

	/* terminate and decrypt the string */
	frlen = i;
        fline[i] = 0;

#if	CRYPT
	if (cryptflag)
		crypt(fline, frlen);
#endif
        return(FIOSUC);
}

#if	AZTEC & MSDOS
#undef	fgetc
/*	a1getc:		Get an ascii char from the file input stream
			but DO NOT strip the high bit
*/

int a1getc(fp)

FILE *fp;

{
	int c;		/* translated character */

	c = getc(fp);	/* get the character */

	/* if its a <LF> char, throw it out  */
	while (c == 10)
		c = getc(fp);

	/* if its a <RETURN> char, change it to a LF */
	if (c == '\r')
		c = '\n';

	/* if its a ^Z, its an EOF */
	if (c == 26)
		c = EOF;

	return(c);
}
#endif

#if	ABACKUP

/*
 * Test if a file is newer than its auto-saved version
 */

ffisnewer(fn)
char    *fn;
{
#if	VMS | USG | BSD | (MSDOS & (MSC | TURBO | C86))
	char asvname[NFILEN + 4];
#if	C86
	struct fbuf asvstat, fnstat;
#else
	struct stat asvstat, fnstat;
#endif

	makeasvname(asvname, fn);
	if (stat(asvname, &asvstat) != 0) return(TRUE);
	if (stat(fn, &fnstat) != 0) return(FALSE);
	if (asvstat.st_ctime > fnstat.st_ctime) return(FALSE);
#endif
	return(TRUE);
}

#endif

