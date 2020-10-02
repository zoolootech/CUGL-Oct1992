/*
	RED main program -- Full C version

	Source:  redmain.c
	Version: see SIGNON constant below.

	Written by
	
		Edward K. Ream
		166 N. Prospect
		Madison WI 53705
		(608) 257-0802


	PUBLIC DOMAIN SOFTWARE

	This software is in the public domain.

	See red.h for a disclaimer of warranties and other information.
*/
#include "red.h"

#undef SL_SETJMP
#ifdef SHERLOCK
#define SL_SETJMP(n) sl_setjmp(n)
#else
#define SL_SETJMP(n)
#endif

/*
	Define the standard signon messages.
*/
#define SIGNON	  "RED v7.0 January 18, 1990."
#define COPYRIGHT "Written by Edward K. Ream, 1983-1990."
#define RESERVED  "This is public domain software."

/*
	Declare local routines.
*/
static int	command(void);
static int	control	(char c);
static int	lookup	(char *line, char *cmnd);

/*
	Define the possible values of mode and lastmode.
*/
#define CMNDMODE 1	/* command mode		*/
#define INSMODE  2	/* insert mode		*/
#define EDITMODE 3	/* edit mode		*/
#define OVERMODE 4	/* overtype mode 	*/
#define ESCAPEMODE 5	/* escape mode		*/
#define EXITMODE 6	/* exit the editor	*/

/*
	Define internal function numbers.
*/
#define BOL	129
#define EOL	130
#define MIDDLE	133
#define KILL	134
#define SEARCH	135
#define GOTO	136
#define XCHNG	137

int def0mode;	/* initial mode and mode after commands. */
int def1mode;	/* mode after UP_INS, DOWN_INS.	*/
int def2mode;	/* mode after UP, DOWN		*/

