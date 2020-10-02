/*
HEADER:		CUG208;
TITLE:		'e' for CP/M68K
VERSION:	1.48+

DESCRIPTION:	"a screen editor";

KEYWORDS:	editor;
SYSTEM:		CP/M68K, V1.2;
FILENAME:	e/eterm.c
WARNINGS:	"the default value is for systems with 128K bytes
		 of memory or more";
SEE-ALSO:	cpm68k.c, e68k.doc, CUG VOL 133;
AUTHORS:	G.N.Gilbert('e'), J.W.Haefner(for DeSmet C on MSDOS and UNIX)
CODER:		Yoshimasa Tsuji
COMPILERS:	DRI C(Alcyon C) for CP/M68K;
*/
/*
	FUNCTIONS: terminit,terminal,gotoxy,deleteline,linedelete,delpage,
		   clear, insertline,standout,standend,keytranslate,help

	PURPOSE: terminal dependent functions
	TERMINALS SUPPORTED:
	adm31, kaypro, adds, vt52, haze, h19, adm3a, beehive, vt100
	(televideo, ibm, ansi fall into somewhere)
*/
#define adm31	1
#define kaypro 0
#define adds	0
#define vt52	0
#define haze	0
#define h19	0
#define adm3a	0
#define beehive 0
#define vt100	0

#include "e.h"

/*-------------------------OUTPUT TO SCREEN---------------------------*/
char *Standout;
char *Standend;
char *csraddr;
char *insline;
char *delline;
char *lindel;
char *pagedel;
char *Clear;		/* clear screen string */
char *initstr = "";
char *finistr = "";
char *termname;

terminit()
{
			/* initialise tty, esc sequence, set raw */
#if adm31
 Standout = "\033)";
 Standend = "\033(";
 csraddr = "\033=";
 insline = "\033E";
 delline = "\033T";
 lindel = "\033R";
 pagedel = "\033Y";
 Clear = "\033*";
 termname = "|for ADM31";
# endif

# if h19 | adm3a | beehive
#	if beehive
 Standout = "\033l";
 Standent = "\033m";
# 	else
 Standout = "\033p";
 Standend = "\033q";
# 	endif
# 	if adm3a
 csraddr = "\033=";
# 	else
 csraddr = "\033Y";
# 	endif
 insline = "\033L";
 delline = "\033K";
 lindel = "\033M";
 pagedel = "\033J";
 Clear = "\033E";
#	if h19
 termname = "|for H19";
#	endif
#	if adm3a
 termname = "|for ADM-3A";
#	endif
#	if beehive
 termname = "|for Beehive";
#	endif
# endif
# if vt52 | adds
 Standout = "\033U";
 Standend = "\033T";
 crsaddr = "\033Y";
 insline = "\033L";
 delline = "\033K";
 lindel = "\033M";
 pagedel = "\033k";
#  if vt52
 initstr = "\033\060@\033\071P";
	/* set foreground to normal
	 * set background to reverse
	 */
 finistr = "\033\071@";
	/* set background back to normal*/
 termname = "|for DEC VT52";
#  endif
#  if adds
 initstr = "\033\071@\033\060P";
 finistr = "\033\060@";
 termname = "|for ADDS";
#  endif
# endif

# if vt100
 Standout = "\033[1m";
 Standend = "\033[0m";
 crsaddr = "\033[";
 insline = "\033[1L";
 delline = "\033[K";
 lindel = "\033[1M";
 pagedel = "\033[J";
 termname = "|for DEC VT100";
# endif

# if kaypro
/*
	specific to the KayPro 10
	THANKS to Don Colner for providing these codes and key assignments!
*/
 Standout = "\033B1";
 Standend = "\033C1";
 crsaddr = "\033=";
 insline = "\033E";
 delline = "\030";
 lindel = "\033R";	/* (0,y), ** , (0,y) */
 pagedel = "\027";
 termname = "|for Kaypro 10";
# endif
# if haze
/*
 specific to the HAZELTINE range of terminals.
	NOTE ALSO that the terminal must be set to use TILDE as the lead -in
	character, not Escape.
 screen control characters for Hazeltine 1420 (and probably other
	Hazeltines as well)
 */
 termname = "|for Hazeltine";
 Standout = "\0134\031";
 Standend = "\0134\037";
 csraddr =  "\0134\021"; /* x, y */
 insline = "\0134\033"; /* (0,y), ** */
 delline = "\0134\017";
 lindel = "\0134\023"; /* (0,y), ** */
 pagedel = "\0134\030";
#endif


	raw(); 	/* set tty input in raw mode */
	putstr(initstr);
}

