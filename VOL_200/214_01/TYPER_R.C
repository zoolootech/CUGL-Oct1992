/* TYPER-R.C  VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:37:23 PM*/
/*
%CC1 $1.C -X 
%CLINK $1 -S
%DELETE $1.CRL 
*/
/* 
Description:

Line editor that handles typing not suitable for a word processor, 
eg, filling out forms, envelopes, short notes, etc.

The program offers control of full two-dimensional movement of the 
printhead, for printers with reverse line feed.  It is set
for Diablo conventions.

Based upon TYPER, by H. G. Lord.

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/
/*
Usage:	TYPER		 <== prints instructions, output goes to LST:
		TYPER PRINTER	 <== no instructions, output to LST:
		TYPER d:file.typ <== no instructions, output to disk file
Commands:
 		^H		= Destructive backspace
		^U 		= Cancel current line
		^P		= Display prompts
 		^R		= Clear screen
		<cr>, ^J, or ^M = Send line to device or file
		^Z		= End of session
		^C		= Abort session
Use wstar commands (^s,^d,^e,^x,tab) for immediate printhead movement,
	after flush of buffer contents.
 */

	/* page eject */


#define EOF		-1
#define NULL		'\0'
#define EOS		NULL
#define TRUE		1
#define TAB		5	/* If you change TAB, change the ruler, too*/
#define SPACE		0x20
#define CTRLC		0x03
#define CTRLD		0x04
#define CTRLE		0x05
#define CTRLH		0x08
#define CTRLI		0x09
#define CTRLJ		0x0A
#define CTRLL		0x0C
#define CTRLM		0x0D
#define CTRLP		0x10
#define CTRLR		0x12
#define CTRLS		0x13
#define CTRLU		0x15
#define CTRLX		0x18
#define ESC		0x1B
#define DEL		0x7F

#define SCREEN_WIDTH	79		/*Televideo 920c dependent*/
#define SCREEN_HEIGHT	23		/*Televideo 920c dependent*/

/*Televideo 920c dependent statments*/
#define CLS		putchar('Z' - 0x40);
#define HOME		putchar('^' - 0x40);
#define DEL_LINE	{putchar(ESC); putchar('T');}
#define INSERT_LINE	{putchar(ESC); putchar('E');}

/*Transtar dependent statements*/
#define REV_LINE_FEED	{putc(ESC, fdes); putc('\n', fdes);}

#include <bdscio.h>

char line_buf[MAXLINE];

int i, vert, m;

main(argc, argv)
int argc;
char *argv[];
{
	char *fdes;        /* File descriptor*/
	char obuf[MAXLINE];        /* IO buffer*/
	if (argc != 2)        /* No arguments*/
	{
		CLS;
		disp_instructions();        /* Display explanations*/
		getchar();
		CLS;
		disp_message();
		getchar();
		fdes = 2;
	}

	/* If argument is "PRINT", then send buffer to LST:*/
	if (strcmp(argv[1], "PRINTER") == NULL)
		fdes = 2;
	else
		if (fdes != 2)
		{
			file(argv[1], obuf);        /* Open file for writing*/
			fdes = obuf;
		}

	CLS;
	/* Get stuff on screen and send it to the file or LST:*/
	typer(fdes);
	If(fdes != 2) putc(CPMEOF, fdes);

	CLS;
	if (fclose(fdes) == ERROR)        /* Close file*/
		printf("\n\nerror in closing file\n\n");
}

int file(fname, iobuf)        /* Open output file in write mode*/
char *fname;
char *iobuf;
{
	int ii;
	ii = fcreat(fname, iobuf);        /* Open the file*/
	if (ii > 0)
		return ii;        /* Return channel number*/
	/* If no channel #, display an error message*/
	puts("\nSorry, I can\'t open the file.\n\n");
	exit();
}
/* End of fileout*/