void
main(int  argc, char **argv)
{
	int mode;	/* current mode.			*/
	int lastmode;	/* last mode (used by ESCAPEMODE).	*/
	int lastc;	/* last command	(used by AGAIN).	*/
	int holdc;	/* queued input character.		*/
	int c;		/* current input character.		*/
	char args [MAXLEN];
	char buffer [SCRNW1];
	int v;
	int x,y,topline;


	SL_INIT();
	SL_PARSE(argc, argv, "++", "--");

	TRACEPB("main",  sl_lpout();
		sl_iout(argc); sl_csout(); sl_pout(argv); sl_rpout());

	/* Ready the system module. */
	sysinit();

	/* Output from fmt module goes to screen. */
	fmtassn(FALSE);

	/* Clear filename [] for save(), resave(). */
	g_file [0] = '\0';

	/* Ready the work file if it exists. */
	if (setjmp(DISK_ERR) == ERROR) {
		/* Error in bufinit(). */
		RETURN_VOID("main");
	}
	else {
		bufinit();
	}

	/* Start off in the default mode. */
	lastmode = mode = DEFIMODE;
	def0mode = mode;
	def1mode = DEF1MODE;
	def2mode = DEF2MODE;

	/* Set tabs, clear the screen and sign on. */
	fmtset(8);
	outinit();
	outclr();
	pmtclr();
	edclr();
	edgetln();		/* Initialize window module. */
	outxy(0,SCRNL1);

	pmthold(TRUE);
	message(SIGNON);
	message(COPYRIGHT);
	message(RESERVED);
	message("");
	message("Type ^c help to print a help message.");
	message("");
	message("");
	pmthold(FALSE);

	/* Set error recovery point for auto-load code. */
	if (setjmp(DISK_ERR) == ERROR) {
		/* Error in load() in argc >= 2 case below. */
		argc = 0;
	}

	/* Never auto-load a file if a previous session was resumed. */
	if (bufchng() == TRUE) {
		/* Draw the first part of the resumed file. */
		bufgo(1);
		edclr();
		edgo(1,0);
	}
	else if (argc >= 2) {
		/* Auto-load a file from the command line. */
		pmtmode("-- loading --");
		strcpy(args, "load ");
		strcat(args, argv [1]);
		if (load(args) == TRUE) {
			syswait();
		}
		else {
			/* Preserve the signon message. */
			bufout(1,1,2);
			outxy(0,1);
		}
	}
	else {
		/* No file was loaded. Preserve the signon message. */
		bufout(1,1,2);
		outxy(0,1);
	}

	/* Set the disk error recovery point. */
	setjmp(DISK_ERR);
	SL_SETJMP("main");

	/* Disable the again command. */
	again_flag = FALSE;

	lastc = NULL;
	holdc = NULL;
	for (;;) {

		/* Update the mode on the prompt line. */
		switch(mode) {
		case EDITMODE:		pmtmode("edit:");	break;
		case INSMODE:		pmtmode("insert:");	break;
		case OVERMODE:		pmtmode("overtype:");	break;
		case ESCAPEMODE:	pmtmode("escape:");	break;
		case EXITMODE:		pmtmode("");
					outxy(0, SCRNL1);
					fmtcrlf();

					TRACE("dump", sl_dump());
					RETURN_VOID("main");
		}

		/* Get the next character. */
		if (holdc != NULL) {
			c = holdc;
			holdc = NULL;
		}
		else {
			c = syscin();
		}

		/* Substitute the last command for the AGAIN key. */
		if (c == AGAIN) {
			c = lastc;
		}

		/* Translate escapes and edit mode commands. */
		if (mode == EDITMODE || mode == ESCAPEMODE) {
			c = tolower(c);
			switch(c) {
			case ' ':	c = RIGHT;	break;
			case 'b':	c = BOL;	break;
			case 'd':	c = SCRN_DN;	break;
			case 'e':	c = EOL;	break;
			case 'h':	c = HOME_KEY;	break;
			case 'g':	c = GOTO;	break;
			case 'k':	c = KILL;	break;
			case 'm':	c = MIDDLE;	break;
			case 'p':	c = PAGE_DN;	break;
			case 'q':	c = PAGE_UP;	break;
			case 's':	c = SEARCH;	break;
			case 'u':	c = SCRN_UP;	break;
			case 'x':	c = XCHNG;	break;
			case 'z':	c = END_KEY;	break;
			case '-':
				/* Move up a half page. */
				edgo(max(0,bufln()-(SCRNL2/2)),0);
				break;
			case '+':
				/* Move down a half page. */
				edgo(min(bufmax(),bufln()+(SCRNL2/2)),0);
				break;
			default:
				break;
			}
		}

		/* Remember the what the last function was. */
		lastc = c;

		/* Restore previous mode in escape mode. */
		if (mode == ESCAPEMODE) {
			mode = lastmode;
		}

		/* Do the requested function. */
		switch(c) {

		case NULL:	break;

		case CMND:	mode = command();
				break;

		case EDIT:	lastmode = mode = EDITMODE;
				break;

		case INS:	lastmode = mode = INSMODE;
				break;

		case OVER:	lastmode = mode = OVERMODE;
				break;


		case ESCAPE:	lastmode = mode;
				mode = ESCAPEMODE;
				break;

		case UP_INS:	ednewup();
				lastmode = mode = def1mode;
				break;

		case DOWN_INS:	ednewdn();
				lastmode = mode = def1mode;
				break;

		case UP:	edup();
				mode = def2mode;
				break;

		case DOWN:	eddn();
				mode = def2mode;
				break;

		case LEFT:	edleft();	break;
		case RIGHT:	edright();	break;
		case WORD_F:	edfword();	break;
		case WORD_B:	edbword();	break;

		case JOIN:	edjoin();	break;
		case SPLIT:	edsplit();	break;
		case UNDO:	edabt();	break;
		case DEL1:	eddel();	break;
		case DEL2:	ed2del();	break;
		case ZAP:	edzap();	break;

		case VERBATIM:
			pmtmode("verbatim:");
			edins(syscin());
			break;

		case SCRN_DN:
			syswait();
			while (chkkey() == FALSE) {
				eddn();
				if (bufnrbot()) {
					eddn();
					break;
				}
			}
			break;

		case PAGE_DN:
			edgo(min(bufmax(),bufln()+(SCRNL1-outy)+SCRNL/2),0);
			break;

		case PAGE_UP:
			edgo(max(0,bufln()-outy-SCRNL/2+3),
			     0);
			break;

		case SCRN_UP:
			syswait();
			while (bufattop() == FALSE && chkkey() == FALSE) {
				edup();
			}
			break;

		case HOME_KEY:
			edgo(max(0,bufln()-outy+1),0);
			lastc = PAGE_UP;
			break;

		case END_KEY:
			edgo(min(bufmax(),bufln()+SCRNL1-outy),0);
			lastc = PAGE_DN;
			break;

		case MIDDLE:
			edgo(min(bufmax(),bufln()+SCRNL1/2-outy+1),0);
			break;

		case BOL:
			edbegin();
			lastc = HOME_KEY;
			break;

		case EOL:
			edend();
			lastc = END_KEY;
			break;

		case GOTO:
			pmtmode("goto: ");
			getcmnd(buffer);
			if(number(buffer,&v)) {
				edgo(v, 0);
			}
			break;

		case KILL:
			pmtmode("kill:");
			c = syscin();
			if (control(c)) {
				holdc = c;
			}
			else {
				edkill(c);
			}
			break;

		case SEARCH:
			pmtmode("search:");
			c = syscin();
			if (control(c)) {
				holdc = c;
			}
			else {
				edsrch(c);
			}
			break;

		case XCHNG:
			pmtmode("eXchange:");
			edchng(syscin());
			break;
			
		default:

			if (control(c)) {
				break;
			}

			if (mode == INSMODE) {
				edins(c);
			}
			else if (mode == OVERMODE) {
				edchng(c);
			}
		}
	}
}

