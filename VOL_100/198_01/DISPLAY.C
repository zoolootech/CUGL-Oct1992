/*
 * The functions in this file handle redisplay. There are two halves, the
 * ones that update the virtual display screen, and the ones that make the
 * physical display screen the same as the virtual display screen. These
 * functions use hints that are left in the windows by the commands.
 *
 */

#include        <stdio.h>
#include	"estruct.h"
#include        "edef.h"

typedef struct  VIDEOTAG {
        int	v_flag;                 /* Flags */
#if	COLOR
	int	v_fcolor;		/* current forground color */
	int	v_bcolor;		/* current background color */
	int	v_rfcolor;		/* requested forground color */
	int	v_rbcolor;		/* requested background color */
#endif
        char    v_text[1];              /* Screen data. */
}       VIDEO;

#define VFCHG   0x0001                  /* Changed flag			*/
#define	VFEXT	0x0002			/* extended (beyond column 80)	*/
#define	VFREV	0x0004			/* reverse video status		*/
#define	VFREQ	0x0008			/* reverse video request	*/
#define	VFCOL	0x0010			/* color change requested	*/
#define	VFSCROL	0x0020			/* cleared by scrolling		*/

#define	WFSCROL	0x0040			/* window scrolled		*/

VIDEO   **vscreen;                      /* Virtual screen. */
VIDEO   **vsave;                        /* Used for scrolling. */
#if	MEMMAP == 0
VIDEO   **pscreen;                      /* Physical screen. */
VIDEO   **psave;                        /* Used for scrolling. */
#endif

int didlins, didldel;

#define fastputc(c)	TTputc(c)

/* these declarations must match termio.c */

#if     VMS
extern char obuf[NOBUF];                /* Output buffer               */
extern int nobuf;                       /* # of bytes in above    */

#undef fastputc
#define fastputc(c)	{if (nobuf >= NOBUF) ttflush(); obuf[nobuf++] = (c);}
#endif

#if     V7 | USG | BSD
#undef fastputc
#define fastputc(c)        putc((c), stdout)
#endif

#if	(TERMCAP | VMSVT) & COLOR
extern int usedcolor;
#endif

/*
 * Initialize the data structures used by the display code. The edge vectors
 * used to access the screens are set up. The operating system's terminal I/O
 * channel is set up. All the other things get initialized at compile time.
 * The original window has "WFCHG" set, so that it will get completely
 * redrawn on the first call to "update".
 */
vtinit()
{
    register int i;
    register VIDEO *vp;
    char *malloc();

    TTopen();		/* open the screen */
    TTkopen();		/* open the keyboard */
    TTrev(FALSE);
    if (firstflag) {
	vscreen = (VIDEO **) malloc(term.t_mrow*sizeof(VIDEO *));
	vsave = (VIDEO **) malloc(term.t_mrow*sizeof(VIDEO *));
    }

    if (vscreen == NULL || vsave == NULL)
        exit(1);

#if	MEMMAP == 0
    if (firstflag) {
	pscreen = (VIDEO **) malloc(term.t_mrow*sizeof(VIDEO *));
	psave = (VIDEO **) malloc(term.t_mrow*sizeof(VIDEO *));
    }

    if (pscreen == NULL || psave == NULL)
        exit(1);
#endif

    for (i = 0; i < term.t_mrow; ++i)
        {
	if (firstflag) vp = (VIDEO *) malloc(sizeof(VIDEO)+term.t_mcol);
	else vp = vscreen[i];

        if (vp == NULL)
            exit(1);

	vp->v_flag = 0;
#if	COLOR
	vp->v_rfcolor = 7;
	vp->v_rbcolor = 0;
#endif
        vscreen[i] = vp;
#if	MEMMAP == 0
	if (firstflag) vp = (VIDEO *) malloc(sizeof(VIDEO)+term.t_mcol);
	else vp = pscreen[i];

        if (vp == NULL)
            exit(1);

	vp->v_flag = 0;
        pscreen[i] = vp;
#endif
        }
}

/*
 * Clean up the virtual terminal system, in anticipation for a return to the
 * operating system. Move down to the last line and clear it out (the next
 * system prompt will be written in the line). Shut down the channel to the
 * terminal.
 */
vttidy()
{
#if	VMSVT | TERMCAP
    ttscroll(0, term.t_nrow, 0);
#endif
    mlerase();
    movecursor(term.t_nrow, 0);
    TTflush();
    TTclose();
    TTkclose();
}

/*
 * Set the virtual cursor to the specified row and column on the virtual
 * screen. There is no checking for nonsense values; this might be a good
 * idea during the early stages.
 */
vtmove(row, col)
{
    vtrow = row;
    vtcol = col;
}

/* Write a character to the virtual screen. The virtual row and
   column are updated. If we are not yet on left edge, don't print
   it yet. If the line is too long put a "$" in the last column.
   This routine only puts printing characters into the virtual
   terminal buffers. Only column overflow is checked.
*/

vtputc(c)

int c;

{
	register VIDEO *vp;	/* ptr to line being updated */

	vp = vscreen[vtrow];

	if (c == '\t') {
		do {
			vtputc(' ');
		} while (((vtcol + taboff) % tabsize) != 0);
	} else if (vtcol >= term.t_ncol) {
		++vtcol;
		vp->v_text[term.t_ncol - 1] = '$';
	} else if ((c&0x7F) < 0x20 || (c&0x7F) == 0x7F) {
		vtputc('^');
		vtputc((c&0x7F) ^ 0x40);
	} else {
		if (vtcol >= 0)
			vp->v_text[vtcol] = c;
		++vtcol;
	}
}

/* Write a line to the virtual screen.  Does equivalent of
	for (j=0; j<llength(lp); ++j) vtputc(lgetc(lp, j));
 */

