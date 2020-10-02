/* Screen editor:  configuration program
 *
 * Source:  config1.c
 * Version: June 4, 1981.
 */

/* Values of control keys go here */

int up;			/* value of cursor up key */
int down;		/* cursor down key */
int left;		/* cursor left key */
int right;		/* cursor right key */
int ins;		/* enter insert mode key */
int del;		/* delete character key */
int esc;		/* leave current mode key */
int abt;		/* undo editing key */
int altup;		/* alternate up key */
int altdn;		/* alternate down key */
int delline;		/* delete line key */
int govid;		/* enter video mode key */

/* Video screen  and printer characteristics go here */

int scrnl;		/* number of rows on screen */
int scrnw;		/* # of columns on screen */
int haseol;		/* has erase to end of line */
int hasel;		/* has erase line */
int hassup;		/* has hardware scroll up */
int hassdn;		/* has hardware scroll down */
int lwidth;		/* width of list device */

/* Define array which contains the code that the
 * user gives to do special screen functions.
 */

#define BYTEMAX	1000	/* size of byte array */
char bytes[BYTEMAX];
int bytec;		/* index of next free entry */

/* Define indices into bytes[] which point at start
 * of code for each special screen function.
 */

int gotoind;		/* index to gotoxy code */
int eolind;		/* erase to end of line */
int elind;		/* erase line */
int supind;		/* scroll up */
int sdnind;		/* scroll down */

/* Define return codes */

#define	YES	1	/* all ok */
#define NO	2	/* try again */
#define EXIT	3	/* stop the program */

/* Define special characters */

#define CR	13	/* carriage return */
#define LF	10	/* line feed */
#define	TAB	9	/* tab */

/* define output file index */

int	output;

/* This program has 5 parts.
 *
 * Part 1 asks which keys do which special functions.
 * Part 2 asks which special functions the screen has.
 * Part 3 asks what code sequences must be output
 *        to the screen to do the functions in part 2.
 * Part 4 creates the file ed1.ccc from the answers to
 *        Part 1.
 * Part 5 creates the file ed6.ccc from the answers to
 *        Parts 2 and 3.
 */

main()
{
	/* initialize byte count */
	bytec=0;
	/* sign on and give general information */
	signon();
	/* get keyboard information */
	while (part1()==NO) {
		;
	}
	/* get screen features */
	while (part2()==NO) {
		;
	}
	/* get control sequences for screen features */
	while (part3()==NO) {
		;
	}
	/* make sure we want to continue */
	blank();
	plc("You are now ready to create files ");
	pc("ed1.ccc and ed6.ccc.");
	plc("Do you want to proceed ?");
	if (yesno()==NO) {
		return;
	}
	/* write keyboard info to file ed1.ccc */
	if (part4()!=YES) {
		return;
	}
	/* write control sequences for screen features
	 * to file ed6.ccc
	 */
	part5();
	/* sign off and tell about compiling */
	signoff();
}

/* sign on and tell how to abort */

signon()
{
plc("Welcome to the configuration program for the ");
pc("screen editor.");
plc("Hit control-c to exit this program early.");
plc("Hit control-p to send output to the printer.");
}

/* part 1.  find out what keys will be used for
 *          special functions.
 */