/*
	Return TRUE if c is a control char.
*/
static int
control(char c)
{
	SL_DISABLE();

	return c != '\t' && (c >= 127 || c < 32);
}

/*
	Handle command mode.
*/
static int
command(void)
{
	int  k, v, junk;
	char c;
	char args [SCRNW1];
	char *argp, *skipbl();
	char tail[256];

	TICKB("command");

	/* Make sure the current line is saved. */
	edrepl();

	pmtmode("command: ");
	getcmnd(args);
	c = args [0];

	switch(c) {
	case EDIT:	RETURN_INT("command", EDITMODE);
	case INS:	RETURN_INT("command", INSMODE);
	case OVER:	RETURN_INT("command", OVERMODE);
	}

	if (c == '!') {
		outclr();		/* Clear the screen. */
		outxy(0, 0);		/* Home cursor. */
		sysexec(args+1);
		pmtzap();
		edclr();
		edgo(bufln(), 0);
	}
	else if (lookup(args, "g")) {
		args[0] = ' ';		/* quick fix for g<num> (no spaces) */
		if (get3args(args, &v, -1, &junk, -1, &junk, -1) == OK) {
			edgo(v, 0);
		}
	}
	else if (lookup(args,"again") || lookup(args,"a")) {
		again();
	}
	else if (lookup(args,"change") || lookup(args,"c")) {
		change(args);
	}
	else if (lookup(args,"clear")) {
		/* clear() conflicts with AZTEC func. */
		clear1();
		pmtfn();
	}
	else if (lookup(args,"copy")) {
		copy(args);
	}
	else if (lookup(args,"def0ins")) {
		def0mode = INSMODE;
	}
	else if (lookup(args,"def0over")) {
		def0mode = OVERMODE;
	}
	else if (lookup(args,"def0edit")) {
		def0mode = EDITMODE;
	}
	else if (lookup(args,"def1ins")) {
		def1mode = INSMODE;
	}
	else if (lookup(args,"def1over")) {
		def1mode = OVERMODE;
	}
	else if (lookup(args,"def1edit")) {
		def1mode = EDITMODE;
	}
	else if (lookup(args,"def2ins")) {
		def2mode = INSMODE;
	}
	else if (lookup(args,"def2over")) {
		def2mode = OVERMODE;
	}
	else if (lookup(args,"def2edit")) {
		def2mode = EDITMODE;
	}
	else if (lookup(args,"delete")) {
		delete(args);
	}
	else if (lookup(args, "exit") || lookup(args, "x")) {
		if (chkbuf() == TRUE) {
			bufend();
			RETURN_INT("command", EXITMODE);
		}
	}

#if 1 /* for debugging only */
	else if (lookup(args,"dump")) {
		bufdump();
	}
#endif

	else if (lookup(args,"extract")) {
		extract(args);
	}
	else if (lookup(args, "findr") || lookup(args, "fr")) {
		findr(args);
	}
	else if (lookup(args,"find") || lookup(args, "f")) {
		find(args);
	}
	else if (lookup(args, "help")) {
		help();
	}
	else if (lookup(args,"inject")) {
		inject(args);
	}
	else if (lookup(args,"list")) {
		list(args);
	}
	else if (lookup(args,"load") ||
		 lookup(args, "red") ||
		 lookup(args, "l")
		) {
		load(args);
		pmtfn();
	}
	else if (lookup(args,"move")) {
		move(args);
	}
	else if (lookup(args,"name")) {
		name(args);
		pmtfn();
	}
	else if (lookup(args, "noword")) {
		hasword = FALSE;
	}
	else if (lookup(args, "nowrap")) {
		haswrap = FALSE;
	}
	else if (lookup(args,"resave") || lookup(args,"r")) {
		resave();
	}
	else if (lookup(args,"rx")) {		/* RD 11/12/85 */
		if (resave()) {
			if (chkbuf() == TRUE) {
				bufend();
				RETURN_INT("command", EXITMODE);
			}
		}
	}
	else if (lookup(args,"save") || lookup(args,"sv")) {
		save();
	}
	else if (lookup(args,"svx")) {
		if (save()) {
			if (chkbuf() == TRUE) {
				bufend();
				RETURN_INT("command", EXITMODE);
			}
		}
	}
	else if (lookup(args,"search") || lookup(args, "s")) {
		search(args);
	}

#ifdef SUSPEND
	else if (lookup(args,"quit") || lookup(args, "q")) {
		/* Make sure the file is named. */
		if (g_file [0] == '\0') {
			cmndmess("File not named.");
		}
		else {
			bufsusp();
			RETURN_INT("command", EXITMODE);
		}
	}
#endif
	else if (lookup(args,"tabs")) {
		tabs(args);
	}
	else if (lookup(args, "word")) {
		hasword = TRUE;
	}
	else if (lookup(args, "wrap")) {
		haswrap = TRUE;
	}
	else if (lookup(args,"")) {
		;
	}

	/* Do not exit. */
	RETURN_INT("command", def0mode);
}

