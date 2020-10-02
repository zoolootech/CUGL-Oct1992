/* $Header: /nw2/tony/src/stevie/src/RCS/dos.c,v 1.7 89/07/11 21:22:01 tony Exp $
 *
 * DOS System-dependent routines.
 *
 * System-specific code for MS-DOS. This has been tested with
 * MSDOS 3.3 on an AT. Also, the console driver "nansi.sys" is
 * required.
 */

#include "stevie.h"
#include <stdio.h>
#include <dos.h>
#include <signal.h>

static	char	getswitch();
static	void	setswitch();

/*
 * inchar() - get a character from the keyboard
 */
int
inchar()
{
	int	c;

	got_int = FALSE;

	for (;;beep()) {	/* loop until we get a valid character */

		flushbuf();	/* flush any pending output */

		switch (c = getch()) {
		case 0x1e:
			return K_CGRAVE;
		case 0:				/* special key */
			if (State != NORMAL) {
				c = getch();	/* throw away next char */
				continue;	/* and loop for another char */
			}
			switch (c = getch()) {
			case 0x50:
				return K_DARROW;
			case 0x48:
				return K_UARROW;
			case 0x4b:
				return K_LARROW;
			case 0x4d:
				return K_RARROW;
			case 0x52:
				return K_INSERT;
			case 0x47:
				stuffin("1G");
				return -1;
			case 0x4f:
				stuffin("G");
				return -1;
			case 0x51:
				stuffin(mkstr(CTRL('F')));
				return -1;
			case 0x49:
				stuffin(mkstr(CTRL('B')));
				return -1;
			/*
			 * Hard-code some useful function key macros.
			 */
			case 0x3b: /* F1 */
				stuffin(":N\n");
				return -1;
			case 0x54: /* SF1 */
				stuffin(":N!\n");
				return -1;
			case 0x3c: /* F2 */
				stuffin(":n\n");
				return -1;
			case 0x55: /* SF2 */
				stuffin(":n!\n");
				return -1;
			case 0x3d: /* F3 */
				stuffin(":e #\n");
				return -1;
			case 0x3e: /* F4 */
				stuffin(":rew\n");
				return -1;
			case 0x57: /* SF4 */
				stuffin(":rew!\n");
				return -1;
			case 0x3f: /* F5 */
				stuffin("[[");
				return -1;
			case 0x40: /* F6 */
				stuffin("]]");
				return -1;
			case 0x41: /* F7 - explain C declaration */
				stuffin("yyp^iexplain \033!!cdecl\n");
				return -1;
			case 0x42: /* F8 - declare C variable */
				stuffin("yyp!!cdecl\n");
				return -1;
			case 0x43: /* F9 */
				stuffin(":x\n");
				return -1;
			case 0x44: /* F10 */
				stuffin(":help\n");
				return -1;
			default:
				break;
			}
			break;

		default:
			return c;
		}
	}
}

#define	BSIZE	2048
static	char	outbuf[BSIZE];
static	int	bpos = 0;

void
flushbuf()
{
	if (bpos != 0)
		write(1, outbuf, bpos);
	bpos = 0;
}

/*
 * Macro to output a character. Used within this file for speed.
 */
#define	outone(c)	outbuf[bpos++] = c; if (bpos >= BSIZE) flushbuf()

/*
 * Function version for use outside this file.
 */
void
outchar(c)
char	c;
{
	outone(c);
}

/*
 * outstr(s) - write a string to the console
 */
void
outstr(s)
register char	*s;
{
	while (*s) {
		outone(*s++);
	}
}

void
beep()
{
	outone('\007');
}

sleep(n)
int	n;
{
	/*
	 * Should do something reasonable here.
	 */
}

void
delay()
{
	long	l;

	flushbuf();
	/*
	 * Should do something better here...
	 */
	for (l=0; l < 5000 ;l++)
		;
}

void
sig()
{
	signal(SIGINT, sig);

	got_int = TRUE;
}

static	char	schar;		/* save original switch character */

void
windinit()
{
	Columns = 80;
	P(P_LI) = Rows = 25;
	schar = getswitch();
	setswitch('/');

	signal(SIGINT, sig);
}

void
windexit(r)
int r;
{
	flushbuf();
	setswitch(schar);
	exit(r);
}

void
windgoto(r, c)
register int	r, c;
{
	r += 1;
	c += 1;

	/*
	 * Check for overflow once, to save time.
	 */
	if (bpos + 8 >= BSIZE)
		flushbuf();

	outbuf[bpos++] = '\033';
	outbuf[bpos++] = '[';
	if (r >= 10)
		outbuf[bpos++] = r/10 + '0';
	outbuf[bpos++] = r%10 + '0';
	outbuf[bpos++] = ';';
	if (c >= 10)
		outbuf[bpos++] = c/10 + '0';
	outbuf[bpos++] = c%10 + '0';
	outbuf[bpos++] = 'H';
}

FILE *
fopenb(fname, mode)
char	*fname;
char	*mode;
{
	FILE	*fopen();
	char	modestr[16];

	sprintf(modestr, "%sb", mode);
	return fopen(fname, modestr);
}

static	char
getswitch()
{
	union	REGS	inregs, outregs;

	inregs.h.ah = 0x37;
	inregs.h.al = 0;

	intdos(&inregs, &outregs);

	return outregs.h.dl;
}

static	void
setswitch(c)
char	c;
{
	union	REGS	inregs, outregs;

	inregs.h.ah = 0x37;
	inregs.h.al = 1;
	inregs.h.dl = c;

	intdos(&inregs, &outregs);
}

#define	PSIZE	128

/*
 * fixname(s) - fix up a dos name
 *
 * Takes a name like:
 *
 *	\x\y\z\base.ext
 *
 * and trims 'base' to 8 characters, and 'ext' to 3.
 */
char *
fixname(s)
char	*s;
{
	char	*strchr(), *strrchr();
	static	char	f[PSIZE];
	char	base[32];
	char	ext[32];
	char	*p;
	int	i;

	strcpy(f, s);

	for (i=0; i < PSIZE ;i++)
		if (f[i] == '/')
			f[i] = '\\';

	/*
	 * Split the name into directory, base, extension.
	 */
	if ((p = strrchr(f, '\\')) != NULL) {
		strcpy(base, p+1);
		p[1] = '\0';
	} else {
		strcpy(base, f);
		f[0] = '\0';
	}

	if ((p = strchr(base, '.')) != NULL) {
		strcpy(ext, p+1);
		*p = '\0';
	} else
		ext[0] = '\0';

	/*
	 * Trim the base name if necessary.
	 */
	if (strlen(base) > 8)
		base[8] = '\0';
	
	if (strlen(ext) > 3)
		ext[3] = '\0';

	/*
	 * Paste it all back together
	 */
	strcat(f, base);
	strcat(f, ".");
	strcat(f, ext);

	return f;
}

void
doshell(cmd)
char	*cmd;
{
	if (cmd == NULL)
		cmd = "command.com";

	system(cmd);
	wait_return();
}