part1()
{

blank();
plc("This section deals with your keyboard.");
plc("For each function key, enter the DECIMAL ");
pc("value of the ascii ");
plc("code that you want to assign to that function key.");
plc("Hit carriage return to indicate the default value.");

blank();
plc("up key:");
indent();
pc("This key usually moves the cursor up.  In insert mode");
indent();
pc("this key inserts a line above the current line.");
indent();
pc("The default is line feed (10).");
up=getval(10);

plc("down key:");
indent();
pc("This key usually moves the cursor down.  In insert mode");
indent();
pc("this key inserts a line below the current line.");
indent();
pc("The default is carriage return (13).");
down=getval(13);

plc("alternate up key:");
indent();
pc("This key usually inserts a line above the current line.");
indent();
pc("In insert mode it just moves the cursor up");
indent();
pc("The default is control-u (21).");
altup=getval(21);

plc("alternate down key:");
indent();
pc("This key usually inserts a line below the current line.");
indent();
pc("In insert mode it just moves the cursor down");
indent();
pc("The default is control-d (4).");
altdn=getval(4);

plc("left key:");
indent();
pc("This key moves the cursor left ");
pc("in all edit modes.");
indent();
pc("The default is back space (8).");
left=getval(8);

plc("right key:");
indent();
pc("This key moves the cursor right ");
pc("in all edit modes.");
indent();
pc("The default is control-r (18).");
right=getval(18);

plc("insert key:");
indent();
pc("This key enters insert mode.");
indent();
pc("The default is control-n (14).");
ins=getval(14);

plc("video key:");
indent();
pc("This key enters video mode.");
indent();
pc("The default is control-v (22).");
govid=getval(22);

plc("exit current mode key:");
indent();
pc("This key exits the current mode.");
indent();
pc("The default is esc (27).");
esc=getval(27);

plc("delete character key:");
indent();
pc("This key deletes the character to the left ");
pc("of the cursor.");
indent();
pc("The default is del (127).");
del=getval(127);

plc("delete line key:");
indent();
pc("This key deletes the line on which the cursor rests.");
indent();
pc("The default is control-z (26).");
delline=getval(26);

plc("abort key:");
indent();
pc("This key undoes the editing done on a line.");
indent();
pc("The default is control-x (24).");
abt=getval(24);

/* recap what the user has typed.
 * ask if everything is all right.
 */

plc("The values of the keyboard keys are:");
plc("up:           ");
putdec(up,3);
plc("down:         ");
putdec(down,3);
plc("alternate up: ");
putdec(altup,3);
plc("alt down:     ");
putdec(altdn,3);
plc("left:         ");
putdec(left,3);
plc("right:        ");
putdec(right,3);
plc("insert move:  ");
putdec(ins,3);
plc("video mode:   ");
putdec(govid,3);
plc("exit mode:    ");
putdec(esc,3);
plc("delete char:  ");
putdec(del,3);
plc("delete line:  ");
putdec(delline,3);
plc("abort:        ");
putdec(abt,3);

blank();
plc("are all values correct ?");
return(yesno());
}

/* get features of the video screen */

part2()
{

blank();
plc("This section deals with the video screen.");
plc("The screen MUST have a cursor positioning function,");
plc("but no other special screen functions are required.");

blank();
plc("screen length:");
indent();
pc("How many rows does your screen have ?");
indent();
pc("The default is 16.");
scrnl=getval(16);

blank();
plc("screen width:");
indent();
pc("How many columns does your screen have ?");
indent();
pc("The default is 64.");
scrnw=getval(64);

blank();
plc("printer width:");
indent();
pc("How many columns does your printer have ?");
indent();
pc("The default is 132.");
lwidth=getval(132);

blank();
plc("erase line:");
indent();
pc("Does your screen have a function which erases ");
indent();
pc("the line on which the cursor rests ?");
hasel=yesno();

plc("erase to end of line:");
indent();
pc("Does your screen have a function which erases ");
indent();
pc("from the cursor to the end of the line ?");
haseol=yesno();

plc("hardware scroll up: (line feed)");
indent();
pc("Does your screen have a function that scrolls the");
indent();
pc("screen up when the cursor is on the bottom line ?");
hassup=yesno();

plc("hardware scroll down:");
indent();
pc("Does your screen have a function which scrolls the");
indent();
pc("screen down when the cursor is on the top line ?");
hassdn=yesno();

blank();
plc("You have answered as follows: ");
plc("screen length:            ");
putdec(scrnl,1);
plc("screen width:             ");
putdec(scrnw,1);
plc("printer width:            ");
putdec(lwidth,1);
plc("erase line ?              ");
putyesno(hasel);
plc("erase to end of line ?    ");
putyesno(haseol);
plc("hardware scroll up ?      ");
putyesno(hassup);
plc("hardware scroll down ?    ");
putyesno(hassdn);

blank();
plc("are all these values correct ?");
return(yesno());
}

