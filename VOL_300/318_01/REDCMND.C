/*
	RED command mode commands -- Full C version

	Source:  redcmnd.c
	Version: February 4, 1985; August 8, 1986; January 18, 1990

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

/*
	Declare the routines in this file.
*/
static int	srch1	(int s_start,int s_end,int r_flag,int w_flag,int c_flag);
static int	srch2	(char *old_pat,char *new_pat,int r_flag,int c_flag,int *a_flag,int s_anchor,int e_anchor,int plen);
static int	sepchar	(int c);
static void	append	(char *fname,int where,char *prompt);
static void	xtrct	(char *fname,int start,int finish,char *prompt);
static void	movecopy(char *args,char *usage,char *prompt,int erase);
static char *	skiparg	(char *args);
static char *	skipbl	(char *args);
static int	chkckey	(void);
static int	replace	(char *oldline,char *newline,char *oldpat,char *newpat,int col);

/*
	Do the last find/change/search again.
*/
void
again(void)
{
	TICKB("again");

	if (again_flag) {
		if (	a_start == bufln() &&
			a_rflag == FALSE &&
			a_wflag == TRUE &&
			a_cflag == FALSE
		) {
			if (a_rev) {
				/* Redo a findr. */
				a_start--;
			}
			else {
				/* Redo a find. */
				a_start++;
			}
		}
		else {
			a_start = bufln();
		}
		srch1(a_start, a_end, a_rflag, a_wflag, a_cflag);
	}
	else {
		cmndmess("no change/find/change active");
	}

	TICKX("again");
}

/*
	Changes lines.
*/
void
change(char *args)
{
	int  from, to, junk;

	TRACEPB("change", sl_lpout(); sl_sout(args); sl_rpout());

	/* Check the arguments. */
	if (get3args(args,&from,1,&to,HUGE,&junk,-1)==ERROR) {
		RETURN_VOID("change");
	}

	/* rflag = TRUE;  wflag = FALSE;  cflag = TRUE */
	again_flag = FALSE;
	srch1(from, to, TRUE, FALSE, TRUE);
	again_flag = TRUE;

	TICKX("change");
}

/*
	Clear main buffer and file name.
	WARNING:  clear() is an AZTEC library function.
*/
void
clear1(void)
{
	TICKB("clear1");

	/* Make sure it is ok to clear buffer. */
	if (chkbuf() == TRUE) {
		g_file [0] = 0;
		outclr();
		bufnew();
		edgetln();
		outxy(0, 1);
		bufout(1, 1, 2);
		pmtzap();		/* 1/7/84 */
	}

	TICKX("clear1");
}

/*
	Copy a block of memory without erasing it.
*/
void
copy(char *args)
{
	TRACEPB("copy", sl_lpout(); sl_sout(args); sl_rpout());

	movecopy(args, "usage: copy <block> <n>", "-- copying --", FALSE);

	TICKX("copy");
}

/*
	Delete multiple lines.
*/
void
delete(char *args)
{
	int from, to, dummy;

	TRACEPB("delete", sl_lpout(); sl_sout(args); sl_rpout());

	/* Check the request. */
	if(get3args(args,&from,bufln(),&to,-1,&dummy,-1)==ERROR){
		RETURN_VOID("delete");
	}
	if (to == -1) {
		to = from;
	}

	/* Say we've started. */
	pmtmode("-- deleting --");

	/* Go to first line to be deleted. */
	bufgo(from);

	/* Delete all line between from and to. */
	bufdeln(to-from+1);

	/* Redraw the screen. */
	bufgo(from);
	edclr();
	edgo(from, 0);

	TICKX("delete");
}

/*
	Extract command.
	Copy lines to a file.
*/
void
extract(char *args)
{
	char locfn [SYSFNMAX];
	int  from, to, junk;

	TRACEPB("extract", sl_lpout(); sl_sout(args); sl_rpout());

	/* Get file name which follows command. */
	if (name1(args, locfn) == ERROR) {
		RETURN_VOID("extract");
	}
	if (locfn [0] == '\0') {
		cmndmess("usage: extract <filename> first last");
		RETURN_VOID("extract");
	}

	/* Skip over command,  so get3args() will skip filename. */
	args = skiparg(args);
	args = skipbl (args);

	/* Get optional line numbers. */
	get3args(args, &from, bufln(), &to, -1, &junk, -1);
	if (to == -1) {
		to = from;
	}

	/* Extract the file. */
	xtrct(locfn, from, to, "-- extracting --");

	TICKX("extract");
}