termfini()	/*restore terminal to 'normal' state.  Called once before
		  program exits */
{
	putstr(finistr);
	noraw();	/* restore normal tty mode */
}

terminal()	/*display name of terminal*/
{
	putstr(termname);
}

gotoxy(x,y)	/*move cursor to column x, line y {top left is (0,0) */
int x,y;
{
	putstr(csraddr);
# if haze
	putch(x), putch(y);
# else
#  if vt100
	uspr(y+1);
	putch(';');
	uspr(x+1);
	putch('H');
#  else
	putch(y+32);
	putch(x+32);
#  endif
# endif
}

clear()	/* clear stdscr */
{
	delpage(0);
/*
	putstr(Clear);
 */
}


deleteline(x,y)	/*clear to spaces all characters on line y from column x
			to the right edge of the screen, inclusive.
			Leave cursor on left most blank */
int x, y;
{
	gotoxy(x,y);
	putstr(delline);
	gotoxy(x,y);

}

linedelete(y)	/*delete the line y.  All following lines move up one.
			Leave cursor at start of line y */
int y;
{
	gotoxy(0,y);
	putstr(lindel);
	gotoxy(0,y);
}

delpage(y)	/*clear to spaces line y and all following lines.  Leave
			cursor at the start of line y */
int y;
{
	gotoxy(0,y);
	putstr(pagedel);
}

insertline()	/*move all lines below line on which cursor is, down one,
			losing last line.  New cursor line is blank, with
			cursor at start of line */
{
	putstr(insline);
}

standout()	/* stand out all subsequent characters */
{
	putstr(Standout);
}

standend()	/* display all subsequent characters normally */
{
	putstr(Standend);
}


/*----------------------INPUT FROM KEYBOARD-----------------------------*/

# define ctrl(letter) ('letter' & 037)
#define ESC 	033