/*
	Return TRUE if line starts with command.
*/
static int
lookup(char *line, char *cmnd)
{
	SL_DISABLE();

	while(*cmnd) {
		/* Watch out:  tolower may be a macro. */
		if (tolower(*line) != *cmnd) {
			return FALSE;
		}
		else {
			line++;
			cmnd++;
		}
	}
	return	(*line == '\0' || *line == ' ' ||
		*line == '\t' || isdigit(*line));
}

/*
	Get next command into argument buffer.
*/
void
getcmnd(char *args)
{
	int i, xpos, x, y;
	int length;
	char c;

	TRACEPB("getcmnd", sl_lpout(); sl_pout(args); sl_rpout());

	/* Remember the cursor position. */
	x      = outx;
	y      = outy;
	xpos   = pmtlast();
	outxy(xpos, 0);

	length = 0;
	while ((c = syscin()) != '\r') {
		pmtupd();

		if (c == EDIT || c == INS || c == OVER) {
			args [0] = c;
			length = 1;
			break;
		}

		if ( (c == DEL1 || c == LEFT) && length > 0) {
			outxy(xpos, 0);
			outdeol();
			length--;
			/* Redraw the field. */
			for (i = 0; i < length; i++) {
				if (args [i] == '\t') {
					outchar(' ');
				}
				else {
					outchar(args [i]);
				}
			}
		}
		else if (c == UNDO) {
			outxy(xpos, 0);
			outdeol();
			length = 0;
		}
		else if (c == '\t' && length + xpos < SCRNW1) {
			args [length++] = '\t';
			outchar(' ');
		}
		else if (c < 32 || c == 127) {
			/* Ignore control characters. */
			continue;
		}
		else {
			if (length + xpos < SCRNW1) {
				args [length++] = c;
				outchar(c);
			}
		}
	}
	args [length] = '\0';

	/* Restore the cursor. */
	outxy(x, y);

	TICKX("getcmnd");
}