/*
	Search for a pattern.
	find -- search forward.  findr -- search backwards.
*/
void
find(char *args)
{
	int  start, last, junk;

	TRACEPB("find", sl_lpout(); sl_sout(args); sl_rpout());

	/* Get starting place for search. */
	if(get3args(args,&start,bufln()+1,&last,HUGE,&junk,-1)==ERROR){
		RETURN_VOID("find");
	}

	/* rflag = FALSE;  wflag = TRUE;  cflag = FALSE */
	again_flag = FALSE;
	srch1 (start, last, FALSE, TRUE, FALSE);
	again_flag = TRUE;
	a_rev = FALSE;

	TICKX("find");
}

/*
	Same as find(),  but in reverse.
*/
void
findr(char *args)
{
	int  start, junk;

	TRACEPB("findr", sl_lpout(); sl_sout(args); sl_rpout());

	/* Get starting place for search. */
	if (get3args(args,&start,bufln()-1,&junk,-1,&junk,-1)==ERROR){
		RETURN_VOID("findr");
	}

	/* rflag = FALSE;  wflag = TRUE;  cflag = FALSE */
	again_flag = FALSE;
	srch1 (max(start,1), 1, FALSE, TRUE, FALSE);
	again_flag = TRUE;
	a_rev = TRUE;

	TICKX("findr");
}

#define MESS message

void
help(void)
{
	SL_DISABLE();

MESS  ("Command Default args   Description");
MESS  ("");
MESS  ("change 1 9999		Change all lines in <line range>");
MESS  ("clear			Reset the editor");
MESS  ("copy   <block> <n>	Copy lines in <block> after line <n>");
MESS  ("delete <current line>	Delete one or more lines");
MESS  ("exit			Exit from the editor");
MESS  ("extract <file> <block>	Create a file from a block");
MESS  ("find	<current line>	Search for a pattern");
MESS  ("findr	<current line>	Backward find");
MESS  ("g <n>			Enter edit mode at line <n>");
MESS  ("help			Print this message");
MESS  ("inject <file>           Add a line to the buffer");
MESS  ("list	1 9999		List lines to the printer");
MESS  ("load   <file>		Replace the buffer with <filename>");
MESS  ("move   <block> <n>	Move lines of <block> after line <n>");
MESS  ("name   <file>		Set filename");
MESS  ("nowrap			Disable line wrapping");
#ifdef SUSPEND
MESS  ("quit			Exit and save work file");
#endif
MESS  ("resave			Save the buffer to an existing file");
MESS  ("save			Save the buffer to a new file");
MESS  ("search	1 9999		Search for pattern");
MESS  ("tabs    8		Set tabs to every <n> columns");
MESS  ("wrap			Enable line wrapping");

	/* Wait for any character and redraw the screen. */
	pmtupd();
	syscin();
	pmtzap();
	edclr();
	edgo(bufln(), 0);

}

/*
	Inject command.
	Load a file into main buffer at current location.
	This command does NOT change the current file name.
*/
void
inject(char *args)
{
	char locfn [SYSFNMAX];
	int  oldline, junk;

	TRACEPB("inject", sl_lpout(); sl_sout(args); sl_rpout());

	/* Get file name which follows command. */
	if (name1(args, locfn) == ERROR) {
		RETURN_VOID("inject");
	}
	if (locfn [0] == '\0') {
		cmndmess("usage: inject <filename> line");
		RETURN_VOID("inject");
	}

	/* Skip over command,  so get3args() will skip filename. */
	args = skiparg(args);
	args = skipbl (args);

	/* Get optional line number. */
	get3args(args, &oldline, bufln(), &junk, -1, &junk, -1);

	/* Load the file at oldline. */
	append(locfn, oldline, "-- injecting --");

	/* Redraw the screen. */
	bufgo(oldline);
	edclr();
	edgo(oldline, 0);

	TICKX("inject");
}

