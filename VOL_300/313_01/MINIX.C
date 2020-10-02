/* $Header: /r3/tony/src/stevie/src/RCS/minix.c,v 1.6 90/01/22 19:08:35 tony Exp $
 *
 * System-dependent routines for Minix-ST
 */

#include "stevie.h"
#include <sgtty.h>
#include <signal.h>

#ifdef	TERMCAP
extern	int	ospeed;
#endif

/*
 * inchar() - get a character from the keyboard
 */
int
inchar()
{
	char	c;

	flushbuf();		/* flush any pending output */

	while (read(0, &c, 1) != 1)
		;

	got_int = FALSE;
	return c;
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
register char	c;
{
	outbuf[bpos++] = c;
	if (bpos >= BSIZE)
		flushbuf();
}

void
outstr(s)
register char	*s;
{
	while (*s) {
		outone(*s++);
	}
}

#ifdef	TERMCAP
void
outcstr(s)
char	*s;
{
	tputs(s, 1, outchar);
}
#endif

void
beep()
{
	outone('\007');
}

/*
 * remove(file) - remove a file
 */
void
remove(file)
char *file;
{
	unlink(file);
}

/*
 * rename(of, nf) - rename existing file 'of' to 'nf'
 */
void
rename(of, nf)
char	*of, *nf;
{
	unlink(nf);
	link(of, nf);
	unlink(of);
}

void
delay()
{
	/* not implemented */
}

static	struct	sgttyb	ostate;

/*
 * Go into cbreak mode
 */
void
set_tty()
{
	struct	sgttyb	nstate;

	ioctl(0, TIOCGETP, &ostate);
	nstate = ostate;
	nstate.sg_flags &= ~(XTABS|ECHO);
	nstate.sg_flags |= CBREAK;
	ioctl(0, TIOCSETP, &nstate);

#ifdef	TERMCAP
	ospeed = nstate.sg_ospeed;
#endif
}

/*
 * Restore original terminal modes
 */
void
reset_tty()
{
	ioctl(0, TIOCSETP, &ostate);
}

void
sig()
{
	signal(SIGINT, sig);
	signal(SIGQUIT, sig);

	got_int = TRUE;
}

void
windinit()
{
#ifdef	TERMCAP
	if (t_init() != 1) {
		fprintf(stderr, "unknown terminal type\n");
		exit(1);
	}
#else
	Columns = 80;
	P(P_LI) = Rows = 25;
#endif
	/*
	 * The code here makes sure that there isn't a window during which
	 * we could get interrupted and exit with the tty in a weird state.
	 */
	signal(SIGINT, sig);
	signal(SIGQUIT, sig);

	set_tty();

	if (got_int)
		windexit(0);
}

void
windexit(r)
int r;
{
	reset_tty();
	exit(r);
}

void
windgoto(r, c)
register int	r, c;
{
#ifdef	TERMCAP
	char	*tgoto();
#else
	r += 1;
	c += 1;
#endif

	/*
	 * Check for overflow once, to save time.
	 */
	if (bpos + 8 >= BSIZE)
		flushbuf();

#ifdef	TERMCAP
	outcstr(tgoto(T_CM, c, r));
#else
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
#endif
}

FILE *
fopenb(fname, mode)
char	*fname;
char	*mode;
{
	return fopen(fname, mode);
}

char *
strchr(s, c)
char	*s;
char	c;
{
	char *index();

	return index(s, c);
}

char *
fixname(s)
char	*s;
{
	return s;
}

/*
 * doshell() - run a command or an interactive shell
 */
void
doshell(cmd)
char	*cmd;
{
	char	*cp, *getenv();
	char	cline[128];

	outstr("\r\n");
	flushbuf();

	if (cmd == NULL) {
		if ((cmd = getenv("SHELL")) == NULL)
			cmd = "/bin/sh";
		sprintf(cline, "%s -i", cmd);
		cmd = cline;
	}

	reset_tty();
	system(cmd);
	set_tty();

	wait_return();
}