vtputl(lp)
register LINE *lp;	/* line to update */
{
	register VIDEO *vp;	/* ptr to line being updated */
	register int i, c;

	vp = vscreen[vtrow];

	for (i = 0; i < llength(lp); ++i) {
		c = lgetc(lp, i);
		if (c == '\t') {
			do {
				if (vtcol >= term.t_ncol) {
					vp->v_text[term.t_ncol - 1] = '$';
					return;
				} else {
					if (vtcol >= 0)
						vp->v_text[vtcol] = ' ';
					++vtcol;
				}
			} while (((vtcol + taboff) % tabsize) != 0);
		} else if (vtcol >= term.t_ncol) {
			vp->v_text[term.t_ncol - 1] = '$';
			return;
		} else if (c < 0x20 || c >= 0x7F) {
			if ((c&0x7F) < 0x20 || (c&0x7F) == 0x7F) {
				if (vtcol >= 0) vp->v_text[vtcol] = '^';
				++vtcol;
				if (vtcol >= term.t_ncol) {
					vp->v_text[term.t_ncol - 1] = '$';
					return;
				} else if (vtcol >= 0)
					vp->v_text[vtcol] = ((c&0x7F) ^ 0x40);
			} else if (vtcol >= 0)
				vp->v_text[vtcol] = c;
			++vtcol;
		} else {
			if (vtcol >= 0)
				vp->v_text[vtcol] = c;
			++vtcol;
		}
	}
}

/* Write a string to the virtual screen and return its length */

int vtputs(s)
register char *s;
{
	register VIDEO *vp;	/* ptr to line being updated */
	register int i;

	vp = vscreen[vtrow];

	for (i = 0; *s != '\0' && vtcol < term.t_ncol; ++i) {
		if (vtcol >= 0) vp->v_text[vtcol] = *(s++);
		++vtcol;
	}
	return(i);
}

/*
 * Erase from the end of the software cursor to the end of the line on which
 * the software cursor is located.
 */
vteeol()
{
    register VIDEO      *vp;

    vp = vscreen[vtrow];
    if (vtcol < 0) vtcol = 0;
    while (vtcol < term.t_ncol)
        vp->v_text[vtcol++] = ' ';
}

/* upscreen:	user routine to force a screen update
		always finishes complete update		*/

upscreen(f, n)

{
	update(TRUE);
	return(TRUE);
}

/*
 * Make sure that the display is right. This is a three part process. First,
 * scan through all of the windows looking for dirty ones. Check the framing,
 * and refresh the screen. Second, make sure that "currow" and "curcol" are
 * correct for the current window. Third, make the virtual and physical
 * screens the same.
 * force == HOOK like FALSE, but also does not update cursor
 *   (for edt page scrolling)
 */
update(force)

int force;	/* force update past type ahead? */

{
	register WINDOW *wp;

#if	TYPEAH
	if (force != TRUE && typahead())
		return(TRUE);
#endif
#if	VISMAC == 0
	if (force != TRUE && kbdmode == PLAY)
		return(TRUE);
#endif

	/* update any windows that need refreshing */
	wp = wheadp;
	while (wp != NULL) {
		if (wp->w_flag) {
			/* if the window has changed, service it */
			reframe(wp);	/* check the framing */
			if ((wp->w_flag & ~WFMODE) == WFEDIT)
				updone(wp);	/* update EDITed line */
			else if ((wp->w_flag & ~(WFMODE | WFMOVE)) == WFSCROL)
				updscroll(wp);
			else if (wp->w_flag & ~WFMOVE)
				updall(wp);	/* update all lines */
			if (wp->w_flag & WFMODE)
				modeline(wp);	/* update modeline */
			wp->w_flag = 0;
			wp->w_force = 0;
		}
		/* on to the next window */
		wp = wp->w_wndp;
	}

	/* recalc the current hardware cursor location */
	updpos();

#if	MEMMAP
	/* update the cursor and flush the buffers */
	movecursor(currow, curcol - lbound);
#endif

	/* check for lines to de-extend */
	upddex();

	/* if screen is garbage, re-plot it */
	if (sgarbf != FALSE)
		updgar();

	/* update the virtual screen to the physical screen */
	if (updupd(force) != ABORT) {

		/* update the cursor and flush the buffers */
#if	MEMMAP | RAINBOW
#else
		/* Fast way to move the cursor a little bit */
		if (ttrow == currow &&
	            ttcol < curcol - lbound &&
	            ttcol + 5 > curcol - lbound)
	          do {fastputc(pscreen[currow]->v_text[ttcol]);++ttcol;}
	          while (ttcol < curcol - lbound);
#endif
		if (force != HOOK)
			movecursor(currow, curcol - lbound);
	}

	if (force != HOOK) TTflush();
	return(TRUE);
}

/*	reframe:	check to see if the cursor is on in the window
			and re-frame it if needed or wanted		*/

reframe(wp)

WINDOW *wp;

