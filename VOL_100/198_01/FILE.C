/*	FILE.C:   for MicroEMACS

	The routines in this file handle the reading, writing
	and lookup of disk files.  All of details about the
	reading and writing of the disk are in "fileio.c".

*/

#include        <stdio.h>
#include	"estruct.h"
#include        "edef.h"

/*
 * Read a file into the current
 * buffer. This is really easy; all you do it
 * find the name of the file, and call the standard
 * "read a file into the current buffer" code.
 * Bound to "C-X C-R".
 */
fileread(f, n)
{
        register int    s;
        char fname[NFILEN];

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
        if ((s=mlreply("Read file: ", fname, NFILEN)) != TRUE)
                return(s);
        return(readin(fname, TRUE, FALSE));
}

/*
 * Insert a file into the current
 * buffer. This is really easy; all you do it
 * find the name of the file, and call the standard
 * "insert a file into the current buffer" code.
 * Bound to "C-X C-I".
 */
insfile(f, n)
{
        register int    s;
        char fname[NFILEN];

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
	if (curbp->b_mode&MDVIEW)	/* don't allow this command if	*/
		return(rdonly());	/* we are in read only mode	*/
        if ((s=mlreply("Insert file: ", fname, NFILEN)) != TRUE)
                return(s);
        return(ifile(fname));
}

/*
 * Select a file for editing.
 * Look around to see if you can find the
 * fine in another buffer; if you can find it
 * just switch to the buffer. If you cannot find
 * the file, create a new buffer, read in the
 * text, and switch to the new buffer.
 * Bound to C-X C-F.
 */
filefind(f, n)
{
        char fname[NFILEN];	/* file user wishes to find */
        register int s;		/* status return */

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
        if ((s=mlreply("Find file: ", fname, NFILEN)) != TRUE)
                return(s);
	return(getfile(fname, TRUE));
}

#if	DECEDT
/*
 * Select a file for editing and insert it into another window.
 * If there is only one window, split it, otherwise pick a non-current window.
 * Look around to see if you can find the
 * file in another buffer; if you can find it
 * just switch to the buffer. If you cannot find
 * the file, create a new buffer, read in the
 * text, and switch to the new buffer.
 * Bound to C-X 4.
 */
filewfind(f, n)
{
        char fname[NFILEN];	/* file user wishes to find */
        register int s;		/* status return */
	register WINDOW *wp;

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
	if ((s=mlreply("Find file: ", fname, NFILEN)) != TRUE) return(s);

	/* look for a window */
	wp = wheadp;
	while (wp != NULL && wp->w_wndp != curwp)
		wp = wp->w_wndp;
	if (wp == NULL) wp = curwp->w_wndp;
	if (wp == NULL) {
		if ((s=splitwind(f, n)) != TRUE) return(s);
		wp = spltwp;
		}
	curwp = wp;
	curbp = wp->w_bufp;

	s = getfile(fname, TRUE);
	return(s);
}

/*
 * Read the last saved version of a file into the current buffer.
 * Unound.
 */
filerevert(f, n)
{
	char fname[NFILEN];

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
	if ((curbp->b_flag&BFCHG) == 0)         /* Return, no changes.  */
		return (TRUE);
	if (curbp->b_fname[0] == 0) {           /* Must have a name.    */
		mlwrite("No file name");
		return (FALSE);
	}
	strcpy(fname, curbp->b_fname);
	return(readin(fname, TRUE, FALSE));
}
#endif

viewfile(f, n)	/* visit a file in VIEW mode */
{
        char fname[NFILEN];	/* file user wishes to find */
        register int s;		/* status return */

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
        if ((s=mlreply("View file: ", fname, NFILEN)) != TRUE)
                return (s);
	s = getfile(fname, FALSE);
	if (s) {	/* if we succeed, put it in view mode */
		curwp->w_bufp->b_mode |= MDVIEW;

		/* scan through and update mode lines of all windows */
		upmode();
	}
	return(s);
}

#if	CRYPT
resetkey()	/* reset the encryption key if needed */

{
	register int s;	/* return status */

	/* turn off the encryption flag */
	cryptflag = FALSE;

	/* if we are in crypt mode */
	if (curbp->b_mode & MDCRYPT) {
		if (curbp->b_key[0] == 0) {
			s = setkey(FALSE, 0);
			if (s != TRUE)
				return(s);
		}

		/* let others know... */
		cryptflag = TRUE;

		/* and set up the key to be used! */
		/* de-encrypt it */
		crypt((char *)NULL, (unsigned) 0);
		crypt(curbp->b_key, (unsigned) strlen(curbp->b_key));

		/* re-encrypt it...seeding it to start */
		crypt((char *)NULL, (unsigned) 0);
		crypt(curbp->b_key, (unsigned) strlen(curbp->b_key));
	}

	return(TRUE);
}
#endif