part3()
{
blank();
plc("This section asks you to supply code sequences for");
plc("the special functions that you said your screen had.");
plc("Enter each sequence one byte at a time.");
plc("End the sequence by typing just a carriage return.");
plc("Indicate each byte by giving a character expression.");
blank();
plc("Hit any character to get more instructions.");
getchar();

blank();
plc("An expression consists of terms separated by + or -.");
plc("Each term is either:");
plc("1.  a decimal number or");
plc("2.  an ascii character in single quotes or");
plc("3.  the letters x or y without quotes.");
plc("Use x and y only for the goto xy code sequence.");
plc("The x and y represent the desired cursor coordinates.");
plc("For example, a common sequence for goto x y is:");
plc("   27, '=', x+32, y+32");

blank();
plc("goto x y:");
indent();
pc("Enter code to position the cursor at column x and row y.");
indent();
pc("0,0 is the top left corner of the screen.");
getbytes(&gotoind);

blank();
if (hasel==YES) {

plc("erase line:");
indent();
pc("Enter code to erase the line on which the ");
pc("cursor rests.");
getbytes(&elind);

}

blank();
if (haseol==YES) {

plc("erase to end of line:");
indent();
pc("Enter code to erase from the cursor to the ");
pc("end of the line.");
getbytes(&eolind);

}

blank();
if (hassup==YES) {

plc("hardware scroll up: (line feed)");
indent();
pc("Enter code to scroll the screen up assuming ");
indent();
pc("the cursor is on the bottom line.");
getbytes(&supind);

}

blank();
if (hassdn==YES) {

plc("hardware scroll down:");
indent();
pc("Enter code to scroll the screen down assuming ");
indent();
pc("that the cursor is on the top line.");
getbytes(&sdnind);

}

}

part4()
{

if ((output=fopen("ed1.ccc","w"))==0) {
	plc("open error on ed1.ccc");
	return(EXIT);
}

comment();
plf("Screen editor:  special key definitions");
plf("Source:  ed1.ccc");
plf("This file was created by the configuration program");
endcom();

blankf();
comment();
plf("Define which keys are used for special edit functions.");
endcom();

blankf();
putdef("UP1",up);
putdef("DOWN1",down);
putdef("UP2",altup);
putdef("DOWN2",altdn);
putdef("LEFT1",left);
putdef("RIGHT1",right);
putdef("INS1",ins);
putdef("VID1",govid);
putdef("ESC1",esc);
putdef("DEL1",del);
putdef("ZAP1",delline);
putdef("ABT1",abt);

blankf();
comment();
plf("Define length and width of screen and printer.");
endcom();

blankf();
putdef("SCRNW",scrnw);
putdef("SCRNW1",scrnw-1);
putdef("SCRNL",scrnl);
putdef("SCRNL1",scrnl-1);
putdef("SCRNL2",scrnl-2);
putdef("LISTW",lwidth);
blankf();

fclose(output);
return(YES);
}