{
	register LINE *lp, *slp, *blp;
	register int i;
	int top, bot, j, k, scrolled;
	VIDEO *vp;

	scrolled = 0;

	blp = wp->w_bufp->b_linep;	/* last line of buffer */

	/* if not a requested reframe, check for a needed one */
	if ((wp->w_flag & WFFORCE) == 0) {
		lp = wp->w_linep;
		for (i = 0; i < wp->w_ntrows; i++) {

			/* if the line is in the window, no reframe */
			if (lp == wp->w_dotp)
				return(TRUE);

			/* if we are at the end of the file, reframe */
			if (lp == blp)
				break;

			/* on to the next line */
			lp = lforw(lp);
		}
	}

	/* reaching here, we need a window refresh */
	i = wp->w_force;

	/* how far back to reframe? */
	if (i > 0) {		/* only one screen worth of lines max */
		if (--i >= wp->w_ntrows)
			i = wp->w_ntrows - 1;
	} else if (i < 0) {	/* negative update???? */
		i += wp->w_ntrows;
		if (i < 0)
			i = 0;
	} else
		i = wp->w_ntrows / 2;

	/* backup to new line at top of window */
	lp = wp->w_dotp;
	while (i != 0 && lback(lp) != blp) {
		--i;
		lp = lback(lp);
	}

#if	VMSVT | TERMCAP

	if (isvt100) {
		/* backup to get end of buffer on last line */
		i = wp->w_ntrows - 2;
		if (lp != blp) {
			slp = lp;
			if (wp->w_dotp != blp)
				slp = lforw(slp);
			if (slp != blp)
				while(i > 0 && lforw(slp) != blp) {
					--i;
					slp = lforw(slp);
				}
			while(i > 0 && lback(lp) != blp) {
				--i;
				lp = lback(lp);
			}
		}
	}
	
	/* check if scrolling possible */
	if (sgarbf == FALSE && isvt100 == TRUE && wp->w_ntrows > 2) {
		slp = wp->w_linep;
		i = 0;
		while (i < wp->w_ntrows && slp != lp && slp != blp)
			{++i; slp = lforw(slp);}
		if (slp != lp)	{
			slp = wp->w_linep;
			i = 0;
			while (-i < wp->w_ntrows && slp != lp && slp != blp)
				{--i; slp = lback(slp);}
		}

		scrolled = (slp == lp);

		if (slp == lp && i != 0) {
			top = wp->w_toprow + menuflag;
			bot = top + wp->w_ntrows - 1;
			ttscroll(top, bot, i);
			if (i > 0) {		/* scroll page up */
				for (j = top; j < top + i; j++) {
					vp = pscreen[j];
					psave[bot + top - j] = vp;
					vsave[bot + top - j] = vscreen[j];
					vsave[bot + top - j]->v_flag |= VFSCROL;
					for(k = 0; k < term.t_ncol; k++)
						vp->v_text[k] = ' ';
#if	(TERMCAP | VMSVT) & COLOR
					if (usedcolor) {
						vp = vscreen[j];
						vp->v_fcolor = 7;
						vp->v_bcolor = 0;
						vp->v_flag |= VFCHG | VFCOL;
					}
#endif
				}
				for (j = top + i; j <= bot; j++) {
					psave[j - i] = pscreen[j];
					vsave[j - i] = vscreen[j];
				}
			}
			else {			/* scroll page down */
				i = -i;
				for (j = bot + 1 - i; j <= bot; j++) {
					vp = pscreen[j];
					psave[bot + top - j] = vp;
					vsave[bot + top - j] = vscreen[j];
					vsave[bot + top - j]->v_flag |= VFSCROL;
					for(k = 0; k < term.t_ncol; k++)
						vp->v_text[k] = ' ';
#if	(TERMCAP | VMSVT) & COLOR
					if (usedcolor) {
						vp = vscreen[j];
						vp->v_fcolor = 7;
						vp->v_bcolor = 0;
						vp->v_flag |= VFCHG | VFCOL;
					}
#endif
				}
				for (j = top; j < bot + 1 - i; j++) {
					psave[j + i] = pscreen[j];
					vsave[j + i] = vscreen[j];
				}
			}
			for (j = top; j <= bot; j++) {
				pscreen[j] = psave[j];
				vscreen[j] = vsave[j];
			}
		}
	}
#endif

	/* and reset the current line at top of window */
	wp->w_linep = lp;
	if (scrolled) {
		wp->w_flag |= WFSCROL;
		if (wheadp->w_wndp == NULL) didlins = didldel = 0;
	}
	else wp->w_flag |= WFHARD;
	wp->w_flag &= ~WFFORCE;
	return(TRUE);
}

/*	updone:	update the current line	to the virtual screen		*/

updone(wp)

WINDOW *wp;	/* window to update current line in */

{
	register LINE *lp;	/* line to update */
	register int sline;	/* physical screen line to update */

	/* search down the line we want */
	lp = wp->w_linep;
	sline = wp->w_toprow + menuflag;
	while (lp != wp->w_dotp) {
		++sline;
		lp = lforw(lp);
	}

	/* and update the virtual line */
	vscreen[sline]->v_flag |= VFCHG;
	vscreen[sline]->v_flag &= ~VFREQ;
	taboff = wp->w_fcol;
	vtmove(sline, -taboff);
	vtputl(lp);
#if	COLOR
	vscreen[sline]->v_rfcolor = wp->w_fcolor;
	vscreen[sline]->v_rbcolor = wp->w_bcolor;
#endif
	vteeol();
	taboff = 0;
}

/*	updscroll: update scrolled lines in a window on the virtual screen */

updscroll(wp)
WINDOW *wp;	/* window to update lines in */

{
	register LINE *lp;	/* line to update */
	register int sline;	/* physical screen line to update */

	/* search down the lines, updating them */
	lp = wp->w_linep;
	sline = wp->w_toprow + menuflag;
	while (sline < wp->w_toprow + wp->w_ntrows + menuflag) {
		if (vscreen[sline]->v_flag & VFSCROL) {
			vscreen[sline]->v_flag |= VFCHG;
			vscreen[sline]->v_flag &= ~VFREQ;
			taboff = wp->w_fcol;
			vtmove(sline, -taboff);
			if (lp != wp->w_bufp->b_linep) vtputl(lp);
#if	COLOR
			vscreen[sline]->v_rfcolor = wp->w_fcolor;
			vscreen[sline]->v_rbcolor = wp->w_bcolor;
#endif
			vteeol();
		}
		if (lp != wp->w_bufp->b_linep) lp = lforw(lp);
		++sline;
	}

}