getfile(fname, lockfl)

char fname[];		/* file name to find */
int lockfl;		/* check the file for locks? */

{
        register BUFFER *bp;
        register LINE   *lp;
        register int    i;
        register int    s;
        char bname[NBUFN];	/* buffer name to put file */

#if	MSDOS
	mklower(fname);		/* msdos isn't case sensitive */
#endif
        for (bp=bheadp; bp!=NULL; bp=bp->b_bufp) {
                if ((bp->b_flag&BFINVS)==0 && strcmp(bp->b_fname, fname)==0) {
			swbuffer(bp);
                        lp = curwp->w_dotp;
                        i = curwp->w_ntrows/2;
                        while (i-- && lback(lp)!=curbp->b_linep)
                                lp = lback(lp);
                        curwp->w_linep = lp;
                        curwp->w_flag |= WFMODE|WFHARD;
                        mlwrite("[Old buffer]");
                        return (TRUE);
                }
        }
        makename(bname, fname);                 /* New buffer name.     */
        while ((bp=bfind(bname, FALSE, 0)) != NULL) {
		/* old buffer name conflict code */
                s = mlreply("Buffer name: ", bname, NBUFN);
                if (s == ABORT)                 /* ^G to just quit      */
                        return (s);
                if (s == FALSE) {               /* CR to clobber it     */
                        makename(bname, fname);
                        break;
                }
        }
        if (bp==NULL && (bp=bfind(bname, TRUE, 0))==NULL) {
                mlwrite("Cannot create buffer");
                return (FALSE);
        }
        if (--curbp->b_nwnd == 0) {             /* Undisplay.           */
                curbp->b_dotp = curwp->w_dotp;
                curbp->b_doto = curwp->w_doto;
                curbp->b_markp = curwp->w_markp;
                curbp->b_marko = curwp->w_marko;
                curbp->b_fcol  = curwp->w_fcol;
        }
        curbp = bp;                             /* Switch to it.        */
        curwp->w_bufp = bp;
        curbp->b_nwnd++;
        return(readin(fname, lockfl, FALSE));   /* Read it in.          */
}

/*
	Read file "fname" into the current buffer, blowing away any text
	found there.  Called by both the read and find commands.  Return
	the final status of the read.  Also called by the mainline, to
	read in a file specified on the command line as an argument. 
	The command bound to M-FNR is called after the buffer is set up
	and before it is read. 
*/

readin(fname, lockfl, recover)

char    fname[];	/* name of file to read */
int	lockfl;		/* check for file locks? */
int	recover;	/* recover an autosaved file */

{
        register LINE   *lp1;
        register LINE   *lp2;
        register int    i;
        register WINDOW *wp;
        register BUFFER *bp;
        register int    s;
        register int    nbytes;
        register int    nline;
	char mesg[NSTRING];
        char tempname[NFILEN + 4];

#if	FILOCK
	if (lockfl && lockchk(fname) == ABORT)
		return(ABORT);
#endif
        bp = curbp;                             /* Cheap.               */
        if ((s=bclear(bp)) != TRUE)             /* Might be old.        */
                return (s);
        bp->b_flag &= ~(BFINVS|BFCHG);
        strcpy(bp->b_fname, fname);

	/* let a user macro get hold of things...if he wants */
	execute(FUNC|'R', HOOK, 1);

#if	CRYPT
	s = resetkey();
	if (s != TRUE)
		return(s);
#endif

	/* turn off ALL keyboard translation in case we get a dos error */
	TTkclose();

#if	ABACKUP
	if (recover) makeasvname(tempname, fname);
	else
#endif
	strcpy(tempname, fname);

        if ((s=ffropen(tempname, !recover)) == FIOERR) {  /* Hard file open. */
		strcpy(mesg, "[File error");
                goto out;
	}

        if (s == FIOFNF) {                      /* File not found.      */
                strcpy(mesg, "[New file");
                goto out;
        }

	/* read the file in */
        mlwrite("[Reading file]");
        nline = 0;
        while ((s=ffgetline()) == FIOSUC) {
                nbytes = frlen;
                if ((lp1=lalloc(nbytes)) == NULL) {
                        s = FIOMEM;             /* Keep message on the  */
                        break;                  /* display.             */
                }
                lp2 = lback(curbp->b_linep);
                lp2->l_fp = lp1;
                lp1->l_fp = curbp->b_linep;
                lp1->l_bp = lp2;
                curbp->b_linep->l_bp = lp1;
                for (i=0; i<nbytes; ++i)
                        lputc(lp1, i, fline[i]);
                ++nline;
        }
        ffclose();                              /* Ignore errors.       */
	strcpy(mesg, "[");
	if (s==FIOERR) {
		strcat(mesg, "I/O ERROR, ");
		curbp->b_flag |= BFTRUNC;
	}
	if (s == FIOMEM) {
		strcat(mesg, "OUT OF MEMORY, ");
		curbp->b_flag |= BFTRUNC;
	}
	sprintf(&mesg[strlen(mesg)], "Read %d line", nline);
        if (nline > 1)
		strcat(mesg, "s");

out:
#if	ABACKUP
	if ((!recover) && (!ffisnewer(fname)))
	  strcat(mesg, "; autosave file is newer, consider M-X recover-file");
#endif
	strcat(mesg, "]");
	mlwrite(mesg);

	TTkopen();	/* open the keyboard again */
        for (wp=wheadp; wp!=NULL; wp=wp->w_wndp) {
                if (wp->w_bufp == curbp) {
                        wp->w_linep = lforw(curbp->b_linep);
                        wp->w_dotp  = lforw(curbp->b_linep);
                        wp->w_doto  = 0;
                        wp->w_markp = NULL;
                        wp->w_marko = 0;
                        wp->w_flag |= WFMODE|WFHARD;
                }
        }
        if (s == FIOERR || s == FIOFNF)		/* False if error.      */
                return(FALSE);
        return (TRUE);
}

