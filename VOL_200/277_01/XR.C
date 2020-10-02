/*
 * xr.c - remote xmodem functions for xenix/unix
 * copyright 1986 Ronald Florence
 *
 * usage: xr|xt [-ct] [-d errfile] file
 *	-c	crc (instead of checksum)
 *	-t	text mode (CR-NL <-> NL)
 *
 * To avoid overwriting existing files, 
 * a received file with the same name 
 * as an existing file is stored as fname~.
 */

#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termio.h>

#define	DEBUG		01
#define	LF		02
#define	CRC		04
#define	BSIZE		128
#define errx(m,f)	printf("%s: ", pname), \
			printf(m, f), \
			printf("\n"), \
			exit(1)

FILE	*errf;

struct	termio	old,
		new;

hangup()
{
	resetline();
	exit(1);
}


main(ac, av)
int	ac;
char	**av;
{
	char	*pname, 
		trans = (av[0][strlen(av[0]) -1] == 't'),
		*fname;
	int	c, opts = 0; 
	struct	stat	stbuf;
	extern	int	optind;
	extern	char	*optarg;
	FILE	*fp;

	pname = *av;
	while ((c = getopt(ac, av, "ctd:?")) != EOF)
		switch (c)  {
		case 't' :
			opts |= LF;
			break;
		case 'c' :
			opts |= CRC;
			break;
		case 'd' :
			opts |= DEBUG;
			if (!(errf = fopen(optarg, "w")))
				errx("can't open %s", optarg);
			setbuf(errf, NULL);
			break;
		case '?' :
			printf("usage: %s [-ct] [-d errfile] file\n", pname);
			exit(1);
		}
	if (ac == 1 || ac == optind) 
		errx("need file name", NULL);
	fname = av[optind];
	if (trans && !(fp = fopen(fname, "r"))) 
		errx("can't open %s", fname);
	if (!trans) {
		if (!access(fname, 0))
			strcat(fname, "~");
		if (!(fp = fopen(fname, "w")))
			errx("can't write %s", fname);
	}
	printf("Ready to %s %s\n", (trans) ? "send" : "receive", fname);
	if (trans) {
		stat(fname, &stbuf);
		printf("%d blocks (128 bytes/block)\n",stbuf.st_size/BSIZE+1);
	}
	printf("Ctrl-X to abort transfer\n");

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGHUP, hangup);

        ioctl(1, TCGETA, &old);
        ioctl(1, TCGETA, &new);
	fflush(stdin);
        new.c_iflag = IGNBRK|IGNPAR;
        new.c_oflag = 0;
        new.c_lflag = 0;
        new.c_cc[4] = 1;
        new.c_cflag &= ~PARENB;
        new.c_cflag |= CS8;
        ioctl(1, TCSETAW, &new);

	(trans) ? xput(fp, opts) : xget(fp, opts);
}


resetline()
{
	ioctl(1, TCSETA, &old);
}

