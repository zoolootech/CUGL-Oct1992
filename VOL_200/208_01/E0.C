/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48b

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/e0.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/

/*
	FUNCTIONS: main, initialise, edit, finish, askforfile,
		   xit
	PURPOSE: initialise; process commands, finish
*/

#define DEF	1
#include "e.h"

#include <setjmp.h>
jmp_buf mainmenu;

main(argc,argv)
char **argv;
{
	int startline = 1;
	char *dig;


	while (--argc)
		if ( **++argv == '-') {
			dig= *argv+1;
			switch((int)*dig) {
			case 'a' :
				autoin= !autoin;
				break;
			case 'b' :
				backup=!backup;
				break;
			case 'l' :	/* ell */
				displaypos=!displaypos;
				break;
			case 'h' :
				blockscroll=!blockscroll;
				break;
			case 'i' :
				hilight=!hilight;
				break;
			case 't' :
				tabwidth=atoi(++dig);
				break;
			case 's' :
				trail=!trail;
				break;
			case 'v' :
				helpon=!helpon;
				break;
			case 'd' :
				strcpy(pagingdir, dig+1);
				break;
			default  :
				if (isdigit(*dig)) {
					startline = atoi(dig);
					break;
				}
				putstr("Illegal option: ");
				putstr(*argv);
				exit(1);
			}
		}
		else strcpy(filename[0] ? name : filename, *argv);

	terminit();
	keytranslate();

	for(;;) {
		initialise(startline);
		edit();
		startline=1;
	}
}

initialise(startline)
{
	/*initialisations that are done before editing each new file: */
	extern char *getwd();

	cursorx=charn=offset=lastoff=from=to=histptr=histcnt=ncommand=0;
	cursory=topline=lastread=lastl=findir=1;
	isdim=replace=repeat=blocking=blankedmess=storehist=NO;
	pfirst= -100;
	goteof=YES;
	errmess=0;

	getwd(curdir);
	initvm();

	text[0]='\0';
	cline=1;
	altered=YES;
	puttext();

	if (filename[0]) {
		clear();
		gotoxy(8,11);
		putstr("e  screen editor  version ");
		putstr(VERSION);
		putstr("  MICROLOGY 1983");
		gotoxy(20,12);
		terminal();

		while ((textfp= fopen(filename,"r")) == 0) {
			error("Can't open file");
			lastl=1;
			askforfile();
			if(!filename[0])goto newfile;
		}
		lastl=UNKNOWN;
		lastread=0;
		goteof=NO;
		if (name[0]) {
			strcpy(filename,name);
			name[0]='\0';
		}
		format(filename);
	}
newfile:
	errmess=0;
	strcpy(text,getline(startline));
	cline=loc(startline,0);
	clear();
	if (helpon) dohelp();
	putpage();
}

edit()		/*command processor*/
{
	int c;

	setjmp(mainmenu);

	for(;;) {
		goodline=cline;
		unmess();
		ptlineno(cline);
		resetcursor();
		c=getkey();
		if (errmess != 0)
			errmess=0,
			putstatusline(cline);
		ncommand++;
		storehist=YES;
		if(goabout(c) == 0) 	/* processed c */
			continue;
		switch(c) {
		case DELLEFT	:
			deletechar(-1);
			break;
		case DELRIGHT	:
			deletechar(0);
			break;
		case DELLNKEY	:
			text[0]='\0';
			crdelete(cline == lastl ? -1 : 0);
			break;
		case DELWDKEY	:
			deleteword();
			break;
		case ALTERKEY	:
			replace=YES;
			findorrep();
			break;
		case BLOCKKEY	:
			blockops();
			break;
		case RDFILEKEY	:
			putmess("File to read? ");
			scans(name,FILELEN);
			if (strlen(name) > 0) {
				readfile(name);
				putpage();
			}
			break;
		case HELPKEY	:
			(helpon = !helpon)? dohelp():unmess();
			break;
		case CR		:
			crinsert(0);
			break;
		case CRSTILL	:
			crinsert(-1);
			break;
		case ENVIRKEY   :
			envir();
			break;
		case QUITKEY	:
			if (finish()) return;
			break;
		case UNDOKEY	:
			undo();
			break;
		case TAB:
			insertchar('\t');
			break;
		case ESCKEY	:
			resetcursor();
			c=inchar();
		default		:
			insertchar(c);
			break;
		}
	}
}
/* shared code between edit() and block operation */
goabout(c)
{
	switch (c) {
	case DOWNKEY	:
		moveline(1);
		break;
	case UPKEY	:
		moveline(-1);
		break;
	case LEFTKEY	:
		movechar(-1);
		break;
	case RIGHTKEY	:
		movechar(1);
		break;
	case LEFTWKEY	:
		moveword(-1);
		break;
	case RIGHTWKEY	:
		moveword(1);
		break;
	case BOLKEY	:
		sync(0);
		break;
	case EOLKEY	:
		sync(strlen(text));
		break;
	case UPPAGE	:
		movepage(-1);
		break;
	case DOWNPAGE	:
		movepage(0);
		break;
	case HOMEKEY	:
		if (jumpline(lastl-cline)) sync(strlen(text));
		break;
	case BOFKEY	:
		if (jumpline(1-cline)) sync(0);
		break;
	case JUMPKEY	:
		{ register char *aptr; register int sign;
			putmess("Jump to? ");
			scans(ans,6); aptr= ans;
			if( *aptr == '+')sign= '+';
			else if(*aptr == '-')sign = '-';
			else sign = 0;
			if(sign)aptr++;
			to= atoi(aptr);
			if (to > 0) {
				if(sign== '+')to += cline;
				else if(sign == '-') to -= cline;
				jumpline(to-cline);
			}
			break;
		}
	case FINDKEY	:
		replace=NO;
		findorrep();
		break;
	case REPKEY	:
		repeat=YES;
		dofindrep(1);
		repeat=NO;
		break;
	default		:
		return(-1);
	}
	return(0);
}