/*
 * Take a file name, and from it
 * fabricate a buffer name. This routine knows
 * about the syntax of file names on the target system.
 * I suppose that this information could be put in
 * a better place than a line of code.
 */
makename(bname, fname)
char    bname[];
char    fname[];
{
        register char *cp1;
        register char *cp2;

        cp1 = &fname[0];
        while (*cp1 != 0)
                ++cp1;

#if     AMIGA
        while (cp1!=&fname[0] && cp1[-1]!=':' && cp1[-1]!='/')
                --cp1;
#endif
#if     VMS | DECUSC
        while (cp1!=&fname[0] && cp1[-1]!=':' && cp1[-1]!=']')
                --cp1;
#endif
#if     CPM
        while (cp1!=&fname[0] && cp1[-1]!=':')
                --cp1;
#endif
#if     MSDOS
        while (cp1!=&fname[0] && cp1[-1]!=':' && cp1[-1]!='\\'&&cp1[-1]!='/')
                --cp1;
#endif
#if     ST520
        while (cp1!=&fname[0] && cp1[-1]!=':' && cp1[-1]!='\\')
                --cp1;
#endif
#if     FINDER
        while (cp1!=&fname[0] && cp1[-1]!=':' && cp1[-1]!='\\'&&cp1[-1]!='/')
                --cp1;
#endif
#if     V7 | USG | BSD
        while (cp1!=&fname[0] && cp1[-1]!='/')
                --cp1;
#endif
        cp2 = &bname[0];
        while (cp2!=&bname[NBUFN-1] && *cp1!=0 && *cp1!=';')
                *cp2++ = *cp1++;
        *cp2 = 0;
}

unqname(name)	/* make sure a buffer name is unique */

char *name;	/* name to check on */

{
	register char *sp;

	/* check to see if it is in the buffer list */
	while (bfind(name, 0, FALSE) != NULL) {

		/* go to the end of the name */
		sp = name;
		while (*sp)
			++sp;
		if (sp == name || (*(sp-1) <'0' || *(sp-1) > '8')) {
			*sp++ = '0';
			*sp = 0;
		} else
			*(--sp) += 1;
	}
}

/*
 * Ask for a file name, and write the
 * contents of the current buffer to that file.
 * Update the remembered file name and clear the
 * buffer changed flag. This handling of file names
 * is different from the earlier versions, and
 * is more compatable with Gosling EMACS than
 * with ITS EMACS. Bound to "C-X C-W".
 */
filewrite(f, n)
{
        register int    s;
        char            fname[NFILEN];

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
        if ((s=mlreply("Write file: ", fname, NFILEN)) != TRUE)
                return (s);
        if ((s=writeout(fname)) == TRUE) {
                strcpy(curbp->b_fname, fname);
                curbp->b_flag &= ~BFCHG;
                upmode();
        }
        return (s);
}

/*
 * Save the contents of the current
 * buffer in its associated file. Do nothing
 * if nothing has changed (this may be a bug, not a
 * feature). Error if there is no remembered file
 * name for the buffer. Bound to "C-X C-S". May
 * get called by "C-Z".
 */
