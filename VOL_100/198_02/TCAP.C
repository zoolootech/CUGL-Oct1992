/*	tcap:	Unix V5, V7 and BS4.2 Termcap video driver
		for MicroEMACS
*/

#define	termdef	1			/* don't define "term" external */

#include <stdio.h>
#include	"estruct.h"
#include        "edef.h"

#if TERMCAP

#define	MARGIN	8
#define	SCRSIZ	64
#define	NPAUSE	10			/* # times thru update to pause */
#define BEL     0x07
#define ESC     0x1B

extern int      ttopen();
extern int      ttgetc();
extern int      ttputc();
extern int	tgetnum();
extern int      ttflush();
extern int      tcapclose();
extern int	tcapkopen();
extern int	tcapkclose();
extern int      tcapmove();
extern int      tcapeol();
extern int      tcapeop();
extern int      tcapbeep();
extern int	tcaprev();
extern int	tcapcres();
extern int      tcapopen();
extern int      tput();
extern char     *tgoto();
#if	COLOR
extern	int	tcapfcol();
extern	int	tcapbcol();

int isansi = 0;		/* has support for ansi colors */
int cfcolor = -1;	/* current forground color */
int cbcolor = -1;	/* current background color */
int usedcolor = 0;	/* true if used a color */
#endif

#define TCAPSLEN 315
char tcapbuf[TCAPSLEN];
char *UP, PC, *CM, *CE, *CL, *SO, *SE;

int oldwidth;
int tcaprv;

TERM term = {
	0,	/* these four values are set dynamically at open time */
	0,
	0,
	0,
	MARGIN,
	SCRSIZ,
	NPAUSE,
        tcapopen,
        tcapclose,
        tcapkopen,
        tcapkclose,
        ttgetc,
        ttputc,
        ttflush,
        tcapmove,
        tcapeol,
        tcapeop,
        tcapbeep,
        tcaprev,
        tcapcres
#if	COLOR
	, tcapfcol,
	tcapbcol
#endif
};

#define fastputc(c)	TTputc(c)

#if     V7 | USG | BSD
#undef fastputc
#define fastputc(c)        fputc((c), stdout)
#endif

tcapopen()

{
        char *getenv();
        char *t, *p, *tgetstr();
        char tcbuf[1024];
        char *tv_stype;
        char err_str[72];
	char *IS;
	char *temp;

	phrow = scrltop = scrlbot = 1000;
	tcaprv = FAILED;

        if ((tv_stype = getenv("TERM")) == NULL)
        {
                puts("Environment variable TERM not defined!");
                exit(1);
        }

        if ((tgetent(tcbuf, tv_stype)) != 1)
        {
                sprintf(err_str, "Unknown terminal type %s!", tv_stype);
                puts(err_str);
                exit(1);
        }

 
	if (*tv_stype=='v' && *(tv_stype+1)=='t' && *(tv_stype+2)!='5') {
		isvt100 = TRUE;
		/* if (*(tv_stype+2) == '2') */ vt100keys = 1;
		}
#if	COLOR
	cfcolor = -1;
	cbcolor = -1;
	isansi = isvt100;
	if (*tv_stype=='a' && *(tv_stype+1)=='n' && *(tv_stype+2)=='s')
		isansi = TRUE;
#endif

       if ((term.t_nrow=(short)tgetnum("li")-1) == -1){
               puts("termcap entry incomplete (lines)");
               exit(1);
       }
	term.t_mrow =  term.t_nrow;

       if ((term.t_ncol=(short)tgetnum("co")) == -1){
               puts("Termcap entry incomplete (columns)");
               exit(1);
       }
	term.t_mcol = term.t_ncol;
	if (isvt100 && term.t_mcol < 132) term.t_mcol = 132;
	oldwidth = term.t_ncol;

        p = tcapbuf;
        t = tgetstr("pc", &p);
        if(t)
                PC = *t;

        CL = tgetstr("cl", &p);
        CM = tgetstr("cm", &p);
        CE = tgetstr("ce", &p);
        UP = tgetstr("up", &p);
	SE = tgetstr("se", &p);
	SO = tgetstr("so", &p);
	IS = tgetstr("is", &p);
	if (SO != NULL)
		revexist = TRUE;

        if(CL == NULL || CM == NULL || UP == NULL)
        {
                puts("Incomplete termcap entry\n");
                exit(1);
        }

	if (CE == NULL)		/* will we be able to use clear to EOL? */
		eolexist = FALSE;
		
        if (p >= &tcapbuf[TCAPSLEN])
        {
                puts("Terminal description too big!\n");
                exit(1);
        }
        ttopen();
	if (IS != NULL)			/* initialize terminal */
		while(*IS) fastputc(*IS++);

	/* if ((temp = getenv("vt100key")) != NULL) vt100keys = (*temp&1); */

#if	DECEDT
	/* hack to force vt100's into keypad mode */
	if (isvt100) {
		fastputc('\033'); fastputc('=');
		}
#endif
}