/*	updall:	update all the lines in a window on the virtual screen */

updall(wp)

WINDOW *wp;	/* window to update lines in */

{
	register LINE *lp;	/* line to update */
	register int sline;	/* physical screen line to update */

#if MENUS
	/* menu line */
	if (menuflag)	{
		vscreen[0]->v_flag |= (VFCHG | VFREQ);
		taboff = 0;
		vtmove(0, 0);
		vtputl(menuline);
#if	COLOR
		vscreen[0]->v_rfcolor = gbcolor;
		vscreen[0]->v_rbcolor = gfcolor;
#endif
		vteeol();
	}	
#endif

	/* search down the lines, updating them */
	lp = wp->w_linep;
	sline = wp->w_toprow + menuflag;
	while (sline < wp->w_toprow + wp->w_ntrows + menuflag) {

		/* and update the virtual line */
		vscreen[sline]->v_flag |= VFCHG;
		vscreen[sline]->v_flag &= ~VFREQ;
		taboff = wp->w_fcol;
		vtmove(sline, -taboff);
		if (lp != wp->w_bufp->b_linep) {
			/* if we are not at the end */
			vtputl(lp);
			lp = lforw(lp);
		}

		/* on to the next one */
#if	COLOR
		vscreen[sline]->v_rfcolor = wp->w_fcolor;
		vscreen[sline]->v_rbcolor = wp->w_bcolor;
#endif
		vteeol();
		++sline;
	}

}

/*	updpos:	update the position of the hardware cursor and handle extended
		lines. This is the only update for simple moves.	*/

updpos()

{
	register LINE *lp;
	register int c;
	register int i;
	int j, savcol, savrow;

	/* find the current row */
	lp = curwp->w_linep;
	currow = curwp->w_toprow + menuflag;
	while (lp != curwp->w_dotp) {
		++currow;
		lp = lforw(lp);
	}

	/* find the current column */
	curcol = 0;
	i = 0;
	while (i < curwp->w_doto) {
		c = lgetc(lp, i++);
		if (c == '\t')
			curcol += tabsize - (curcol % tabsize) - 1;
		else
			if ((c&0x7F) < 0x20 || (c&0x7F) == 0x7f)
				++curcol;

		++curcol;
	}

	/* if extended, flag so and update the virtual line image */
	if (curcol < curwp->w_fcol || curcol - curwp->w_fcol >= term.t_ncol-1) {
		if (hscroll) {
			curcol -= curwp->w_fcol;
			j = hjump;
			if (j <= 0) j = 1;
			if (j > term.t_ncol-1) j = term.t_ncol-1;
			if (curcol > 0) j = -j;
			while(curcol < 0 || curcol >= term.t_ncol-1) {
				curcol += j;
				curwp->w_fcol -= j;
			}
			if (curwp->w_fcol < 0) {
				curcol += curwp->w_fcol;
				curwp->w_fcol = 0;
			}
			curwp->w_flag |= WFHARD | WFMODE;
			savcol = curcol; savrow = currow;
			updall(curwp);
			modeline(curwp);
			curwp->w_flag &= ~(WFHARD | WFMODE);
			taboff = lbound = curwp->w_fcol;
			curcol = savcol+taboff; currow = savrow;
		}
		else {
			vscreen[currow]->v_flag |= (VFEXT | VFCHG);
			updext();
		}
	} else	{
		taboff = lbound = curwp->w_fcol;
	}
}

/*	upddex:	de-extend any line that derserves it		*/

upddex()

{
	register WINDOW *wp;
	register LINE *lp;
	register int i;

	wp = wheadp;

	while (wp != NULL) {
		lp = wp->w_linep;
		i = wp->w_toprow + menuflag;

		while (i < wp->w_toprow + wp->w_ntrows + menuflag) {
			if (vscreen[i]->v_flag & VFEXT) {
				if (wp != curwp || lp != wp->w_dotp ||
				   (curcol - wp->w_fcol < term.t_ncol - 1 &&
				    curcol >= wp->w_fcol)) {
					taboff = wp->w_fcol;
					vtmove(i, -taboff);
					if (lp != wp->w_bufp->b_linep)
						vtputl(lp);
					vteeol();

					/* this line no longer is extended */
					vscreen[i]->v_flag &= ~VFEXT;
					vscreen[i]->v_flag |= VFCHG;
				}
			}
			if (lp != wp->w_bufp->b_linep) lp = lforw(lp);
			++i;
		}
		/* and onward to the next window */
		wp = wp->w_wndp;
	}
}

/*	updgar:	if the screen is garbage, clear the physical screen and
		the virtual screen and force a full update		*/

updgar()

{
	register char *txt;
	register int i,j;

	for (i = 0; i < term.t_nrow; ++i) {
		vscreen[i]->v_flag |= VFCHG;
#if	REVSTA
		vscreen[i]->v_flag &= ~VFREV;
#endif
#if	COLOR
		vscreen[i]->v_fcolor = gfcolor;
		vscreen[i]->v_bcolor = gbcolor;
#endif
#if	MEMMAP == 0
		txt = pscreen[i]->v_text;
		for (j = 0; j < term.t_ncol; ++j)
			txt[j] = ' ';
#endif
	}

#if	VMSVT | TERMCAP
	ttsetwid(term.t_ncol);
	ttscroll(0, term.t_nrow, 0);
#if	DECEDT
	/* hack to force vt100's into keypad mode */
	if (isvt100) { fastputc('\033'); fastputc('='); }
#endif
#endif
	movecursor(0, 0);		 /* Erase the screen. */
	(*term.t_eeop)();
	sgarbf = FALSE;			 /* Erase-page clears */
	i = mpresf;
	mpresf = FALSE;			 /* the message area. */
#if	COLOR
	mlerase();			/* needs to be cleared if colored */
#endif
	if (i)	mlwrite("%s", mlline);
	didlins = didldel = 0;
}