filesave(f, n)
{
        register int    s;

	if (curbp->b_mode&MDVIEW)	/* don't allow this command if	*/
		return(rdonly());	/* we are in read only mode	*/
        if ((curbp->b_flag&BFCHG) == 0)         /* Return, no changes.  */
                return (TRUE);
        if (curbp->b_fname[0] == 0) {           /* Must have a name.    */
                mlwrite("No file name");
                return (FALSE);
        }
        /* complain about truncated files */
        if (curbp->b_flag & BFTRUNC) {
        	if (mlyesno("Truncated file..write it out") == FALSE) {
        		mlwrite("[Aborted]");
        		return(FALSE);
        	}
        }
        /* complain about narrowed buffers */
        if (curbp->b_flag & BFNAROW) {
        	if (mlyesno("Narrows buffer..write it out") == FALSE) {
        		mlwrite("[Aborted]");
        		return(FALSE);
        	}
        }
        if ((s=writeout(curbp->b_fname)) == TRUE) {
                curbp->b_flag &= ~BFCHG;
                upmode();
        }
        return (s);
}

/*
 * This function performs the details of file
 * writing. Uses the file management routines in the
 * "fileio.c" package. The number of lines written is
 * displayed. Sadly, it looks inside a LINE; provide
 * a macro for this. Most of the grief is error
 * checking of some sort.
 */
writeout(fn)
char    *fn;
{
        register int    s;
        register LINE   *lp;
        register int    nline;

#if	CRYPT
	s = resetkey();
	if (s != TRUE)
		return(s);
#endif
	/* turn off ALL keyboard translation in case we get a dos error */
	TTkclose();

        if ((s=ffwopen(fn)) != FIOSUC) {        /* Open writes message. */
		TTkopen();
                return (FALSE);
        }
	mlwrite("[Writing...]");		/* tell us were writing */
        lp = lforw(curbp->b_linep);             /* First line.          */
        nline = 0;                              /* Number of lines.     */
        while (lp != curbp->b_linep) {
                if ((s=ffputline(&lp->l_text[0], llength(lp))) != FIOSUC)
                        break;
                ++nline;
                lp = lforw(lp);
        }
        if (s == FIOSUC) {                      /* No write error.      */
                s = ffclose();
                if (s == FIOSUC) {              /* No close error.      */
                        if (nline == 1)
                                mlwrite("[Wrote 1 line]");
                        else
                                mlwrite("[Wrote %d lines]", nline);
                }
        } else                                  /* Ignore close error   */
                ffclose();                      /* if a write error.    */
	TTkopen();
        if (s != FIOSUC)                        /* Some sort of error.  */
                return (FALSE);
        return (TRUE);
}

/*
 * The command allows the user
 * to modify the file name associated with
 * the current buffer. It is like the "f" command
 * in UNIX "ed". The operation is simple; just zap
 * the name in the BUFFER structure, and mark the windows
 * as needing an update. You can type a blank line at the
 * prompt if you wish.
 */
filename(f, n)
{
        register int    s;
        char            fname[NFILEN];

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
        if ((s=mlreply("Name: ", fname, NFILEN)) == ABORT)
                return (s);
        if (s == FALSE)
                strcpy(curbp->b_fname, "");
        else
                strcpy(curbp->b_fname, fname);
        upmode();
	curbp->b_mode &= ~MDVIEW;	/* no longer read only mode */
        return (TRUE);
}

/*
 * Insert file "fname" into the current
 * buffer, Called by insert file command. Return the final
 * status of the read.
 */
