/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48+

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e1.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	FUNCTIONS:  printdirectory, envir, files, putonoff, putxy, getnumb
	PURPOSE:  environment; change files.
*/

#include "e.h"


printdirectory(path)
char *path;
{
	int y, cols;
	int entriesonline = 0;
	extern char *readdir();
	register char *p;


	clear();
	if (errmess != 0) {
		gotoxy(EMPOS,0);
		putstr(errmess);
	}
	gotoxy(0,6);
	putstr("Directory of ");
	putstr(path);
	putch(':');
	if (strcmp(curdir,path) && chdir(path) != FAIL) {
		putstr("  |(working directory is ");
		putstr(curdir);
		putstr(":|)");
	}
	standout(); isdim = YES;
	deleteline(2,(y=8));

	p = readdir(path);
	/* the names are separated by a NEWLINE and
	 * terminated by a NULL
	 */
	while (*p) {
		if (inbufp) {
			putstr(" ...");
			goto incomplete;
		}
		cols = 0;
		while(*p != '\n')
			cols++, putch(*p++);
		p++;
		while (cols++ < 13) putch(' ');
		if (++entriesonline == 6) {
			deleteline(2,++y);
			if (y == SHEIGHT) {
				putstr("[||<cr>| for MORE...]");
				getkey();
				standout(); isdim = YES;
				delpage((y=8));
				gotoxy(2,8);
			}
			entriesonline=0;
		}
	}
	gotoxy(50,SHEIGHT);
	putstr("Free space = ");
	uspr(getfree(path));
incomplete:
	if (strcmp(curdir,path))
		chdir(curdir);
	standend(); isdim = NO;
	gotoxy(0,0);
}


#define XTEXT 10
#define XVAL  42
#define XPROMPT 63
#define YTITLE 4
#define YFILE 6
#define YAUTO 8
#define YBACKUP 9
#define YTAB 10
#define YSTRIP 11
#define YCURSOR 12
#define YSCROLL 13

#define YHILITE 14
#define YPROMPT 16
#define YSPACE 22
envir()
{
	puttext();
	clear();
	putstatusline(cline);

	putxy(YTITLE,"Edit context:");
	putxy(YFILE,"  |enter |F| for file context");
	putxy(YAUTO,"A|uto indent:");
	putonoff(YAUTO,autoin);
	putxy(YBACKUP,"|make |B|ackup of original file:");
	putonoff(YBACKUP,backup);
	putxy(YTAB,"T|ab stops every");
	gotoxy(XVAL,YTAB);
	uspr(tabwidth);
	putxy(YSTRIP,"S|trip trailing blanks,tabs");
	putonoff(YSTRIP,!trail);
	putxy(YCURSOR,"|display cursor |L|ine and column");
	putonoff(YCURSOR,displaypos);
	putxy(YSCROLL,"H|orizontal scroll whole screen");
	putonoff(YSCROLL,blockscroll);
	putxy(YHILITE,"|h|I|ghlight cursor line");
	putonoff(YHILITE,hilight);

	putxy(YSPACE,"|Memory allows for no more than ");
	uspr( (slotsinmem-2)*(PAGESIZE/sizeof(struct addr)) );
	putstr("| lines of text");

	putxy(YPROMPT,"Enter a letter, or [return] to exit:");
	for(;;) {
		gotoxy(XPROMPT,YPROMPT);
		switch(getlow()) {
		case 'a':
			putonoff(YAUTO,(autoin=!autoin));
			continue;
		case 'b':
			putonoff(YBACKUP,(backup=!backup));
			continue;
		case 't':
			putxy(YTAB,"|Enter new |tab width|:");
			gotoxy(XVAL,YTAB);
			putstr("    ");
			gotoxy(XVAL,YTAB);
			tabwidth=getnumb();
			putxy(YTAB,"T|ab stops every        ");
			continue;
		case 'l':
			putonoff(YCURSOR,(displaypos=!displaypos));
			continue;
		case 'h':
			putonoff(YSCROLL,(blockscroll=!blockscroll));
			continue;
		case 's':
			putonoff(YSTRIP,!(trail=!trail));
			continue;
		case 'i':
			putonoff(YHILITE,(hilight=!hilight));
			continue;
		case 'f':
			files();
		case 'q':
		case 'm':
		case ESCKEY:
			break;
		}
		break;
	}
	clear();
	topline=1;
	if (helpon) dohelp();
	putpage();
}

files()
{
	char oldname[FILELEN], newname[FILELEN], c;
	/*	int warn;	*/
	char dir[PATHLEN];
	char *p;

	strcpy(dir, curdir);
	for(;;) {
		printdirectory(dir);
		putstatusline(cline);
newcomm:
		errmess=0;
		gotoxy(0,1);
		putstr("|Enter |[return]| to exit           |D| to delete a file\n");
		putstr("      R| to rename a file         |V| to view another directory  \n");
		deleteline(0,3);
		putstr("      C| to change the name of the edited file ");
		putstr(filename);
		deleteline(0,4);
		deleteline(0,5);
		gotoxy(6,4);
		c=getlow();
		putstatusline(cline);
		deleteline(6,4);
		if (c == 'r' || c == 'd' || c == 'c') {
			putstr("Name of file: ");
			scans(oldname,FILELEN);
			if (!oldname[0]) goto newcomm;
			format(oldname);
		}
		switch (c) {
		case 'm':
		case 'q':
		case ESCKEY:
			return;
		case 'v':
			putstr("Enter pathname ending with a / ");
			scans(dir, FILELEN);
			p = dir;
			while(*p)p++;
			if(*--p != '/')
				strcpy(dir, curdir);
			else
				*p = 0;
			continue;
		case 'd':
			if (unlink(oldname) == FAIL) {
				error("Can't delete");
				goto newcomm;
			}
			break;
		case 'r' :
			deleteline(6,5);
			putstr("New name:     ");
			scans(newname,FILELEN);
			format(newname);
			if (!newname[0] || rename(oldname,newname) == FAIL) {
				error("Can't rename");
				goto newcomm;
			}
			break;
		case 'c'  :
			strcpy(filename,oldname);
			putstatusline(cline);
		default:
			goto newcomm;
		}
	}
}

putxy(row,string)
int row;
char *string;
{
	gotoxy(XTEXT,row);
	putstr(string);
}

putonoff(row,flag)
int row,flag;
{
	gotoxy(XVAL,row);
	putstr((flag?"ON ":"OFF"));
}

getnumb()
{
	int i, n, c;
	char numb[5];

	scans(numb,5);
	n=0;
	for (i=0; (c=numb[i]); i++)
		if (c >= '0' && c <= '9') n= n*10+c-'0';
	return(n);
}