/*	updupd:	update the physical screen from the virtual screen	*/

updupd(force)

int force;	/* forced update flag */

{
	register VIDEO *vp1;
	register int i;
#if	VMSVT || TERMCAP
	int n, r, j, k, cmp;

	if (didlins < 0 || didlins >= term.t_nrow) didlins = 0;
	if (didldel < 0 || didldel >= term.t_nrow) didldel = 0;
#endif

	for (i = 0; i < term.t_nrow; ++i) {
		vp1 = vscreen[i];

		/* for each line that needs to be updated*/
		if ((vp1->v_flag & VFCHG) != 0) {
#if	TYPEAH
			if (force != TRUE && typahead())
				return(ABORT);
#endif
#if	MEMMAP
			lupdate(i, vp1);
#else
#if	VMSVT || TERMCAP
			cmp = 1;
			if (isvt100 && (didldel || didlins) &&
				(vp1->v_flag & (VFREV|VFREQ)) == 0 &&
				i+2 < term.t_nrow &&
				(vscreen[i+1]->v_flag & VFCHG) != 0 &&
				(vscreen[i+2]->v_flag & VFCHG) != 0 &&
				(cmp=strncmp(vp1->v_text, pscreen[i]->v_text,
					term.t_ncol)) != 0) {
				/* check for inserted lines */
				n = i+1; j = 1;
				/* look for a match further down */
				if (didlins)
				  while (n < term.t_nrow &&
				    ((vscreen[n]->v_flag&(VFREQ|VFREV))==0)&&
				    strncmp(vscreen[n]->v_text,
				    	pscreen[n]->v_text, term.t_ncol)!=0 &&
				    (j=strncmp(vscreen[n]->v_text,
				    	pscreen[i]->v_text, term.t_ncol))!=0 &&
				    n-i < didlins)
				    	n++;
				r = i+1;
				n = n - i;
				/* see how many lines match */
				if (j == 0)
				  while (r+n < term.t_nrow &&
				    ((vscreen[r+n]->v_flag&(VFREQ|VFREV))==0)&&
				    !strncmp(vscreen[r+n]->v_text,
				    	pscreen[r]->v_text, term.t_ncol))
				    	r++;
				/* scroll the lines */
				if (j == 0 && r > i+1) {
					r = r + n - 1;
					ttscroll(i, r, -n);
					for (j = 0; j < n; j++) {
						psave[j] = pscreen[r-j];
						for (k=0; k<term.t_ncol; k++)
						    psave[j]->v_text[k] = ' ';
#if	(TERMCAP | VMSVT) & COLOR
						if (usedcolor) {
						  vscreen[i+j]->v_flag |=
								VFCHG | VFCOL;
						  vscreen[i+j]->v_fcolor = 7;
						  vscreen[i+j]->v_bcolor = 0;
						  lupdate(i+j,
							vscreen[i+j],
							pscreen[r-j]);
						}
#endif
					}
					for (j = r; j >= i+n; j--) {
						pscreen[j] = pscreen[j-n];
						vscreen[j]->v_flag &=
						  ~(VFCHG | VFCOL | VFSCROL);
					}
					for (j = 0; j < n; j++)
						pscreen[i+j] = psave[j];
				}
				else if (didldel) {
				/* check for deleted lines */
				n = i+1; j = 1;
				while (n < term.t_nrow &&
				    ((vscreen[n]->v_flag&(VFREQ|VFREV))==0)&&
				    strncmp(vscreen[n]->v_text,
				    	pscreen[n]->v_text, term.t_ncol)!=0 &&
				    (j=strncmp(vscreen[i]->v_text,
				    	pscreen[n]->v_text, term.t_ncol))!=0 &&
				    n-i < didldel)
				    	n++;
				r = i+1;
				n = n - i;
				if (j == 0)
				  while (r+n < term.t_nrow &&
				    ((vscreen[r+n]->v_flag&(VFREQ|VFREV))==0)&&
				    !strncmp(vscreen[r]->v_text,
				    	pscreen[r+n]->v_text, term.t_ncol))
				    	r++;
				if (j == 0 && r > i+1) {
					r = r + n - 1;
					ttscroll(i, r, n);
					for (j = 0; j < n; j++) {
						psave[j] = pscreen[i+j];
						for (k=0; k<term.t_ncol; k++)
						    psave[j]->v_text[k] = ' ';
#if	(TERMCAP | VMSVT) & COLOR
						if (usedcolor) {
						  vscreen[r-j]->v_flag |=
								VFCHG | VFCOL;
						  vscreen[r-j]->v_fcolor = 7;
						  vscreen[r-j]->v_bcolor = 0;
						  lupdate(r-j,
							vscreen[r-j],
							pscreen[i+j]);
						}
#endif
					}
					for (j = i; j <= r-n; j++) {
						pscreen[j] = pscreen[j+n];
						vscreen[j]->v_flag &=
						  ~(VFCHG | VFCOL | VFSCROL);
					}
					for (j = 0; j < n; j++)
						pscreen[r-j] = psave[j];
				}
				}
			}
			if (cmp) lupdate(i, vp1, pscreen[i]);
			else vp1->v_flag &= ~(VFCHG | VFCOL | VFSCROL);
#else
			lupdate(i, vp1, pscreen[i]);
#endif
#endif
		}
	}
	didlins = didldel = 0;
	return(TRUE);
}

