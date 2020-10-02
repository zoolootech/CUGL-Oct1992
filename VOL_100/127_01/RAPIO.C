/*********************************************************************\
** .---------------------------------------------------------------. **
** |                                                               | **
** |                                                               | **
** |         Copyright (c) 1981, 1982, 1983 by Eric Martz.         | **
** |                                                               | **
** |                                                               | **
** |       Permission is hereby granted to use this source         | **
** |       code only for non-profit purposes. Publication of       | **
** |       all or any part of this source code, as well as         | **
** |       use for business purposes is forbidden without          | **
** |       written permission of the author and copyright          | **
** |       holder:                                                 | **
** |                                                               | **
** |                          Eric Martz                           | **
** |                         POWER  TOOLS                          | **
** |                    48 Hunter's Hill Circle                    | **
** |                      Amherst MA 01002 USA                     | **
** |                                                               | **
** |                                                               | **
** `---------------------------------------------------------------' **
\*********************************************************************/

/*
REDIRECTIONS SUPPORTED:

	)filename
	
	)/x...
	
		where x is:
		
			l or L for LIST
			pu or PU for PUNCH
			po or PO for user port
			s or S for SCREEN (CONSOLE)
*/

#include "rap.h"


#define SCREEN 1
#define LIST_OUT 2
#define PUNCH_OUT 4
#define PORT_OUT 8
#define FILE_OUT 16

#define CTRL_C '\3'
#define CTRL_W '\27'
#define CTRL_E '\5'

#define INTOREV "\33[7m"
#define OUTAREV "\33[0m"

/* 
	The "dioinit" function must be called at the beginning of the
	"main" function:
*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define argc *p_argc

dioinit(p_argc,argv)
int *p_argc;
char **argv;
{
	int i,j;
	int n;	/* this keeps track of location in argument */
	char *p;

#ifdef DEBUG
	if (Debug) fprintf(STDERR, "DIOINIT: (argc = %d)\n", argc);
#endif

	Xflag = YES;

	for (i = 1; i < argc; i++) { /* Scan the command line for / */

#ifdef DEBUG
		if (Debug) fprintf(STDERR, "\tArg %d:%s\n", i, argv[i]);
#endif

		if (*argv[i] EQ ')') {
			if (!argv[i][1]) {
				_Outflag = 0;
				goto delarg;
			}
			/* DEVICE? */
			if (argv[i][1] EQ '/') {
				p = argv[i]+2;
				switch (*p) {
					case 'L':
						_Outflag |= LIST_OUT;
						break;
					case 'P':
						switch (*(p+1)) {
							case 'U':
								_Outflag |= PUNCH_OUT;
								break;
							case 'O':
								_Outflag |= PORT_OUT;
								break;
							default:
								goto badout;
						}
						break;
					case 'S':
						_Outflag |= SCREEN;
						Screen_size = SCREEN_SIZE;
						break;
					default:
						goto badout;
				}
			}
			
			/* FILE OUT */
			else {
				strcpy (Out_file, argv[i]+1 );
				if (badname(Out_file)) exit(0);
				if (fcreat("TEMPOUT.$$$", _Dobuf) == ERROR) {
					fprintf(STDERR,"\nCan't create <%s>; disk full?\n",
						"TEMPOUT.$$$");
					exit();
				}
				_Outflag |= FILE_OUT;
			}

			/* DELETE ARGUMENT FOR PASSING TO MAIN */
delarg:
			for (j = i; j < argc; j++) argv[j] = argv[j+1];
			(argc)--;
			i--;
		}
	}
#ifdef DEBUG
	if (Debug) fprintf(STDERR, "Returning from DIOINIT\n");
#endif
	return(0);
badout:
	fprintf(STDERR,
		"Bad output specifier:%s",
		argv[i]);
	exit();
}
#undef argc
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*
	The "dioflush" function must be called before exiting the program:
*/