/*
	Print lines to list device.
*/
void
list(char *args)
{
	char linebuf [MAXLEN1];
	int n;
	int from, to, dummy, line, oldline;

	TRACEPB("list", sl_lpout(); sl_sout(args); sl_rpout());

	/* Save the buffer's current line. */
	oldline = bufln();

	/* Get starting, ending lines to print. */
	if (get3args(args,&from,1,&to,HUGE,&dummy,-1)==ERROR) {
		RETURN_VOID("list");
	}

	/* Say we've started. */
	pmtmode("-- listing --");

	/* Print lines one at a time to list device. */
	line = from;
	while (line <= to) {

		/* Make sure prompt goes to console. */
		fmtassn(FALSE);

		/* Check for interrupt. */
		if (chkckey() == TRUE) {
			break;
		}

		/* Print line to list device. */
		fmtassn(TRUE);

		bufgo(line++);
		if (bufatbot()) {
			break;
		}
		n = bufgetln(linebuf,MAXLEN1);
		n = min(n,MAXLEN);
		linebuf [n] = '\n';
		fmtsout(linebuf,0);
		fmtcrlf();
	}

	/* Redirect output to console. */
	fmtassn(FALSE);

	/* Restore cursor. */
	bufgo(oldline);

	TICKX("list");
}

/*
	Load file into buffer.
	Return TRUE if the file was loaded correctly.
*/
int
load (char *args)
{
	char buffer [MAXLEN];	/* disk line buffer */
	char locfn  [SYSFNMAX];  /* file name */
	int n;
	int topline;

	TRACEPB("load", sl_lpout(); sl_sout(args); sl_rpout());

	/* Get filename following command. */
	if (name1(args,locfn) == ERROR) {
		RETURN_INT("load", FALSE);
	}

	if (locfn [0] == '\0') {
		cmndmess("No file argument.");
		RETURN_INT("load", FALSE);
	}

	/* Give user a chance to save the buffer. */
	if (chkbuf() == FALSE) {
		RETURN_INT("load", FALSE);
	}

	/* Open the new file. */
	if (sysexists(locfn) == FALSE) {
		cmndmess("File not found.");
		RETURN_INT("load", FALSE);
	}

	/* Say we've started. */
	pmtmode("-- loading --");

	/* Update file name. */
	syscopfn(locfn, g_file);
	pmtfn();
	pmtupd();

	/* Clear the buffer. */
	bufnew();

	/* Read the whole file into the buffer. */
	read_file(g_file);

	/* Indicate that the buffer is fresh. */
	bufsaved();

	/*	Set current line to line 1.
		Redraw the screen.
	*/
	bufgo(1);
	edclr();
	edgo (1, 0);

	again_flag = FALSE;

	RETURN_INT("load", TRUE);
}

/*
	Move a block of lines.
*/
void
move(char *args)
{
	TRACEPB("move", sl_lpout(); sl_sout(args); sl_rpout());

	/* Copy a block,  then delete it. */
	movecopy(args, "usage: move <block> <n>", "-- moving --", TRUE);

	TICKX("move");
}

/*
	Change current file name.
*/
void
name(char *args)
{
	TRACEPB("name", sl_lpout(); sl_sout(args); sl_rpout());

	name1(args, g_file);

	TICKX("name");
}

static int
name1(char *args, char *filename)
{
	TRACEPB("name1",  sl_lpout();
		sl_sout(args);     sl_csout();
		sl_sout(filename); sl_rpout());

	/* Skip command. */
	args = skiparg(args);
	args = skipbl(args);

	/* Copy filename. */
	syscopfn(args, filename);

	RETURN_INT("name1", OK);
}

/*
	Save the buffer in an already existing file.
*/
int		/* 11/11/85 RD: make resave return success (TRUE/FALSE) */
resave(void)
{
	int n, oldline;

	TICKB("resave");

	/* Save line number. */
	oldline = bufln();

	/* Make sure file has a name. */
	if (g_file [0] == '\0') {
		cmndmess("File not named.");
		RETURN_INT("resave", FALSE);
	}

	/* The file must exist for resave. */
	if (sysexists(g_file) == FALSE) {
		cmndmess("File not found.");
		RETURN_INT("resave", FALSE); 
	}

	/* Say we've started. */
	pmtmode("-- resaving --");

	/* Write out the whole buffer. */
	write_file(g_file);

	/* Indicate that the buffer has been saved. */
	bufsaved();

	/* Restore line number. */
	bufgo(oldline);

	RETURN_INT("resave", TRUE);
}

/*
	Save the buffer in a new file.
	Return TRUE if all went well.
*/
int
save(void)
{
	int file, n, oldline;

	TICKB("save");

	/* Save current line number. */
	oldline = bufln();

	/* Make sure the file is named. */
	if (g_file [0] == '\0') {
		cmndmess("File not named.");
		RETURN_INT("save", FALSE);
	}

	/* File must NOT exist for save. */
	if (sysexists(g_file) == TRUE) {
		cmndmess("File exists.");
		RETURN_INT("save", FALSE);
	}

	/* Say we've started. */
	pmtmode("-- saving --");

	/* Write out the whole buffer. */
	write_file(g_file);

	/* Indicate buffer saved. */
	bufsaved();

	/* Restore line number. */
	bufgo(oldline);

	RETURN_INT("save", TRUE);
}