finish()	/*return YES to edit another file; NO to return to current file
		  or don't return, but exit if finished altogther */
{
	bool abandon;
	int c;
	char tempname[FILELEN], namebak[FILELEN];

	putmess("W|rite edited text to file, |A|bandon all edits, or |R|eturn? ");
	while ( (c=getlow()) != 'w' && c != 'a' && c != 'r');
	putch(c);
	if (c == 'r') return(NO);

	abandon= c == 'a';
	if (c == 'w') {
		if (!filename[0]) {
			putmess("File to write to? ");
			scans(filename,FILELEN);
			format(filename);
			if (filename[0] <= ' ' || (!backup && !exists(filename)))
			{
				filename[0]='\0';
				return(NO);
			}
		}
		if (backup) {	/*delete old bak file*/
			retag(strcpy(namebak,filename),"BAK");
			if (checkexists(namebak) && unlink(namebak) == FAIL) {
				error("Can't delete backup file");
				return(NO);
			}
		}
		strcpy(tempname,filename); /*keep old name in 'filename'*/
		retag(tempname,"$$$"); /*new file called'.$$$'*/
		if (writefile(1,lastl,tempname,filename,YES) == FAIL) {
			unlink(tempname); return(NO);}
		/*check original file still exists - may have been deleted or
				   renamed by user */
		if (checkexists(filename)) {
			if (backup) {
				/*orig. file becomes '.bak' */
				if (rename(filename,namebak) == FAIL) {
					error("Can't rename old file to .BAK");
					goto failed;
				}
			}
			else {
				/*delete orig file*/
				if (unlink(filename) == FAIL) {
					error("Can't delete old file");
failed: 			/*if can't delete/rename old file,
					change new name to '.$$$'*/
					strcpy(filename,tempname);
					goto nowrite;
				}
			}
		}
		rename(tempname,filename); /*new file goes from '$$$' to orig name*/
	}
nowrite:
	putmess("E|xit from editor, |R|eturn to this file, or edit |A|nother file? ");
	while ( (c=getlow()) != 'e' && c!='a' && c!='r');
	putch(c);
	switch(c) {
	case 'e' :
		if (pagefd != NOFILE)
			close(pagefd),
			unlink(pagingfile);
		xit();
	case 'a' :
		if(textfp)
			fclose(textfp);
		if (pagefd != NOFILE)
			close(pagefd),
			unlink(pagingfile);
		askforfile();
		return(YES);
	case 'r' :
		if (!abandon) {
			gotoxy(WAITPOS,0);
			textfp= fopen(filename,"r");
			lastl=UNKNOWN;
			lastread=0;
			goteof=NO;
			initvm();
			strcpy(text,getline(cline));
			errmess=0;
			putstatusline(cline);
		}
		return(NO);
	}
}

xit()
{
	deleteline(0,23);
	gotoxy(0,22);
	termfini();	/*close down terminal*/
	exit(0);
}

askforfile()	/*get another file to edit into 'filename' */
{
	char *p, *index();

	for(;;) {
		printdirectory(curdir);
		putstr("Name of file to edit     |or [return] to create a new file,\n");
		putstr(" |pathname/ to change directory,\n");
		putstr(" |[escape] to exit)|          ? ");
		if (scans(filename,FILELEN) == ESCKEY) xit();
		p = filename;
		while(*p)p++;
		if(*--p == '/') {
			*p = 0;
			if(chdir(filename) != FAIL)
				strcpy(curdir,filename);
#if CPM68K
			__BDOS(13);	/*reset all drives*/
#endif
			continue;
		}
		name[0]='\0';
		return;
	}
}