/*	updext: update the extended line which the cursor is currently
		on at a column greater than the terminal width. The line
		will be scrolled right or left to let the user see where
		the cursor is
								*/

updext()

{
	register int rcursor;	/* real cursor location */
	register LINE *lp;	/* pointer to current line */
	int islong;

	/* calculate what column the real cursor will end up in */
	islong = (curcol >= curwp->w_fcol);
	if (islong)
		rcursor = (curcol - curwp->w_fcol - term.t_ncol) % term.t_scrsiz
				+ term.t_margin + 1;
	else	{
		rcursor = term.t_scrsiz -
				((curwp->w_fcol - curcol) % term.t_scrsiz);
		if (rcursor > curcol) rcursor = curcol;
	}
	taboff = lbound = curcol - rcursor;

	/* scan through the line outputing characters to the virtual screen */
	/* once we reach the left edge					*/
	vtmove(currow, -taboff);	/* start scanning offscreen */
	lp = curwp->w_dotp;		/* line to output */
	vtputl(lp);

	/* truncate the virtual line, restore tab offset */
	vteeol();
	taboff = 0;

	/* and put a '$' in column 1 */
	if (islong)
		vscreen[currow]->v_text[0] = '$';
}

/*
 * Update a single line. This does not know how to use insert or delete
 * character sequences; we are using VT52 functionality. Update the physical
 * row and column variables. It does try an exploit erase to end of line. The
 * RAINBOW version of this routine uses fast video.
 */
#if	MEMMAP
/*	lupdate specific code for the IBM-PC and other compatables */

lupdate(row, vp1)

int row;		/* row of screen to update */
VIDEO *vp1;		/* virtual screen image */

{
#if	COLOR
	scwrite(row, vp1->v_text, vp1->v_rfcolor, vp1->v_rbcolor);
	vp1->v_fcolor = vp1->v_rfcolor;
	vp1->v_bcolor = vp1->v_rbcolor;
#else
	if (vp1->v_flag & VFREQ)
		scwrite(row, vp1->v_text, 0, 7);
	else
		scwrite(row, vp1->v_text, 7, 0);
#endif
	/* flag this line as changed */
	vp1->v_flag &= ~(VFCHG | VFCOL | VFSCROL);

}

#else

lupdate(row, vp1, vp2)

int row;		/* row of screen to update */
VIDEO *vp1;		/* virtual screen image */
VIDEO *vp2;		/* physical screen image */

{
#if RAINBOW
/*	lupdate specific code for the DEC rainbow 100 micro	*/

    register char *cp1;
    register char *cp2;
    register int nch;

    /* since we don't know how to make the rainbow do this, turn it off */
    flags &= (~VFREV & ~VFREQ);

    cp1 = &vp1->v_text[0];                    /* Use fast video. */
    cp2 = &vp2->v_text[0];
    putline(row+1, 1, cp1);
    nch = term.t_ncol;

    do
        {
        *cp2 = *cp1;
        ++cp2;
        ++cp1;
        }
    while (--nch);
    *flags &= ~(VFCHG | VFSCROL);
#else
/*	lupdate code for all other versions		*/

	register char *cp1;
	register char *cp2;
	register char *cp3;
	register char *cp4;
	register char *cp5;
	register int nbflag;	/* non-blanks to the right flag? */
	int rev;		/* reverse video flag */
	int req;		/* reverse video request flag */


	/* set up pointers to virtual and physical lines */
	cp1 = &vp1->v_text[0];
	cp2 = &vp2->v_text[0];

#if	COLOR
	TTforg(vp1->v_rfcolor);
	TTbacg(vp1->v_rbcolor);
#endif

#if	REVSTA | COLOR
	/* if we need to change the reverse video status of the
	   current line, we need to re-write the entire line     */
	rev = (vp1->v_flag & VFREV) == VFREV;
	req = (vp1->v_flag & VFREQ) == VFREQ;
	if ((rev != req)
#if	COLOR
	    || (vp1->v_fcolor != vp1->v_rfcolor) || (vp1->v_bcolor != vp1->v_rbcolor)
#endif
#if	HP150
	/* the HP150 has some reverse video problems */
	    || req || rev
#endif
			) {
		movecursor(row, 0);	/* Go to start of line. */
		/* set rev video if needed */
		if (rev != req)
			(*term.t_rev)(req);

		/* scan through the line and dump it to the screen and
		   the virtual screen array				*/
		cp3 = &vp1->v_text[term.t_ncol];
		while (cp1 < cp3) {
			fastputc(*cp1);
			++ttcol;
			*cp2++ = *cp1++;
		}
		/* turn rev video off */
		if (rev != req)
			(*term.t_rev)(FALSE);

		/* update the needed flags */
		vp1->v_flag &= ~(VFCHG | VFSCROL);
		if (req)
			vp1->v_flag |= VFREV;
		else
			vp1->v_flag &= ~VFREV;
#if	COLOR
		vp1->v_fcolor = vp1->v_rfcolor;
		vp1->v_bcolor = vp1->v_rbcolor;
#endif
		return(TRUE);
	}
#endif

	/* advance past any common chars at the left */
	while (cp1 != &vp1->v_text[term.t_ncol] && cp1[0] == cp2[0]) {
		++cp1;
		++cp2;
	}

/* This can still happen, even though we only call this routine on changed
 * lines. A hard update is always done when a line splits, a massive
 * change is done, or a buffer is displayed twice. This optimizes out most
 * of the excess updating. A lot of computes are used, but these tend to
 * be hard operations that do a lot of update, so I don't really care.
 */
	/* if both lines are the same, no update needs to be done */
	if (cp1 == &vp1->v_text[term.t_ncol]) {
 		vp1->v_flag &= ~(VFCHG | VFSCROL);
		return(FALSE);		/* signal that nothing was done */
	}

	/* find out if there is a match on the right */
	nbflag = FALSE;
	cp3 = &vp1->v_text[term.t_ncol];
	cp4 = &vp2->v_text[term.t_ncol];

	while (cp3[-1] == cp4[-1]) {
		--cp3;
		--cp4;
		if (cp3[0] != ' ')		/* Note if any nonblank */
			nbflag = TRUE;		/* in right match. */
	}

	cp5 = cp3;

	/* Erase to EOL ? */
	if (nbflag == FALSE && eolexist == TRUE
#if	REVSTA | COLOR
		&& (req != TRUE)
#endif
		) {
		while (cp5!=cp1 && cp5[-1]==' ')
			--cp5;

		if (cp3-cp5 <= 3)		/* Use only if erase is */
			cp5 = cp3;		/* fewer characters. */
	}

	movecursor(row, cp1 - &vp1->v_text[0]);

#if	REVSTA
	TTrev(rev);
#endif

	while (cp1 != cp5) {		/* Ordinary. */
		fastputc(*cp1);
		++ttcol;
		*cp2++ = *cp1++;
	}

	if (cp5 != cp3) {		/* Erase. */
		TTeeol();
		while (cp1 != cp3)
			*cp2++ = *cp1++;
	}
#if	REVSTA
	TTrev(FALSE);
#endif
	vp1->v_flag &= ~(VFCHG | VFSCROL);	/* flag this line as updated */
	return(TRUE);
#endif
}
#endif