dioflush()
	{
	if (_Outflag & FILE_OUT) {
#ifdef DEBUG
		if (Debug) fprintf(STDERR,"FLUSHING FILE_OUT\n");
#endif
		putc(CPMEOF,_Dobuf);
		fflush(_Dobuf);
		bf_close(_Dobuf);
		if (strcmp ("TEMPOUT.$$$", Out_file)) {
			unlink (Out_file);
			rename ("TEMPOUT.$$$", Out_file);
		}
	}
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*
	This version of "putchar" replaces the regular version when using
	directed I/O:
*/

putchar(c)
	char c;
	{
	char *pc;
	int control;
	int suppress_cr, compress;

	if (First_text) {
		First_text = NO;
		if (Screen_size) fprintf(STDERR,
			"%s<Top of first page>%s\n", Bar, Bar);
		if (Init) puts(Init); /* RECURSIVE CALL OF PUTCHAR! */
		if (Font NE 0 AND Font_o) printf("%s%d%s", Font_o, Font, Font_c);
		if (Justify NE 0) restore_mode();
	}

	if (bdos(CON_STATUS)) {
		control = bdos(CONIN);
		if (!Interrupted AND control EQ CTRL_C) abort();
		if (control EQ CTRL_W) wait();
		if (control EQ CTRL_E) End_wait = YES;
	}
		
	if (c > 127) {	/* META CHARACTER */
		if (c EQ c1(WAIT)) wait();

		/* TOGGLE PROMPT FLAG */
		else if (c EQ c1(PROMPT)) {
			if(!Prompt) Prompt = YES;
			else {
				Prompt = NO;
				if (!Screen_size) {
					bdos(CONOUT, '\r');
					bdos(CONOUT, '\n');
				}
			}
		}
		else {
			fprintf(STDERR,
"Warning: illegal meta character (decimal %d) in output: page %d line %d\n",
				c, Curpag, Lineno-1);
		}
	}
	else if (Prompt) {
		if (!Screen_size) bdos(CONOUT, c);
	}
	else {
		suppress_cr = NO;

		if (Comp_carat AND !Carat AND c EQ '^') {
			Carat = YES;
			return(0);
		}

		if (Carat) {
			Carat = NO;
			if (c >= 64 AND c <=95) {
				if (c EQ 'J') suppress_cr = YES;
				c -= 64;
			}
			else {
				compress = Comp_carat;
				Comp_carat = NO;
				putchar('^');
				Comp_carat = compress;
			}
		}

		if (c EQ Blank_c) c = ' '; /* BLANK OUT SPECIAL CHARACTER */

		/* PRECEDE NL WITH CR UNLESS NL WAS COMPRESSED ^J */
		if (c EQ NEWLINE AND !suppress_cr) put_char(CR);

		/* PRECEDE LITERAL CR WITH REVERSE LINE FEED IF PRINTER GENERATES
		AUTO LF ON CR */
		if (c EQ CR AND Suppress_lf)
			for(pc=Rev_lf; *pc; pc++) put_char(*pc);

		/* AT LONG LAST, PUT OUT THE SILLY CHARACTER UNLESS IT IS
		AN ORDINARY NEWLINE, IN WHICH CASE, PUT IT OUT ONLY IF
		EITHER
			WE ARE NOT SUPPRESSING LINE FEEDS
		OR
			WE ARE SUPPRESSING LINE FEEDS BUT IT IS A COMPRESSED ^J */
		if (c NE NEWLINE OR !Suppress_lf OR suppress_cr)
			put_char(c);
	}
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
put_char(c)
	int c;
	{

	if (_Outflag & FILE_OUT) {
		if(putc(c,_Dobuf) == ERROR) {
			fprintf(STDERR,"File output error; disk full?\n");
			exit();
		}
	}

	if (_Outflag & SCREEN /* OR Debug */) {
#ifdef DEBUG
		if (Debug) fprintf(STDERR,"%s",INTOREV);
#endif
		bdos(CONOUT,c);
#ifdef DEBUG
		if (Debug) fprintf(STDERR,"%s",OUTAREV);
#endif
	}

	if (_Outflag & LIST_OUT)	bdos(5,c);

	if (_Outflag & PUNCH_OUT) bdos(4,c);

	if (_Outflag & PORT_OUT) {
		if (Pdata EQ -1) {
			fprintf(STDERR,
				"Error: SS file did not define i/o port.\n");
			exit(0);
		}
		xxoutc(c);
	}
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
xxoutc(c)
	char c;
	{
	int x;

	do {
		while (miready()) {
			if ((x = getmod()) == XOFF) Xflag = NO;
			if (x EQ NULL) Xflag = NO;
			if (x == XON) Xflag = YES;
		}
	} while (Xflag == NO);

	/* LOOP UNTIL READY */
	while ((inp(Pstatus) & Pomask) != (Pready ? Pomask : 0));

	outp(Pdata,c);
}	
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
miready()
	{
	return (inp(Pstatus) & Pimask) == (Pready ? Pimask : 0);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
getmod()
	{
	char c;
	c = inp(Pdata);
	c &= '\177';
	if (Preset) outp(Pstatus,Presetval);
	return c;
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
wait(){
	int c;

	if (Screen_size) return(0);

	while (!bdos(CON_STATUS));
	c = bdos(CONIN);
	puts("\b \b");
	if (c EQ CTRL_C) abort();
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
abort(){
	Interrupted = YES;
	fprintf(STDERR,"\n--- RAP ABORTED WITH ^C ---\n");
	if (!Screen_size) {
		printf("\n--- RAP ABORTED WITH ^C ---\n%c", FORMFEED);
		if (Forceprint) puts(Forceprint);
		if (Deinit) puts (Deinit);
	}
	dioflush();
	exit(1);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
c1(s)
	char *s;
	{
	return(*s);
}
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*	END OF RAPIO.C	*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 - - - - - -