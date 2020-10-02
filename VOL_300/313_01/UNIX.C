/* $Header: /nw2/tony/src/stevie/src/RCS/unix.c,v 1.9 89/08/31 10:03:09 tony Exp $
 *
 * System-dependent routines for UNIX System V or Berkeley.
 */

#include "stevie.h"

#ifdef	TERMCAP
extern	int	ospeed;
#endif

#ifdef BSD

/*
 * We have to dork around here to avoid name space pollution on some
 * systems that define their own version of CTRL within standard
 * header files.
 */
#undef	CTRL
#include <sgtty.h>
#include <sys/time.h>
#undef	CTRL
#define	CTRL(x)	((x) & 0x1f)

#else
#include <termio.h>
#endif

#include <signal.h>

#ifdef	BSD
bool_t	got_stop = FALSE;	/* for job control */
#endif

/*
 * inchar() - get a character from the keyboard
 */
int
inchar()
{
	char	c;
#ifdef	BSD
	void	set_tty(), reset_tty(), tsig();
	int	rv;
	int	rfds;
#endif

	flushbuf();		/* flush any pending output */

	do {
#ifdef	BSD
		/*
		 * Much of the following deals with job control issues.
		 * If the editor has received a stop signal, it resets
		 * tty modes, puts the cursor at the bottom of the screen,
		 * and suspends itself. When execution resumes, the modes
		 * are restored and the screen is redrawn.
		 *
		 * The select call below is used because an interrupted
		 * read is restarted after the stop signal is received. To
		 * avoid this problem, we use select instead which will
		 * return -1 (with errno == EINTR) if a signal is received.
		 */
		do {
			if (got_stop) {
				msg("");
				reset_tty();
				signal(SIGTSTP, SIG_DFL);
				kill(getpid(), SIGTSTP);

				/* process stops here */

				set_tty();
				signal(SIGTSTP, tsig);
				screenclear();
				updatescreen();
				windgoto(Cursrow, Curscol);
				flushbuf();
				got_stop = FALSE;
			}
			rfds = 1;	/* wait for input from stdin */
			rv=select(32,&rfds,(int*)0,(int*)0,(struct timeval*)0);
			if (rv == 1 && rfds == 1)
				rv = read(0, &c, 1);
		} while (rv != 1);
#else
		while (read(0, &c, 1) != 1)
			;
#endif
	} while (c == NUL);

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
char	c;
{
	outone(c);
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

#ifdef BSD
static	struct	sgttyb	ostate;
static	struct	ltchars	oltchars;
#else
static	struct	termio	ostate;
#endif

/*
 * Go into cbreak mode
 */
void
set_tty()
{
#ifdef BSD
	struct	sgttyb	nstate;
	struct	ltchars	nltchars;

	ioctl(0, TIOCGETP, &ostate);
	nstate = ostate;
	nstate.sg_flags &= ~(XTABS|CRMOD|ECHO);
	nstate.sg_flags |= CBREAK;
	ioctl(0, TIOCSETN, &nstate);

	/*
	 * If ^Y is being used as the "delayed suspend" character, we
	 * change it to be whatever the regular suspend character is
	 * (probably ^Z). Vi needs ^Y as a command character, and the
	 * delayed suspend isn't needed here.
	 */
	ioctl(0, TIOCGLTC, &oltchars);
	nltchars = oltchars;
	if (nltchars.t_dsuspc == CTRL('Y'))
		nltchars.t_dsuspc = nltchars.t_suspc;
	ioctl(0, TIOCSLTC, &nltchars);

#ifdef	TERMCAP
	ospeed = nstate.sg_ospeed;
#endif

#else
	struct	termio	nstate;

	ioctl(0, TCGETA, &ostate);
	nstate = ostate;
	nstate.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL);
	nstate.c_cc[VMIN] = 1;
	nstate.c_cc[VTIME] = 0;
	ioctl(0, TCSETAW, &nstate);

#ifdef	TERMCAP
	ospeed = nstate.c_cflag & CBAUD;
#endif

#endif
}

/*
 * Restore original terminal modes
 */
void
reset_tty()
{
#ifdef BSD
	ioctl(0, TIOCSETP, &ostate);
	ioctl(0, TIOCSLTC, &oltchars);
#else
	ioctl(0, TCSETAW, &ostate);
#endif
}

static void
sig()
{
	signal(SIGINT, sig);
	signal(SIGQUIT, sig);

	got_int = TRUE;
}

#ifdef	BSD
static void
tsig()
{
	signal(SIGTSTP, tsig);

	got_stop = TRUE;
}
#endif

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
	P(P_LI) = Rows = 24;
#endif

	/*
	 * The code here makes sure that there isn't a window during which
	 * we could get interrupted and exit with the tty in a weird state.
	 */
	signal(SIGINT, sig);
	signal(SIGQUIT, sig);
#ifdef	BSD
	signal(SIGTSTP, tsig);
#endif

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
	char	*getenv();
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