/*
 * Redisplay the mode line for the window pointed to by the "wp". This is the
 * only routine that has any idea of how the modeline is formatted. You can
 * change the modeline format by hacking at this routine. Called by "update"
 * any time there is a dirty window.
 */
modeline(wp)
    WINDOW *wp;
{
    register char *cp;
    register int c;
    register int n;		/* cursor position count */
    register BUFFER *bp;
    register i;			/* loop index */
    register lchar;		/* character to draw line in buffer with */
    register firstm;		/* is this the first mode? */

    n = wp->w_toprow + wp->w_ntrows + menuflag;	/* Location. */

    vscreen[n]->v_flag |= VFCHG | VFREQ | VFCOL;/* Redraw next time. */
#if	COLOR
    vscreen[n]->v_rfcolor = 0;			/* black on */
    vscreen[n]->v_rbcolor = 7;			/* white.....*/
#endif
    taboff = 0;
    vtmove(n, 0);                       	/* Seek to right line. */
    if (wp == curwp)				/* mark the current buffer */
	lchar = '=';
    else
#if	REVSTA
	if (revexist)
		lchar = ' ';
	else
#endif
		lchar = '-';

    vtputc(lchar);
    bp = wp->w_bufp;

    if ((bp->b_flag&BFCHG) != 0)                /* "*" if changed. */
        vtputc('*');
    else
        vtputc(lchar);

    if ((bp->b_flag&BFTRUNC) != 0)                /* "#" if truncated. */
        vtputc('#');
    else
        vtputc(lchar);

    if ((bp->b_flag&BFNAROW) != 0)                /* "<>" if narrowed. */
        { vtputc('<'); vtputc('>'); }
    else
        { vtputc(lchar); vtputc(lchar); }

    n  = 11;	/* =,#,*,<,>, , , ,(,),  */
    vtputc(' ');				/* Buffer name. */
    n += vtputs(PROGNAME);
    vtputc(' ');
    n += vtputs(VERSION);
    vtputc(' ');

    /* are we horizontally scrolled? */
    if (wp->w_fcol > 0) {
    	vtputc('[');
    	vtputc('<');
    	n += 4 + vtputs(itoa(wp->w_fcol));
    	vtputc(']');
    	vtputc(' ');
    }
    vtputc('(');

    /* display the modes */

	firstm = TRUE;
	for (i = 0; i < NUMMODES; i++)	/* add in the mode flags */
		if (wp->w_bufp->b_mode & (1 << i)) {
			if (firstm != TRUE)
				{++n; vtputc(' ');}
			firstm = FALSE;
			n += vtputs(modename[i]);
		}
	vtputc(')');
	vtputc(' ');

#if 0
    vtputc(lchar);
    vtputc((wp->w_flag&WFCOLR) != 0  ? 'C' : lchar);
    vtputc((wp->w_flag&WFMODE) != 0  ? 'M' : lchar);
    vtputc((wp->w_flag&WFHARD) != 0  ? 'H' : lchar);
    vtputc((wp->w_flag&WFEDIT) != 0  ? 'E' : lchar);
    vtputc((wp->w_flag&WFMOVE) != 0  ? 'V' : lchar);
    vtputc((wp->w_flag&WFFORCE) != 0 ? 'F' : lchar);
    vtputc(lchar);
    n += 8;
#endif

    vtputc(lchar);
    vtputc(lchar);
    vtputc(' ');
    n += 3;
    cp = &bp->b_bname[0];

    while ((c = *cp++) != 0)
        {
        vtputc(c);
        ++n;
        }

    vtputc(' ');
    vtputc(lchar);
    vtputc(lchar);
    n += 3;

    if (bp->b_fname[0] != 0)            /* File name. */
        {
	vtputc(' ');
	n += 2 + vtputs("File: ");

        cp = &bp->b_fname[0];

        while ((c = *cp++) != 0)
            {
            vtputc(c);
            ++n;
            }

	vtputc(' ');
        }

    while (n < term.t_ncol)             /* Pad to full width. */
        {
        vtputc(lchar);
        ++n;
        }
}

upmode()	/* update all the mode lines */

{
	register WINDOW *wp;

	wp = wheadp;
	while (wp != NULL) {
		wp->w_flag |= WFMODE;
		wp = wp->w_wndp;
	}
}