/*
	Search for a pattern.
*/
void
search(char *args)
{
	int from, to, junk;

	TRACEPB("search", sl_lpout(); sl_sout(args); sl_rpout());

	/* Check the request. */
	if (get3args(args,&from,1,&to,HUGE,&junk,-1)==ERROR) {
		RETURN_VOID("search");
	}

	/* r_flag = FALSE,  w_flag = FALSE, c_flag = TRUE. */
	again_flag = FALSE;
	srch1(from, to, FALSE, FALSE, TRUE);
	again_flag = TRUE;

	TICKX("search");
}

/*
	Search/change utility routine.
	Redraw the screen if the pattern is found.

	s_pat[]   contains the search pattern.
	r_pat[]   contains the change pattern.
	s_start   is first line to search.
	s_end     is last  line to search.
	r_flag    is TRUE if change pattern is used.
	w_flag    is TRUE if search wraps around.
	c_flag    is TRUE if search continues after a match.
*/
static int
srch1(int s_start, int s_end, int r_flag, int w_flag, int c_flag)
{
	char s_pat [MAXLEN1];
	char r_pat [MAXLEN1];

	int oldbuf;
	int oldstart;
	int a_flag, old_plen;
	int start_anchor,  end_anchor;
	int code;

	TRACEPB("srch1",  sl_lpout();
		sl_iout(s_start); sl_csout();
		sl_iout(s_end);   sl_csout(); 
		sl_iout(r_flag);  sl_csout();
		sl_iout(w_flag);  sl_csout(); 
		sl_iout(c_flag);  sl_rpout());

	/* Save again parameters. */
	a_start = s_start;
	a_end   = s_end;
	a_rflag = r_flag;
	a_wflag = w_flag;
	a_cflag = c_flag;

	if (again_flag == FALSE) {
		/* Get search mask. */
		pmtmode("Search mask? ");
		getcmnd(s_pat);
		if (strlen(s_pat) == 0) {
			RETURN_VOID("srch1");
		}
		strcpy(a_spat, s_pat);
	
		if (r_flag) {
			pmtmode("Change mask? ");
			getcmnd(r_pat);
			strcpy(a_rpat, r_pat);
		}
	}
	else {
		strcpy(s_pat, a_spat);
		strcpy(r_pat, a_rpat);
	}

	/* Remember the current line. */
	oldbuf = bufln();

	/* Go to first line. */
	bufgo(s_start);
	if (bufatbot()) {
		bufup();
	}

	/* Remember the initial params. */
	oldstart = bufln();
	a_start  = s_start  = bufln();
	a_end    = s_end;
	old_plen = strlen(s_pat);

	/* Set start-of-line anchor. */
	start_anchor = (s_pat [0] == '^') ? 1 : 0;

	/* Set end-of-line anchor. */
	end_anchor = (s_pat [old_plen - 1] == '$') ? 1 : 0;

	/* Delete trailing anchor from search pattern. */
	s_pat [old_plen - end_anchor] = '\0';

	/* Adjust plen to reflect only non-anchor characters. */
	old_plen -= (start_anchor + end_anchor);

	/* Enable prompts in srch2(). */
	a_flag = 0;

	pmtmode("-- searching --");

	/* Search all lines in between s_start and s_end. */
	if (s_start <= s_end) {
		while (s_start <= s_end) {

			/* Check for user abort. */
			if (chkckey() == TRUE) {
				break;
			}

			code = srch2( s_pat, r_pat, r_flag,
			              c_flag, &a_flag, 
				      start_anchor,  end_anchor,
				      old_plen);

			if (code == TRUE) {
				RETURN_VOID("srch1");
			}
			else if (code == FALSE) {
				/* Remember the last match point. */
				oldbuf = bufln();
				pmtmode("-- searching --");
			}

			if (bufnrbot() && w_flag == FALSE) {
				break;
			}
			else if (bufnrbot() && w_flag == TRUE) {
				/* Wrap around search. */
				w_flag = FALSE;
				bufgo(1);
				a_start = s_start = 1;
				a_end   = s_end   = oldstart;
			}
			else {
				bufgo(++s_start);
				a_start++;
			}

		}

		/*  Return to the last line with a match. */
		if (a_flag == 'a') {	/* 4/24/84 */
			/* Force redraw. */
			edclr();
		}
		edgo(oldbuf, 0);
		RETURN_VOID("srch1");
	}
	else {
		a_end = s_end = max(1, s_end);
		while(s_start >= s_end) {

			/* Check for user abort. */
			if (chkckey() == TRUE) {
				break;
			}

			code = srch2( s_pat, r_pat, r_flag,
			              c_flag, &a_flag,
				      start_anchor,  end_anchor,
				      old_plen);

			if (code == TRUE) {
				RETURN_VOID("srch1");
			}
			else if (code == FALSE) {
				oldbuf = bufln();
			}

			if (bufln() == 1 && w_flag == FALSE) {
				break;
			}
			else if (bufln() == 1 && w_flag == TRUE) {
				w_flag = FALSE;
				bufgo(HUGE);
				bufup();
				a_start = s_start = bufln();
				a_end   = s_end   = oldstart;
			}
			else {
				bufgo(--s_start);
				a_start--;
			}
		}

		/* Return to the last line that matched. */
		if (a_flag == 'a') {	/* 4/25/84 */
			/* Force redraw of screen. */
			edclr();
		}
		edgo(oldbuf, 0);
		RETURN_VOID("srch1");
	}
}