void typer(fdes)
char *fdes;
{
	/* i is the line_buffer subscript*/
	/* m is the screen image cursor horizontal position*/
	/* vert is the screen image cursor vertical position*/

	int c;
	int j;

	i = m = NULL;        /* Reset character counter*/
	vert = 1;        /* Begin at line 1, not line 0*/
	ruler();        /*to allow display of ruler*/

	/* Get and process characters one at a time*/
	while ((c = getchar()) != EOF && c != CPMEOF)
	{
		if (c >= SPACE && c < DEL)        /* Displayable ASCII char*/
		{
			line_buf[i++] = c;        /* Char into buffer*/
			/* If cursor on screen, continue getting input*/
			/* If cursor off screen, send backspace*/
			/* to delete the last character*/
			if (++m <= SCREEN_WIDTH)
				continue;
			else
			    putchar(c = CTRLH);
		}
		switch (c)        /* Process control character, etc*/
		{
			/* Backspace or Delete*/
		case DEL :
			putchar(c = CTRLH);
		case CTRLH :
			if (!i)        /* If beginning of line*/
				putchar(CTRLL);        /* Move right*/
			else
			{
				putchar(SPACE);        /* Write a space*/
				putchar(CTRLH);        /* Backspace*/
				--m;
				--i;        /* Decrement subscripts*/
			}
			break;
			/* Flush line_buf then printhead movmt (back, fwrd, down, up)*/
		case CTRLS :
			for (j = 0; j < i; j++)
				putc(line_buf[j], fdes);
			i = NULL;
			/*backspace if cursor not at start of screen line*/
			if (m)
			{
				cursor(vert, --m);
				putc(CTRLH, fdes);
			}
			break;
		case CTRLD :
			for (j = 0; j < i; j++)
				putc(line_buf[j], fdes);
			i = NULL;
			/*advance if cursor not at end of screen line*/
			if (m < SCREEN_WIDTH)
			{
				cursor(vert, ++m);
				putc(SPACE, fdes);
			}
			break;
		case CTRLX :
			for (j = 0; j < i; j++)
				putc(line_buf[j], fdes);
			i = NULL;
			/*move down one line*/
			/*but if cursor at screen bottom, scroll screen up*/
			if (vert < SCREEN_HEIGHT)
				vert++;
			putchar('\n');
			ruler();
			cursor(vert, m);
			putc('\n', fdes);
			break;
		case CTRLE :
			for (j = 0; j < i; j++)
				putc(line_buf[j], fdes);
			i = NULL;
			/*move up one line*/
			/*but if cursor at screen top, scroll screen down*/
			if (vert == 1)
			{
				INSERT_LINE;
			}
			else
				--vert;
			cursor(vert, m);
			REV_LINE_FEED;
			break;
			/* Cancel line_buf contents*/
		case CTRLU :
			m = m - i;        /* Adjust cursor*/
			cursor(vert, m);
			for (j = 0; j < i; j++)
				putchar(SPACE);        /*blank screen*/
			cursor(vert, m);        /*adjust cursor again*/
			i = NULL;        /* Zero line_buf*/
			break;
			/* Tab = Place spaces in array & on screen*/
		case CTRLI :
			if ((SCREEN_WIDTH - m) >= TAB)
				detab();
			cursor(vert, m);
			break;
			/* Clear screen*/
		case CTRLR :
			CLS;        /* Clear screen*/
			ruler();        /* Replace ruler line*/
			i = m = NULL;        /* Beginning of buffer*/
			vert = 1;        /* First line below ruler*/
			break;
			/* CR/LF*/
		case '\n' :
		case CTRLJ :
		case CTRLM :
			for (j = 0; j < i; j++)
				putc(line_buf[j], fdes);
			putc('\r', fdes);
			putc('\n', fdes);
			i = m = NULL;        /* Beginning of buffer*/
			if (vert < SCREEN_HEIGHT)
				++vert;        /* Increment vert counter*/
			ruler();
			cursor(vert, 0);
			break;
			/* Display instructions*/
		case CTRLP :
			CLS;
			disp_instructions();
			getchar();
			vert = 1;        /* First line below ruler*/
			i = m = NULL;        /* Beginning of buffer*/
			CLS;
			ruler();
			break;
		}
	}
	/* Final flush of buffer to file or LST:*/
	for (j = 0; j < i; j++)
		putc(line_buf[j], fdes);
}
/* End of typer*/



int cursor(v, h)        /* Televideo 920C cursor positioning*/
int v, h;
{
	putchar(ESC);
	putchar('=');
	putchar(v + 32);
	putchar(h + 32);
	return 1;
}
/* End of cursor*/



int detab()        /* Expands tabs into spaces until next tabstop*/
{
	do        /* Every tab expands to at least one space*/
	{
		line_buf[i++] = SPACE;        /* Put space in buffer*/
		putchar(SPACE);        /* Put space on screen*/
	}
	/* And check to see if we're on a tabstop now.*/
	while (((++m % TAB) != NULL) && (m <= SCREEN_WIDTH))
		;
	return 1;        /* If tabstop, return ok*/
}
/* End of detab*/



void ruler()        /* Put ruler line at screen top*/
{
	HOME;
	puts("L----!----!----!----!----!----!----!----!----!----!----!----!----!----!----!---R");
}
/* End of ruler*/



void disp_instructions()
{
	puts("TYPER Ver. 2.0  by Harvey G. Lord\n");
	puts("TYPER placed in the public domain by Harvey G. Lord (10/14/83)\n");
	puts("Modified by J. A. Rupley for printhead movement (05/07/85)\n");
	puts("Written for Televideo 920 terminal and printer with reverse linefeed.\n");
	puts("\n");
	puts("Simple line editor.  Sends one line at a time to the\n");
	puts("	LST:  device (printer) or to a disk file.\n");
	puts("\n");
	puts("Usage: TYPER            <== Prints instructions, output goes to LST:\n");
	puts("       TYPER PRINTER    <== No instructions, output to LST:\n");
	puts("       TYPER d:file.typ <== No instructions, output to disk file\n");
	puts("Editing Commands:\n");
	puts("       ^H, DEL         = Destructive backspace\n");
	puts("       ^U              = Cancel buffer contents\n");
	puts("       ^R              = Clear screen\n");
	puts("       ^P              = Display prompts\n");
	puts("       <cr>, ^J, ^M    = Send line to indicated device\n");
	puts("       ^Z              = End of session\n");
	puts("       ^C              = Abort session\n");
	puts("       ^S, ^D, ^E, ^X  = Immediate printhead movement\n");
	puts("                             after write of buffer contents.\n");
	puts("\n");
	puts("Press any key to continue.\n");
}



void disp_message()
{
	puts("\n");
	puts("\n");
	puts("Press any key when your printer is plugged in\n");
	puts("and ready to run.\n");
	puts("\n");
	puts("^Z ends and ^C aborts session\n");
	puts("\n");
}