upwind()	/* force hard updates on all windows */
{
	register WINDOW *wp;

	wp = wheadp;
	while (wp != NULL) {
		wp->w_flag |= WFHARD | WFMODE;
		wp = wp->w_wndp;
	}
}

/*
 * Send a command to the terminal to move the hardware cursor to row "row"
 * and column "col". The row and column arguments are origin 0. Optimize out
 * random calls. Update "ttrow" and "ttcol".
 */
movecursor(row, col)
    {
    if (row!=ttrow || col!=ttcol)
        {
        ttrow = row;
        ttcol = col;
        TTmove(row, col);
        }
    }

/*
 * Erase the message line. This is a special routine because the message line
 * is not considered to be part of the virtual screen. It always works
 * immediately; the terminal buffer is flushed via a call to the flusher.
 */
mlerase()
    {
    int i;
    
    movecursor(term.t_nrow, 0);

    if (discmd == FALSE)
	return;

#if	COLOR
     TTforg(7);
     TTbacg(0);
#endif
    if (eolexist == TRUE)
	    TTeeol();
    else {
        for (i = 0; i < term.t_ncol - 1; i++)
            fastputc(' ');
        movecursor(term.t_nrow, 1);	/* force the move! */
        movecursor(term.t_nrow, 0);
    }
    TTflush();
    mpresf = FALSE;
    }

/*
 * Write a message into the message line. Keep track of the physical cursor
 * position. A small class of printf like format items is handled. Assumes the
 * stack grows down; this assumption is made by the "++" in the argument scan
 * loop. Set the "message line" flag TRUE.
 */

/*VARARGS*/

mlwrite(fmt, arg)
    char *fmt;		/* format string for output */
    char *arg;		/* pointer to first argument to print */
    {
    register int c;	/* current char in format string */
    register char *ap;	/* ptr to current data field */
    char *s;
    int i, f;

    /* if we are not currently echoing the command line, abort this */

    if (discmd == FALSE) {
	movecursor(term.t_nrow, 0);
	return;
    }

#if	COLOR
	TTforg(7);
	TTbacg(0);
#endif

    /* if we can not erase to end-of-line, do it manually */

    if (eolexist == FALSE && mpresf == TRUE) {
        mlerase();
        TTflush();
    }

    mllen = 0;
    movecursor(term.t_nrow, 0);
    ap = (char *) &arg;
    while ((c = *fmt++) != 0 && mllen < term.t_ncol) {
        if (c != '%') {
            mlline[mllen++] = c;
            }
        else
            {
            c = *fmt++;
            switch (c) {
                case 'd':
                    mlputi(*(int *)ap, 10);
                    ap += sizeof(int);
                    break;

                case 'o':
                    mlputi(*(int *)ap,  8);
                    ap += sizeof(int);
                    break;

                case 'x':
                    mlputi(*(int *)ap, 16);
                    ap += sizeof(int);
                    break;

                case 'D':
                    mlputli(*(long *)ap, 10);
                    ap += sizeof(long);
                    break;

                case 's':
                    s = *(char **)ap;
                    ap += sizeof(char *);
		    while (*s != '\0') mlline[mllen++] = *s++;
                    break;

		case 'f':
		    /* break it up */
		    i = *(int *) ap;
		    f = i % 100;

		    /* send out the integer portion */
		    mlputi(i / 100, 10);
		    mlline[mllen++] = '.';
		    mlline[mllen++] = (f / 10) + '0';
		    mlline[mllen++] = (f % 10) + '0';

		    ap += sizeof(int);
		    break;

                default:
		    mlline[mllen++] = c;
                }
            }
        }

    if (mllen >= term.t_ncol) mllen = term.t_ncol - 1;
    mlline[mllen] = '\0';
    ttcol += mllen;
    for(ap = mlline; *ap; ap++)
	fastputc(*ap);

    /* if we can, erase to the end of screen */

    if (eolexist == TRUE && mpresf == TRUE)
        TTeeol();
    TTflush();
    mpresf = ((mllen > 0)? TRUE: FALSE);
    }

/*
 * Force a string out to the message line regardless of the
 * current $discmd setting. This is needed when $debug is TRUE
 * and for the write-message and clear-message-line commands
 */
mlforce(s)
	char *s;	/* string to force out */
{
	register oldcmd;	/* original command display flag */

	oldcmd = discmd;	/* save the discmd value */
	discmd = TRUE;		/* and turn display on */
	mlwrite(s);		/* write the string out */
	discmd = oldcmd;	/* and restore the original setting */
}

/*
 * Write out a string. Update the physical cursor position. This assumes that
 * the characters in the string all have width "1"; if this is not the case
 * things will get screwed up a little.
 */
mlputs(s)
    char *s;
    {
    register int c;

    while ((c = *s++) != 0)
        {
        fastputc(c);
        ++ttcol;
        }
    }

/*
 * do the same except as a long integer.
 */

mlputli(l, r)
    long l;
    {
    register long q;
    int digit;

    if (l < 0)
        {
        l = -l;
        mlline[mllen++] = '-';
        }

    q = l/r;

    if (q != 0)
        mlputli(q, r);

    digit = (int)(l%r);
    mlline[mllen++] = ((digit < 10)? '0'+digit: 'A'+digit-10);
    }

/*
 * Write out an integer, in the specified radix using mlputli.
 */
mlputi(i, r)
    {
	mlputli( (long) i, r);
    }

#if RAINBOW

putline(row, col, buf)
    int row, col;
    char buf[];
    {
    int n;

    n = strlen(buf);
    if (col + n - 1 > term.t_ncol)
        n = term.t_ncol - col + 1;
    Put_Data(row, col, n, buf);
    }
#endif