/*
	Search one line for all instances of old_pat.
	If r_flag is TRUE, replace them by new_pat.
	If c_flag is FALSE,  exit after finding the first instance.
	If a_flag is 'a', do not prompt the user.
	plen == strlen(old_pat).

	Return TRUE   if search should stop.
	Return No    if search should continue.
	Return ERROR if no match found.
*/
static int
srch2(
	char *old_pat, char *new_pat,
	int  r_flag, int c_flag, int *a_flag,
	int  s_anchor, int e_anchor, int plen)
{
	char old_line [MAXLEN1];
	char new_line [MAXLEN1];
	int  old_length, col, mode, match, match1, xpos;
	char * line, * pat;

	TRACEPB("srch2",  sl_lpout();
		sl_sout(old_pat);  sl_csout(); 
		sl_sout(new_pat);  sl_csout();
		sl_iout(r_flag);   sl_csout(); 
		sl_iout(c_flag);   sl_csout();
		sl_pout(a_flag);   sl_csout(); 
		sl_iout(s_anchor); sl_csout(); 
		sl_iout(e_anchor); sl_csout();
		sl_iout(plen);     sl_rpout());

	/* Get the current line into oldline[] */
	old_length = bufgetln(old_line, MAXLEN1);
	old_length = min(old_length, MAXLEN);
	old_line [old_length] = '\0';

	/* No match is possible if old_pat[] is too long. */
	if (s_anchor && e_anchor && plen != old_length) {
		RETURN_INT("srch2", ERROR);
	}
	else if (plen > old_length) {
		RETURN_INT("srch2", ERROR);
	}

	/* Set starting column. */
	col = (e_anchor) ? old_length - plen : 0;

	/* Set prompting mode. */
	mode = * a_flag;

	/* Remember whether any match was seen on this line. */
	match = FALSE;

	/* Search column by column. */
	while (col < old_length) {
		line = old_line + col;
		pat = old_pat + s_anchor;

		/* Check for beginning of a word if required. */
		if (hasword == FALSE || sepchar(*(line-1)) || col == 0) {

			/* Check character matches and keep advancing if so. */
			match1 = TRUE;
			while(*pat != '\0') {
				if (*pat == *line) {
					pat++;
					line++;
				}
				else if (*pat == '?' && *line != '\0') {
					pat++;
					line++;
				}
				else {
					match1 = FALSE;
					break;
				}
			}

			/* Check for end of word if required. */
			match1 = match1 && (hasword == FALSE || sepchar(*line));
		}
		else {
			match1 = FALSE;
		}

		if (!match1) {
			if (s_anchor || e_anchor) {
				RETURN_INT("srch2", ERROR);
			}
			else {
				col++;
				continue;
			}
		}
		else {
			/* Remember that a match was seen. */
			match = TRUE;
		}

		/* Show the screen before any replacement. */
		if (mode != 'a') { 	/* 3/1/84 */
			edgo(bufln(), col);
			syswait();
			xpos = outx;
		}

		/* Draw the proposed change on the screen. */
		if (r_flag == TRUE) {
			replace(old_line, new_line,
				old_pat + s_anchor, new_pat, col);
			bufrepl(new_line, strlen(new_line));
			if (mode != 'a') {	/* 3/1/84 */
				outxy(0, outy);
				bufoutln(bufln());
				outxy(xpos, outy);
			}
		}

		/* Stop the search if continue flag is FALSE. */
		if (c_flag == FALSE) {
			RETURN_INT("srch2", TRUE);
		}

		/* Prompt the user unless in 'all' mode. */
		if (mode == 'a') {
			/* Update the search line. */
			old_length = bufgetln(old_line, MAXLEN1);
			old_length = min(old_length, MAXLEN);
			old_line [old_length] = '\0';
			col     += strlen(new_pat);
		}
		else if (r_flag == FALSE) {
			/* Give search-mode prompt. */
			pmtmode("next, exit? ");
			mode = syscin();
			mode = tolower(mode);
			if (mode != 'n' && mode != 'y') {
				RETURN_INT("srch2", TRUE);
			}
			else {
				/* Do not rescan matched pattern. */
				col += plen;
			}
		}
		else {
			/* Give change-mode prompt. */
			pmtmode("yes, no, all, exit? ");
			mode = syscin();
			mode = tolower(mode);
			*a_flag = mode;

			if (mode == 'y' || mode == 'a') {
				/* Update the search pattern. */
				old_length = bufgetln(old_line, MAXLEN1);
				old_length = min(old_length, MAXLEN);
				old_line [old_length] = '\0';
			}
			else {
				/* Undo the change. */
				bufrepl(old_line, strlen(old_line));
				outxy(0, outy);
				bufoutln(bufln());
				outxy(xpos, outy);
			}
	
			if (mode == 'y' || mode == 'a') {
				/* Do not rescan replacement text. */
				col += strlen(new_pat);
			}
			else if (mode == 'n') {
				/* Continue the search on this line. */
				col += strlen(old_pat);
			}
			else {
				/* Default is 'e' */
				RETURN_INT("srch2", TRUE);
			}
		}

		/* Anchored searches examine line only once. */
		if (s_anchor || e_anchor) {
			RETURN_INT("srch2", FALSE);
		}
	}

	/* Indicate whether any match was found on the line. */

	RETURN_INT("srch2", (match) ? FALSE : ERROR);
}