keytranslate()	/*defines the terminal key codes which perform
			the editor commands */
{
	/* each tran[xxxx]= should be set to the code emitted by the terminal
	key which will perform the indicated action.  The recommended (control)
	key assignments are shown in round brackets.
		Some terminals precede their codes by a lead-in character
	(eg the Hazeltine uses the tilde).  This char should be assigned to
	tran[LEADIN].  If there is no lead-in character, set tran[LEADIN] to zero.
		'e' will ignore the leadin character if tran[LEADIN] is non-zero,
	but will set the parity bit on the next character. All other chars from the
	keyboard will already have any parity bits removed as they are read in.  Thus
	codes with lead-ins should be entered in the table below as code+0x80, or
	more clearly, as code+PARBIT.
		For example, suppose that one is coding the Hazeltine 1420, which
	generates a tilde,^L (0x0d) sequence when the cursor up key is pressd.  To
	recognise this sequence, set tran[LEADIN] to tilde (0x7e) and set
	tran[UPKEY] to 0x0d+PARBIT.
	*/
# if adm31 | h19 | adm3a | beehive | vt100
	tran[LEADIN]=0;		/*lead-in character, 0 if not used*/
	tran[DOWNKEY]= ctrl(J);	/*cursor down */
	tran[UPKEY]= ctrl(K);	/*cursor up*/
	tran[LEFTKEY]= ctrl(H);	/*cursor left*/
	tran[RIGHTKEY]= ctrl(L);	/*cursor right*/
	tran[RIGHTWKEY]= ctrl(D);	/*cursor right one word  */
	tran[LEFTWKEY]= ctrl(S);	/*cursor left one word  */
	tran[EOLKEY]= ctrl(E);	/*cursor to end of line  */
	tran[BOLKEY]= ctrl(B);	/*cursor to beginning of line  */
	tran[UPPAGE]= ctrl(W);	/*scroll up a page  */
	tran[DOWNPAGE]= ctrl(Z);	/*scroll down a page  */
	tran[BOFKEY]= ctrl(U);	/*cursor to beginning of file  */
	tran[HOMEKEY]= ctrl(^);	/*cursor to end of file (HOME) */
	tran[DELLEFT]= 0177;	/*delete char to left of cursor (DEL) */
	tran[DELRIGHT]= ctrl(G);	/*delete char under cursor  */
	tran[DELLNKEY]= ctrl(Y);	/*delete cursor line  */
	tran[DELWDKEY]= ctrl(T);	/*delete word to right of cursor  */
	tran[JUMPKEY]= ctrl(X);	/*jump to  */
	tran[CRSTILL]= ctrl(N);	/*insert newline after cursor  */
	tran[QUITKEY]= ctrl(Q);	/*quit  */
	tran[ENVIRKEY]= ctrl(C);/*edit and file context */
	tran[FINDKEY]= ctrl(F);	/*find  */
	tran[ALTERKEY]= ctrl(A);	/*alter  */
	tran[BLOCKKEY]= ctrl(O);	/*block operations  */
	tran[RDFILEKEY]= ctrl(P);	/*read a file */
	tran[REPKEY]= ctrl(R);	/*repeat last find/alter */
	tran[HELPKEY]= ctrl(V);	/*display help menu */
	tran[UNDOKEY]= ctrl(\\);	/*restore unedited line  */
	tran[TAB]= ctrl(I);	/*tab  */
	tran[RETRIEVE]= ctrl(R);	/*retrieve */
	tran[CR]= ctrl(M);	/*return*/
	tran[ESCKEY]=ESC;	/*escape, the magic key (ESC)*/
}
# endif
/* of adm31 */
# if vt52
	tran[LEADIN]=ESC;	/*lead-in character, 0 if not used*/
	tran[DOWNKEY]= 'B'+PARBIT;	/*cursor down */
	tran[UPKEY]= 'A'+PARBIT;	/*cursor up*/
	tran[LEFTKEY]= 'D'+PARBIT;	/*cursor left*/
	tran[RIGHTKEY]= 'C'+PARBIT;	/*cursor right*/
	tran[RIGHTWKEY]= ctrl(D);	/*cursor right one word  */
	tran[LEFTWKEY]= ctrl(S);	/*cursor left one word  */
	tran[EOLKEY]= ctrl(E);	/*cursor to end of line  */
	tran[BOLKEY]= ctrl(B);	/*cursor to beginning of line  */
	tran[UPPAGE]= ctrl(W);	/*scroll up a page  */
	tran[DOWNPAGE]= ctrl(K);	/*scroll down a page (Z) */
	tran[BOFKEY]= ctrl(H);	/*cursor to beginning of file (U) */
	tran[HOMEKEY]= ctrl(A);	/*cursor to end of file (HOME) */
	tran[DELLEFT]= 0177;	/*delete char to left of cursor (DEL) */
	tran[DELRIGHT]= ctrl(G);	/*delete char under cursor (G) */
	tran[DELLNKEY]= ctrl(Y);	/*delete cursor line (Y) */
	tran[DELWDKEY]= ctrl(T);	/*delete word to right of cursor (T) */
	tran[JUMPKEY]= ctrl(X);	/*jump to (X) */
	tran[CRSTILL]= ctrl(N);	/*insert newline after cursor (N) */
	tran[QUITKEY]= ctrl(Q);	/*quit (Q) */
	tran[ENVIRKEY]= ctrl(C);	/*edit and file context (C) */
	tran[FINDKEY]= ctrl(L);	/*find (F) */
	tran[ALTERKEY]= ctrl(^);	/*alter (A) */
	tran[BLOCKKEY]= ctrl(O);	/*block operations (O) */
	tran[RDFILEKEY]= ctrl(P);	/*read a file (P) */
	tran[REPKEY]= ctrl(R);	/*repeat last find/alter (R) */
	tran[HELPKEY]= ctrl(V);	/*display help menu (V) */
	tran[UNDOKEY]= ctrl(\\);	/*restore unedited line (\) */
	tran[TAB]= ctrl(I);	/*tab (I) */
	tran[RETRIEVE]= ctrl(R);	/*retrieve (R)*/
	tran[CR]= ctrl(M);	/*return*/
	tran[ESCKEY]=ESC;	/*escape, the magic key (ESC)*/
}
# endif
/* of vt52 */
# if adds
	tran[LEADIN]=ESC;	/*lead-in character, 0 if not used*/
	tran[DOWNKEY]= ctrl(J);	/*cursor down */
	tran[UPKEY]= ctrl(Z);	/*cursor up*/
	tran[LEFTKEY]= ctrl(U);	/*cursor left*/
	tran[RIGHTKEY]= ctrl(F);	/*cursor right*/
	tran[RIGHTWKEY]= ctrl(D);	/*cursor right one word  */
	tran[LEFTWKEY]= ctrl(S);	/*cursor left one word  */
	tran[EOLKEY]= ctrl(E);	/*cursor to end of line  */
	tran[BOLKEY]= ctrl(B);	/*cursor to beginning of line  */
	tran[UPPAGE]= ctrl(W);	/*scroll up a page  */
	tran[DOWNPAGE]= ctrl(K);	/*scroll down a page (Z) */
	tran[BOFKEY]= ctrl(H);	/*cursor to beginning of file (U) */
	tran[HOMEKEY]= ctrl(A);	/*cursor to end of file (HOME) */
	tran[DELLEFT]= 0177;	/*delete char to left of cursor (DEL) */
	tran[DELRIGHT]= ctrl(G);	/*delete char under cursor (G) */
	tran[DELLNKEY]= ctrl(Y);	/*delete cursor line (Y) */
	tran[DELWDKEY]= ctrl(T);	/*delete word to right of cursor (T) */
	tran[JUMPKEY]= ctrl(X);	/*jump to (X) */
	tran[CRSTILL]= ctrl(N);	/*insert newline after cursor (N) */
	tran[QUITKEY]= ctrl(Q);	/*quit (Q) */
	tran[ENVIRKEY]= ctrl(C);	/*edit and file context (C) */
	tran[FINDKEY]= ctrl(L);	/*find (F) */
	tran[ALTERKEY]= ctrl(^);	/*alter (A) */
	tran[BLOCKKEY]= ctrl(O);	/*block operations (O) */
	tran[RDFILEKEY]= ctrl(P);	/*read a file (P) */
	tran[REPKEY]= ctrl(R);	/*repeat last find/alter (R) */
	tran[HELPKEY]= ctrl(V);	/*display help menu (V) */
	tran[UNDOKEY]= ctrl(\\);	/*restore unedited line (\) */
	tran[TAB]= ctrl(I);	/*tab (I) */
	tran[RETRIEVE]= ctrl(R);	/*retrieve (R)*/
	tran[CR]= ctrl(M);	/*return*/
	tran[ESCKEY]=ESC;	/*escape, the magic key (ESC)*/
}
# endif
/* of adds */
# if kaypro
	tran[LEADIN]=0;		/*lead-in character, 0 if not used*/
	tran[DOWNKEY]= ctrl(X);	/*cursor down */
	tran[UPKEY]= ctrl(E);	/*cursor up*/
	tran[LEFTKEY]= ctrl(S);	/*cursor left*/
	tran[RIGHTKEY]= ctrl(D);	/*cursor right*/
	tran[RIGHTWKEY]= ctrl(P);	/*cursor right one word (D) */
	tran[LEFTWKEY]= ctrl(O);	/*cursor left one word (S) */
	tran[EOLKEY]= ctrl(J);	/*cursor to end of line (E) */
	tran[BOLKEY]= ctrl(H);	/*cursor to beginning of line (B) */
	tran[UPPAGE]= ctrl(W);	/*scroll up a page  */
	tran[DOWNPAGE]= ctrl(Z);	/*scroll down a page (Z) */
	tran[BOFKEY]= ctrl(T);	/*cursor to beginning of file (U) */
	tran[HOMEKEY]= ctrl(B);	/*cursor to end of file (HOME) */
	tran[DELLEFT]= 0177;	/*delete char to left of cursor (DEL) */
	tran[DELRIGHT]= ctrl(G);	/*delete char under cursor (G) */
	tran[DELLNKEY]= ctrl(Y);	/*delete cursor line (Y) */
	tran[DELWDKEY]= ctrl(]);/*delete word to right of cursor (T) */
	tran[JUMPKEY]= ctrl(L);	/*jump to (X) */
	tran[CRSTILL]= ctrl(N);	/*insert newline after cursor (N) */
	tran[QUITKEY]= ctrl(Q);	/*quit (Q) */
	tran[ENVIRKEY]= ctrl(C);	/*edit and file context (C) */
	tran[FINDKEY]= ctrl(F);	/*find (F) */
	tran[ALTERKEY]= ctrl(A);	/*alter (A) */
	tran[BLOCKKEY]= ctrl(K);	/*block operations (O) */
	tran[RDFILEKEY]= ctrl(R);	/*read a file (P) */
	tran[REPKEY]= ctrl(\\);	/*repeat last find/alter (R) */
	tran[HELPKEY]= ctrl(V);	/*display help menu (V) */
	tran[UNDOKEY]= ctrl(U);	/*restore unedited line (\) */
	tran[TAB]= ctrl(I);	/*tab (I) */
	tran[RETRIEVE]= ctrl(R);	/*retrieve (R)*/
	tran[CR]= ctrl(M);	/*return*/
	tran[ESCKEY]=ESC;	/*escape, the magic key (ESC)*/
}
# endif
/* of kaypro */
# if haze
	tran[LEADIN]= 0176;		/* TILDE is the lead in character */
	tran[DOWNKEY]= (ctrl(K))+PARBIT;	/*cursor down */
	tran[UPKEY]= (ctrl(L))+PARBIT;		/*cursor up*/
	tran[LEFTKEY]= ctrl(H);		/*cursor left*/
	tran[RIGHTKEY]= ctrl(P);		/*cursor right*/
	tran[RIGHTWKEY]= ctrl(D);		/*cursor right one word  */
	tran[LEFTWKEY]= ctrl(S);		/*cursor left one word  */
	tran[EOLKEY]= ctrl(E);		/*cursor to end of line  */
	tran[BOLKEY]= ctrl(B);		/*cursor to beginning of line  */
	tran[UPPAGE]= ctrl(W);		/*scroll up a page  */
	tran[DOWNPAGE]= ctrl(Z);		/*scroll down a page (Z) */
	tran[BOFKEY]= ctrl(U);		/*cursor to beginning of file (U) */
	tran[HOMEKEY]= (ctrl(R))+PARBIT;	/*cursor to end of file (HOME) */
	tran[DELLEFT]= 0177;		/*delete char to left of cursor (DEL) */
	tran[DELRIGHT]= ctrl(G);		/*delete char under cursor (G) */
	tran[DELLNKEY]= ctrl(Y);		/*delete cursor line (Y) */
	tran[DELWDKEY]= ctrl(T);		/*delete word to right of cursor (T) */
	tran[JUMPKEY]= ctrl(X);		/*jump to (X) */
	tran[CRSTILL]= ctrl(N);		/*insert newline after cursor (N) */
	tran[QUITKEY]= ctrl(Q);		/*quit (Q) */
	tran[ENVIRKEY]= ctrl(C);		/*edit and file context (C) */
	tran[FINDKEY]= ctrl(F);		/*find (F) */
	tran[ALTERKEY]= ctrl(A);		/*alter (A) */
	tran[BLOCKKEY]= ctrl(O);		/*block operations (O) */
	tran[RDFILEKEY]= ctrl(K);		/*read a file (P) */
	tran[REPKEY]= ctrl(R);		/*repeat last find/alter (R) */
	tran[HELPKEY]= ctrl(V);		/*display help menu (V) */
	tran[UNDOKEY]= ctrl(\\);		/*restore unedited line (\) */
	tran[TAB]= ctrl(I);		/*tab (I) */
	tran[RETRIEVE]= ctrl(R);		/*retrieve (R)*/
	tran[CR]= ctrl(M);		/*return*/
	tran[ESCKEY]=ESC;		/*escape, the magic key (ESC)*/
}
# endif
/* of haze */

/*-------------------------------HELP MENU-------------------------------*/
help()	/*display the help menu.  There must be (helplines-1)
			lines of menu here (see terminal function,above) */
{
	putstr("CURSOR:  ^B|=beginning of line| ^E|=end of line|    ^S|=left word|  ^D|=right word\n");
	putstr("MOVE:    ^W|=up a screen|       ^Z|=down a screen|  ^U|=top,     |HOME|=bottom of file\n");
	putstr("DELETE: DEL|=char left|         ^G|=char right|     ^T|=word|       ^Y|=line|  ^\\|=undo\n");
	putstr("         ^F|=find|              ^A|=alter|          ^R|=repeat|     ^V|=view menu\n");
	putstr("   ^     ^N|=insert a new line| ^X|=jump to line|   ^P|=read file|  ^Q|=quit,save file\n");
	putstr(" <- ->   ^C|=edit,file context| ^O|=write,print,shift,move,copy,or delete block\n");
	putstr("---v------------------------------------------------------------------------");
}
/*---------------------------------END---------------------------------------*/