tcapclose()
{
	if (isvt100)	{
		ttsetwid(oldwidth);
	}
#if	COLOR
	if (usedcolor) {
		tcapfcol(7);
		tcapbcol(0);
	}
#endif
	ttclose();
}

tcapkopen()

{
	strcpy(sres, "NORMAL");
}

tcapkclose()

{
}

tcapmove(row, col)
register int row, col;
{
	if (isvt100 && col == 0)
		if (row == phrow+1 && row != scrlbot)
			{fastputc(13); fastputc(10);}
		else if (row == phrow-1 && row+2 != scrltop)
			{fastputc(13); fastputc(27); fastputc('M');}
		else if (row == phrow)
			{fastputc(13);}
		else {
			fastputc(27); fastputc('[');
			if (row >= 9) fastputc('0' + (row+1)/10);
			if (row > 0) fastputc('0' + (row+1) % 10);
			fastputc('H');
		}
        else putpad(tgoto(CM, col, row));
	phrow = row;
}

tcapeol()
{
#if	COLOR
	tcapfcol(gfcolor);
	tcapbcol(gbcolor);
#endif
        putpad(CE);
}

tcapeop()
{
#if	COLOR
	tcapfcol(gfcolor);
	tcapbcol(gbcolor);
#endif
        putpad(CL);
	phrow = 1000;
}

tcaprev(state)		/* change reverse video status */

int state;		/* FALSE = normal video, TRUE = reverse video */

{
#if	COLOR
	/* int ftmp, btmp; */
#endif

#if	COLOR
	/* forground and background already set */
	if (usedcolor) return;
#endif
	if (state == tcaprv)	return;
	tcaprv = state;
	if (state) {
		if (SO != NULL)
			putpad(SO);
	} else	{
		if (SE != NULL)
			putpad(SE);
#if	COLOR
		/*
		ftmp = cfcolor;
		btmp = cbcolor;
		cfcolor = -1;
		cbcolor = -1;
		tcapfcol(ftmp);
		tcapbcol(btmp);
		*/
#endif
	}
}

tcapcres()	/* change screen resolution */

{
	return(TRUE);
}

spal(dummy)	/* change palette string */
char *dummy;
{
	/* Does nothing here */
}

#if	COLOR
tcapparm(n)
register int n;
{
	register q,r;

	q = n/10;
	if (q != 0)	{
		r = q/10;
		if (r != 0)	{fastputc('0' + r%10);}
		fastputc('0' + q%10);
	}
	fastputc('0' + n%10);
}

tcapfcol(color)	/* set the current output color */
int color;
{
	if (!usedcolor || color == cfcolor)	return;
	fastputc(ESC);
	fastputc('[');
	tcapparm(color + 30);
	fastputc('m');
	cfcolor = color;
}

tcapbcol(color)	/* set the current background color */
int color;
{
	if (!usedcolor || color == cbcolor)	return;
	fastputc(ESC);
	fastputc('[');
	tcapparm(color + 40);
	fastputc('m');
	cbcolor = color;
}
#endif

tcapbeep()
{
	fastputc(BEL);
}

putpad(str)
char    *str;
{
#if USG && MSC
	int cnt;
	cnt = 0;
	while(*str >= '0' && *str <= '9')
		cnt = cnt*10 + *str++ - '0';
	while(cnt-- > 0) fastputc(PC);
	while(*str != '\0') {putc(*str, stdout); str++;}
#else
	tputs(str, 1, ttputc);
#endif
}

putnpad(str, n)
char    *str;
{
	tputs(str, n, ttputc);
}


#if	FLABEL
fnclabel(f, n)		/* label a function key */

int f,n;	/* default flag, numeric argument [unused] */

{
	/* on machines with no function keys...don't bother */
	return(TRUE);
}
#endif
#else

tcaphello()
{
}

#endif