/*
	Check for a separator character.
	Return TRUE if the character cannot be part of a "word."
	Here "word" is taken to mean "C identifier."
*/
static int
sepchar(int c)
{
	return (isalpha(c) || isdigit(c) || c == '_') ? FALSE : TRUE;
}

/*
	Set tab stops for fmt routines.
*/
void
tabs(char *args)
{
	int n, junk;

	TRACEPB("tabs", sl_lpout(); sl_sout(args); sl_rpout());

	/* Default is every 8 columns. */
	if (get3args(args,&n,8,&junk,-1,&junk,-1)==ERROR){
		RETURN_VOID("tabs");
	}
	fmtset(n);

	/* Redraw the screen. */
	edclr();
	edgo(bufln(), 0);

	TICKX("tabs");
}

/*
	----- Utility routines start here -----
*/

/*
	Insert file whose name is fname after line where.
	(where can be zero,  in which case insert at start of file.)
	Use promt as the promt line mode while doing so.
*/
static void
append(char *fname, int where, char *prompt)
{
	FILE *fd;
	char buffer [MAXLEN];	/* disk line buffer */
	int  oldline, n;

	TRACEPB("append",  sl_lpout();
		sl_sout(fname);  sl_csout();
		sl_iout(where);  sl_csout(); 
		sl_sout(prompt); sl_rpout());

	/* Open the new file. */
	fd = sysfopen(fname);
	if (fd == NULL) {
		cmndmess("File not found.");
		RETURN_VOID("append");
	}

	/* Say that we've started. */
	pmtmode(prompt);

	/* Go to after proper line,  unless line is zero. */
	bufgo(where);
	if (where) {
		bufdn();
	}

	/* Read the file into the buffer. */
	while ((n = sysfgets(fd,buffer,MAXLEN)) >= 0) {
		if (n > MAXLEN) {
			cmndmess("line truncated.");
			n = MAXLEN;
		}
		bufins(buffer,n);
		bufdn();
	}

	/* Close the file. */
	sysfclose(fd);

	TICKX("append");
}

