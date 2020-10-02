/*% cc -O -K % -o undos
 *
 * Undos - change DOS format files to Unix, etc.
 */
char ID[] =
 "Undos Rev 4-23-85 (C)Copyright Omen Technology Inc All Rights Reserved\n";
/*
 * This program and documentation may be copied, used, or modified
 *  by Professional-YAM lincesees provided these notices are not removed.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LL 1024
#define SUB 032

char Lbuf[LL];
char *Progname;
int Todos = 0;
int Tocpm = 0;
int Tomac = 0;
int Unmac = 0;
int Strip = 0;

main(argc, argv)
char **argv;
{
	Progname = *argv;
	if (! strcmp(Progname, "tocpm"))
		Todos = Tocpm = 1;
	if (! strcmp(Progname, "todos"))
		Todos = 1;
	if (! strcmp(Progname, "unmac"))
		Unmac = 1;
	if (! strcmp(Progname, "tomac"))
		Tomac = 1;

	if (! strcmp(argv[1], "-s")) {
		++Strip; --argc; ++argv;
	}


	if (argc<2 || *argv[1]== '-')
		usage();
	while (--argc >= 1)
		chngfmt(*++argv);
	exit(0);
}
usage()
{
	fprintf(stderr, ID);
	fprintf(stderr, "Usage: {undos|tounix|todos|tocpm|unmac} [-s] file ...\n");
	fprintf(stderr, "	-s Strip parity bit, ignore bytes < 007\n");
	exit(1);
}


chngfmt(name)
char *name;
{
	register c;
	register char *p;
	register n;
	register long fpos;
	struct stat st;
	FILE *fin, *fout;
	int linno = 0;
	long ftell();
	char *mktemp();
	char outnam[64];

	if (stat(name, &st)) {
		xperror(name); return;
	}
	if ((st.st_mode & S_IFMT) != S_IFREG) {
		fprintf(stderr, "%s: %s is not a regular file\n", Progname, name);
		return;
	}
	if ((fin = fopen(name, "r")) == NULL) {
		xperror(name); return;
	}
	strcpy(outnam, "undosXXXXXX");
	mktemp(outnam);
	if ((fout = fopen(outnam, "w")) == NULL) {
		xperror(outnam); exit(1);
	}

	for (;;) {
		++linno;
		for (p=Lbuf, n=LL; --n>0; ) {
ignore:
			if ((c = getc(fin)) == EOF)
				break;
			if ( !c)
				goto ignore;
			if (c < '\7' || (c & 0200)) {
				if (Strip) {
					if ((c &= 0177) < 7)
						goto ignore;
				} else
					goto thisbin; 
			}
			if (c == SUB)
				break;
			if (c == '\r' && Unmac)
				c = '\n';
			*p++ = c;
			if (c == '\n')
				break;
		}
		*p = '\0';

		if (n == 0) {
	thisbin:
			if (n) {
				fprintf(stderr, "%s: %s is a binary file", Progname, name);
				fprintf(stderr, " line=%d char =%2X\n", linno, c);
			} else
				fprintf(stderr, "%s: %s has no linefeeds: try unmac?\n", Progname, name);
			fclose(fout);
			unlink(outnam);
			return;
		}

		if (Todos) {
			if (*--p == '\n' && p[-1] != '\r') {
				*p++ = '\r'; *p++ = '\n'; *p = 0;
			}
		} else if (Tomac) {
			if (*--p == '\n') {
				if (p[-1] == '\r')
					--p;
				*p++ = '\r'; *p = 0;
			}
		} else {
			if (*--p == '\n' && *--p == '\r') {
				*p++ = '\n'; *p = 0;
			}
		}
		if (fputs(Lbuf, fout) == EOF) {
			xperror(outnam); exit(1);
		}
		switch (c) {
		case EOF:
			if (ferror(fin)) {
				xperror(name); exit(0200);
			}
		case SUB:
			if (Tocpm) {
				fpos = ftell(fout);
				do {
					putc(SUB, fout);
				} while (++fpos & 127);
			}
			fclose(fout); fclose(fin);
			sprintf(Lbuf, "mv %s %s", outnam, name);
			system(Lbuf);
			utime(name, (struct utimbuf *) &st.st_atime);
			return;
		}
	}
}

xperror(s)
char *s;
{
	register char *p;
	extern int sys_nerr;
	extern char *sys_errlist[];
	extern errno;

	if (errno >= sys_nerr)
		p = "Gloryovsky: a New Error!";
	else
		p = sys_errlist[errno];
	fprintf(stderr, "%s: %s: %s\n", Progname, s, p);
}