part5()
{

if ((output=fopen("ed6.ccc","w"))==0) {
	plc("Open error on ed6.ccc");
	return(EXIT);
}

comment();
plf("Screen editor:  terminal output module");
plf("Source:  ed6.ccc");
plf("This file was created by the configuration ");
pf("program.");
endcom();

blankf();
comment();
plf("Define the current coordinates of the cursor.");
endcom();

blankf();
plf("int outx, outy;");

blankf();
comment();
plf("Return the current coordinates of the cursor.");
endcom();

blankf();
plf("outgetx()");
beginf();
tab1f(); pf("return(outx);");
endf();

blankf();
plf("outgety()");
beginf();
tab1f(); pf("return(outy);");
endf();

blankf();
comment();
plf("Output one printable character to the screen.");
endcom();

blankf();
plf("outchar(c) char c;");
beginf();
tab1f(); pf("syscout(c);");
tab1f(); pf("outx++;");
tab1f(); pf("return(c);");
endf();

blankf();
comment();
plf("Position cursor to position x,y on screen.");
plf("0,0 is the top left corner.");
endcom();

blankf();
plf("outxy(x,y) int x,y;");
beginf();
tab1f(); pf("outx=x;");
tab1f(); pf("outy=y;");
putbytes(gotoind);
endf();

blankf();
comment();
plf("Erase the entire screen.");
plf("Make sure the rightmost column is erased.");
endcom();

blankf();
plf("outclr()");
if ((hasel==YES)+(haseol==YES)) {

beginf();
plf("int k;");
tab1f(); pf("k=0;");
tab1f(); pf("while (k<SCRNL) {");
tab2f(); pf("outxy(0,k++);");
tab2f(); pf("outdelln();");
tab1f(); pf("}");
tab1f(); pf("outxy(0,0);");
endf();

}
else {

beginf();
plf("int k;");
tab1f(); pf("outxy(0,0);");
tab1f(); pf("k=SCRNL*SCRNW;");
tab1f(); pf("while ((k--)>0) {");
tab2f(); pf("syscout(' ');");
tab1f(); pf("}");
tab1f(); pf("outxy(0,0);");
endf();

} /* end else */

blankf();
comment();
plf("Delete the line on which the cursor rests.");
plf("Leave the cursor at the left margin.");
endcom();

blankf();
plf("outdelln()");
if (hasel==YES) {

beginf();
putbytes(elind);
endf();

}
else {

beginf();
tab1f(); pf("outxy(0,outy);");
tab1f(); pf("outdeol();");
endf();

}

blankf();
comment();
plf("Delete to end of line.");
plf("Assume the last column is blank.");
endcom();

blankf();
plf("outdeol()");
if (haseol==YES) {

beginf();
putbytes(eolind);
endf();

}
else {

beginf();
plf("int k;");
tab1f(); pf("k=outx;");
tab1f(); pf("while (k++<SCRNW1) {");
tab2f(); pf("syscout(' ');");
tab1f(); pf("}");
tab1f(); pf("outxy(outx,outy);");
endf();

}

blankf();
comment();
plf("Return yes if terminal has indicated hardware scroll.");
endcom();

blankf();
plf("outhasup()");
beginf();
tab1f();
if (hassup==YES) {
	pf("return(YES);");
}
else {
	pf("return(NO);");
}
endf();

blankf();
plf("outhasdn()");
beginf();
tab1f();
if (hassdn==YES) {
	pf("return(YES);");
}
else {
	pf("return(NO);");
}
endf();

blankf();
comment();
plf("Scroll the screen up.");
plf("Assume the cursor is on the bottom line.");
endcom();

blankf();
plf("outsup()");
if (hassup==YES) {

beginf();
tab1f(); pf("/* auto scroll */");
tab1f(); pf("outxy(0,SCRNL1);");
putbytes(supind);
endf();

}
else {

beginf();
endf();

}

blankf();
comment();
plf("Scroll screen down.");
plf("Assume the cursor is on the top line.");
endcom();

blankf();
plf("outsdn()");
if (hassdn==YES) {

beginf();
tab1f(); pf("/* auto scroll */");
tab1f(); pf("outxy(0,0);");
putbytes(sdnind);
endf();

}
else {

beginf();
endf();

}

/* make sure last line ends with CR */
plf("");
fclose(output);
return(YES);
}

signoff()
{
plc("The configuration process is now complete.");
plc("You are now ready to compile the screen editor.");
return(YES);
}

eturn(NO);");
}
endf();

blankf();
plf("outhasdn()");
beginf();
tab1f();
if (hassdn==YES) {
	pf("return(YES                                                                                                                                