/*
	Return TRUE if buffer may be drastically changed.
*/
int
chkbuf(void)
{
	int c;
	int x, y;

	SL_DISABLE();

	/* Save cursor position. */
	x = outx;
	y = outy;

	if (bufchng() == FALSE) {
		return TRUE;
	}

	pmtmess("", "Buffer not saved.  Proceed ?  ");
	c = syscin();
	outchar(c);

	/* Restore cursor postion. */
	outxy(x, y);

	/* Watch out:  tolower may be a macro. */
	if (tolower(c) == 'y') {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

/*
	Return TRUE if the user has pressed any key.
*/
int
chkkey(void)
{
	int c;

	SL_DISABLE();

	c = syscstat();
	return (c == -1) ? FALSE : TRUE;
}

/*
	Return TRUE if the user has pressed any control key.
*/
static int
chkckey(void)
{
	int c;

	SL_DISABLE();

	c = syscstat();
	return (c != -1 && ((c & 0x7f) < 32)) ? TRUE : FALSE;
}

/*
	Print a cmndmess on the command line and wait for a key.
*/
void
cmndmess(char * mess)
{
	int x, y;

	SL_DISABLE();

	/* Save cursor. */
	x = outx;
	y = outy;

	pmtmess("", mess);

	/* Wait for any key. */
	syscin();

	/* Restore cursor. */
	outxy(x, y);
}

/*
	Get one, two or three arguments.
	Missing arguments are set to default values.
*/
int
get3args(
	char *args,
	int *val1, int def1,
	int *val2, int def2,
	int *val3, int def3)
{
	TRACEPB("get3args", sl_lpout();
		sl_sout(args); sl_csout();
		sl_pout(val1); sl_csout(); 
		sl_iout(def1); sl_csout();
		sl_pout(val2); sl_csout(); 
		sl_iout(def2); sl_csout();
		sl_pout(val3); sl_csout(); 
		sl_iout(def3); sl_rpout());

	/* Skip the command. */
	args = skiparg (args);
	args = skipbl (args);

	/* Set defaults. */
	*val1 = def1;
	*val2 = def2;
	*val3 = def3;

	/* Check first arg. */
	if (*args == '\0') {
		RETURN_INT("get3args", OK);
	}
	if (number (args, val1) == FALSE) {
		cmndmess("Bad argument.");
		RETURN_INT("get3args", ERROR);
	}

	/* Skip over first argument. */
	args = skiparg(args);
	args = skipbl(args);

	/* Check second argument. */
	if (*args == '\0') {
		RETURN_INT("get3args", OK);
	}
	if (number(args, val2) == FALSE) {
		cmndmess("Bad argument.");
		RETURN_INT("get3args", ERROR);
	}

	/* Skip over third argument. */
	args = skiparg(args);
	args = skipbl(args);

	/* Check third argument. */
	if (*args == '\0') {
		RETURN_INT("get3args", OK);
	}

	if (number (args, val3) == FALSE) {
		cmndmess("Bad argument.");
		RETURN_INT("get3args", ERROR);
	}
	else {
		RETURN_INT("get3args", OK);
	}
}

/*
	Print message.
*/
void
message(char *s)
{
	SL_DISABLE();

	fmtsout(s,0);
	fmtcrlf();
}

/*
	Move or copy a block of lines.
*/
static void
movecopy(char *args, char *usage, char *prompt, int erase)
{
	int count, i, length;
	int fstart, fend, tstart;
	char buffer [MAXLEN1];

	TRACEPB("movecopy", sl_lpout();
		sl_sout(args);   sl_csout();
		sl_sout(usage);  sl_csout(); 
		sl_sout(prompt); sl_csout();
		sl_iout(erase);  sl_rpout());

	/* Get two or three args. */
	if (get3args(args,&fstart,-1,&fend,-1,&tstart,-1)==ERROR){
		RETURN_VOID("movecopy");
	}

	TRACEP("movecopy",
		sl_sout("fstart ");   sl_iout(fstart);
		sl_sout(", fend ");   sl_iout(fend);
		sl_sout(", tstart "); sl_iout(tstart);
		sl_cout('\n'));

	if (fstart == -1 || fend == -1) {
		cmndmess(usage);
		RETURN_VOID("movecopy");
	}
	if (tstart == -1) {
		tstart = fend;
		fend   = fstart;
	}

	/* Make sure the last line exists. */
	bufgo(max(fstart, tstart));
	if (bufatbot()) {
		bufup();
		if (fstart >= tstart) {
			fend = bufln();
		}
		else {
			tstart = bufln();
		}
	}		

	/*
	The 'to block' and 'from block' must not overlap.
	fstart must be > 0, tstart must be >=  0.
	*/
	if (fend < fstart ||
	    fstart <= 0 ||
	    tstart < 0 ||
	    (tstart >=  fstart && tstart < fend)
	   ) {
		cmndmess(usage);
		RETURN_VOID("movecopy");
	}

	/* Extract block to TEMP_FILE. */
	xtrct(TEMP_FILE, fstart, fend, prompt);

	/* Inject TEMP_FILE at tstart. */
	append(TEMP_FILE, tstart, prompt);

	if (erase) {
		count = fend - fstart + 1;
		/* Erase 'from block'. */
		if (fstart < tstart) {
			bufgo(fstart);
		}
		else {
			bufgo(fstart + count);
		}
		bufdeln(count);
	}

	/* Erase the TEMP_FILE. */
	sysunlink(TEMP_FILE);

	/* Redraw the screen. */
	bufgo(tstart);
	edclr();
	edgo(tstart, 0);

	TICKX("movecopy");
}

/*
	Replace oldpat in oldline by newpat starting at col.
	Put result in newline.
	Return number of characters in newline.
*/
static int
replace(char *oldline, char *newline, char *oldpat, char *newpat, int col)
{
	int k;
	char *tail, *pat;

	TRACEPB("replace",  sl_lpout();
		sl_sout(oldline); sl_csout(); 
		sl_sout(newline); sl_csout();
		sl_sout(oldpat);  sl_csout(); 
		sl_sout(newpat);  sl_csout();
		sl_iout(col);     sl_rpout());

	/* Copy oldline preceding col to newline. */
	k = 0;
	while (k < col) {
		newline [k++] = *oldline++;
	}

	/* Remember where end of oldpat in oldline is. */
	tail = oldline;
	pat = oldpat;
	while (*pat++ != '\0') {
		tail++;
	}

	/*
		Copy newpat to newline.
		Use oldline and oldpat to resolve question
		marks in newpat.
	*/
	while (*newpat != '\0') {
		if (k > MAXLEN-1) {
			cmndmess("New line too long.");
			RETURN_INT("replace", ERROR);
		}
		if (*newpat != '?') {
			/* Copy newpat to newline. */
			newline [k++] = *newpat++;
			continue;
		}

		/* Scan for '?' in oldpat. */
		while (*oldpat != '?') {
			if (*oldpat == '\0') {
				cmndmess(
				"Too many ?'s in change mask."
				);
				RETURN_INT("replace", ERROR);
			}
			oldpat++;
			oldline++;
		}

		/* Copy char from oldline to newline. */
		newline [k++] = *oldline++;
		oldpat++;
		newpat++;
	}

	/* Copy oldline after oldpat to newline. */
	while (*tail != '\0') {
		if (k >=  MAXLEN-1) {
			cmndmess("New line too long.");
			RETURN_INT("replace", ERROR);
		}
		newline [k++] = *tail++;
	}
	newline [k] = '\0';

	RETURN_INT("replace", k);
}

/*
	Skip over all except '\0', and blanks.
*/
static char *
skiparg(char *args)
{
	SL_DISABLE();

	while (*args != '\0' && *args != ' ') {
		args++;
	}
	return args;
}

/*
	Skip over all blanks.
*/
static char *
skipbl(char *args)
{
	SL_DISABLE();

	while (*args == ' ') {
		args++;
	}
	return args;
}

/*
	Create a file named fname from start to finish.
	Set the prompt mode to prompt while doing so.
*/
void
xtrct(char * fname, int start, int finish, char * prompt)
{
	FILE *fd;
	int i;
	int count, length;
	int oldline;
	char buffer [MAXLEN1];

	TRACEPB("xtrct",  sl_lpout();
		sl_sout(fname);  sl_csout();
		sl_iout(start);  sl_csout(); 
		sl_iout(finish); sl_csout();
		sl_sout(prompt); sl_rpout());

	/* Say we've started. */
	pmtmode(prompt);

	/* Open a temporary file. */
	fd = sysfcreat(fname);
	if (fd == NULL) {
		cmndmess("Can not open temporary file.");
		RETURN_VOID("xtrct");
	}

	/* Save the current line. */
	oldline = bufln();

	/* Copy the block to the temp file. */
	bufgo(start);
	for (count = finish - start + 1; count; count--) {
		length = bufgetln(buffer, MAXLEN);
		if (length < 0) {
			cmndmess("Error reading line.");
		}
		for (i = 0; i < length; i++) {
			sysputc(buffer [i], fd);
		}
		sysputc('\r', fd);
		sysputc('\n', fd);
		bufdn();
		if (bufatbot()) {
			break;
		}
	}

	/* Close the file. */
	sysfclose(fd);

	/* Restore the current line. */
	bufgo(oldline);

	TICKX("xtrct");
}