ifile(fname)
char    fname[];
{
        register LINE   *lp0;
        register LINE   *lp1;
        register LINE   *lp2;
        register int    i;
        register BUFFER *bp;
        register int    s;
        register int    nbytes;
        register int    nline;
	char mesg[NSTRING];

        bp = curbp;                             /* Cheap.               */
        bp->b_flag |= BFCHG;			/* we have changed	*/
	bp->b_flag &= ~BFINVS;			/* and are not temporary*/
        if ((s=ffropen(fname,FALSE)) == FIOERR) /* Hard file open.      */
                goto out;
        if (s == FIOFNF) {                      /* File not found.      */
                mlwrite("[No such file]");
		return(FALSE);
        }
        mlwrite("[Inserting file]");

#if	CRYPT
	s = resetkey();
	if (s != TRUE)
		return(s);
#endif
	/* back up a line and save the mark here */
	curwp->w_dotp = lback(curwp->w_dotp);
	curwp->w_doto = 0;
	curwp->w_markp = curwp->w_dotp;
	curwp->w_marko = 0;

        nline = 0;
        while ((s=ffgetline()) == FIOSUC) {
                nbytes = frlen;
                if ((lp1=lalloc(nbytes)) == NULL) {
                        s = FIOMEM;             /* Keep message on the  */
                        break;                  /* display.             */
                }
		lp0 = curwp->w_dotp;	/* line previous to insert */
		lp2 = lp0->l_fp;	/* line after insert */

		/* re-link new line between lp0 and lp2 */
		lp2->l_bp = lp1;
		lp0->l_fp = lp1;
		lp1->l_bp = lp0;
		lp1->l_fp = lp2;

		/* and advance and write out the current line */
		curwp->w_dotp = lp1;
                for (i=0; i<nbytes; ++i)
                        lputc(lp1, i, fline[i]);
                ++nline;
        }
        ffclose();                              /* Ignore errors.       */
	curwp->w_markp = lforw(curwp->w_markp);
	strcpy(mesg, "[");
	if (s==FIOERR) {
		strcat(mesg, "I/O ERROR, ");
		curbp->b_flag |= BFTRUNC;
	}
	if (s == FIOMEM) {
		strcat(mesg, "OUT OF MEMORY, ");
		curbp->b_flag |= BFTRUNC;
	}
	sprintf(&mesg[strlen(mesg)], "Inserted %d line", nline);
        if (nline > 1)
		strcat(mesg, "s");
	strcat(mesg, "]");
	mlwrite(mesg);

out:
	/* advance to the next line and mark the window for changes */
	curwp->w_dotp = lforw(curwp->w_dotp);
	curwp->w_flag |= WFHARD | WFMODE;

	/* copy window parameters back to the buffer structure */
	curbp->b_dotp = curwp->w_dotp;
	curbp->b_doto = curwp->w_doto;
	curbp->b_markp = curwp->w_markp;
	curbp->b_marko = curwp->w_marko;
	curbp->b_fcol  = curwp->w_fcol;

        if (s == FIOERR)                        /* False if error.      */
                return (FALSE);
        return (TRUE);
}

#if	ACMODE
/* set CMODE based on buffer name */
acmode(f, n)
{
	char *s;
	int len;

	s = curbp->b_fname;
	if (s && strlen(s) > 1) {
		len = strlen(s) - 1;
		while (len >= 0 && s[len] != '.') len--;
		if (len >= 0 && s[len] == '.') {
			len++;
			if (s[len] == 'c' || s[len] == 'h' ||
			    s[len] == 'p' || s[len] == 'f' ||
			    s[len] == 'a')
				curbp->b_mode |= MDCMOD;
		}
	}
	return(TRUE);
}
#endif

#if	ABACKUP

/* make an autosave name */

makeasvname(tempname, fname)
char *tempname, *fname;
{
	char *i, *j;
	char c;

	j = tempname;
	for(i=tempname; (c = *i = *fname++) != '\0'; i++)
		if (c == '.') j = i;
		else if (c == '/' || c == '\\' || c == ':' || c == ']')
			j = tempname;
	if (j == tempname) j = i;
	strcpy(j, ".asv");
}

/*
 * Recover an auto-saved version of the file in the current buffer.
 */
filercover(f, n)
{
        register int    s;
        char fname[NFILEN];

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
	if (curbp->b_mode&MDVIEW)	/* don't allow this command if	*/
		return(rdonly());	/* we are in read only mode	*/
        if (curbp->b_fname[0] == 0) {           /* Must have a name.    */
                mlwrite("[No file name]");
                return (FALSE);
        }
	strcpy(fname, curbp->b_fname);
	s = readin(fname, TRUE, TRUE);
	curbp->b_flag |= BFCHG;
	return(s);
}

/*
 * Backup the contents of the current
 * buffer in its associated file. Do nothing
 * if nothing has changed (this may be a bug, not a
 * feature). Error if there is no remembered file
 * name for the buffer.  Called when an auto-save is done.
 */
fileback(f, n)
{
	char tempname[NFILEN + 4];

	if (restflag)		/* don't allow this command if restricted */
		return(resterr());
	if (curbp->b_mode&MDVIEW)	/* don't allow this command if	*/
		return(rdonly());	/* we are in read only mode	*/
        if ((curbp->b_flag&BFCHG) == 0)         /* Return, no changes.  */
                return (TRUE);
        if (curbp->b_fname[0] == 0) {           /* Must have a name.    */
                mlwrite("[No file name for auto-save]");
                return (FALSE);
        }
	makeasvname(tempname, curbp->b_fname);
	return(writeout(tempname));
}

#endif

