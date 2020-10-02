/*	graphics.c

	Version 3.0	29-May-80

  	special CRT graphics functions are included in the
	file GRAPHICS.CRL. this file contains most of the more
	common functions of the terminal (VT100) itself as
	well as a number of more general purpose drawing
	functions. versions from 3.0 use the alternate ROM
	character set.

		Function Summary
		----------------

1. chrmode(mode)

	sets the character attributes. by convention these
	are OFF,BOLD,UNDERLINE,BLINK,and REVERSE which are
	defined as '0','1','4','5',and '7' respectively.

2. clearl(mode)

	clears (erases) a line or part of a line according
	to the mode. by convention these are TO_END,TO_CURSOR,
	and ENTIRE which are defined as '0','1',and '2'
	respectively.

3. clears(mode)

	clears (erases) the screen (protocol as in 'clearl').

4. clrallt()

	clears all tabs.

5. clrtab()

	clear tab at current column.

6. cursor(row,column)

	position cursor at the given coordinates.

7. dheight(row,column,string)

	print the given string in double height (and double
	width) characters at the given cursor position.

8. diagbt(row,column,length,angle,scan_line)

	plot a diagonal line beginning at the given cursor
	position from the bottom left to the top right of
	the screen. by convention, the angles are ANGLE_30,
	ANGLE_60 which are defined as 30,45,and 60 respectively.
	scan_line is the initial line from which the diagonal
	is drawn. these are given as follows:

	30 degrees bt: 10,8,5,3
	30 degrees tb: 1,3,6,8
	45 degrees bt: 10,5
	45 degrees tb: 1,5
	60 degrees bt: 10
	60 degrees tb: 1

	the length is expressed as follows:

	30 degrees:    in 1/4 character slices
	45 degrees:    in 1/2 character slices
	60 degrees:    in one character slices
	
	the cursor comes to rest on the row that the next character
	would be printed.

9. diagtb(row,column,length,angle,scan_line)

	as in 'diagbt' but from the top right to the bottom
	left of the screen.

10. dwidth(row,column,string)

	print the given string in double width characters
	at the given cursor position.

11. entergraph(set)

	enter special graphics mode. by convention the character
	sets are SET_UK,SET_0,SET_1,and SET_2 which are defined
        as 'A','0','1',and '2' respectively.

12. exitgraph()

	exit all graphics modes (ie. enter USASCII set).

13. hline(row,column,length,scan_line)

	print a horizontal line of the specified length
	beginning at the given cursor position. the line
	is drawn on scan line which may be 1,3,5,7,9, or 10.

14. horiz(row,column,length,character,mode)

	print a horizontal string of the specified character
 	beginning at the given cursor position in the
	specified mode (character attribute(s)).

15. index()

	cursor down one row (same column).

16. leds(mode)

	turn the specified led(s) on or off. by convention
	the modes are OFF,ON1,ON2,ON3,ON4 which are defined
	as '0' through '4' respectively.

17. movcur(direction,length)

	move the cursor in the specified direction the given
	length. by convention the directions are UP,DOWN,
	RIGHT,LEFT which are defined as 'A','B','C',and 'D'
	respectively.

18. normsize(row,column,string)

	print the given string in single height and single
	width characters at the given cursor position.

19. plot(row,column,character)

	plot the given character at the specified cursor position.

20. plotgraph(row,column,character,set)

	plot the given character at the specified cursor position
	using the specified character set (see entergraph).

21. rindex()

	cursor up one row (same column).
	
22. rstcur()

	restore cursor to previously saved position with saved
	character attributes.

23. savcur()

	save cuurent cursor position and character attributes.

24. scroll(top,bottom)

	set boundaries of the scrolling region.

25. setmode(mode)

	set/reset a terminal mode. the currently implemented
	modes are NORMAL (80 columns), WIDE (132 columns),
	BLACK (normal-black screen), WHITE (white screen),
	KEYPAD (set keypad to function keys), NUMERIC (set
	keypad to normal-numeric mode). these are defined
	as '1' through '6' respectively.

26. settab()

	set tab at current column.

27. square(row,column,width,height,origin)

	plot a square or rectangle beginning at the given
	cursor position with the specified width and height.
	the origin is given as an integer as follows:

	  ((scan_row * 10) + scan_column)

	possible origins:

	  11:	scan_row 1,scan_column 1
	  14:   scan_row 1,scan_column 4
	  51:	scan_row 5,scan_column 1
	  54:   scan_row 5,scan_column 4

28. txtplot(row,column,string)

	print the given string at the specified cursor position
	with the given attributes mode (see chrmode).

29. vert(row,column,length,character,mode)

	print a vertical string of the specified character
	beginning at the given cursor position in the
	specified mode (character attribute(s)).

30. vline(row,column,length,scan_line)

	print a vertical line of the specified length
	beginning at the given cursor position. the
	line is drawn down scan_line which may be 1,4,or 7.

31. extdig(plot)

	a dedicated sub-function not for general use.

*/
#define RESET 0
#define ESC   27

#define	ANGLE_30	30
#define	ANGLE_45	45
#define	ANGLE_60	60

#define	OFF	'0'
#define NORMAL  '1'
#define WIDE    '2'
#define BLACK   '3'
#define WHITE   '4'
#define KEYPAD  '5'
#define NUMERIC '6'
#define SET_UK  'A'
#define SET_0	'0'
#define SET_1	'1'
#define SET_2	'2'

chrmode(mode)
char(mode);
{
	putchar(ESC); putchar('[');
	putchar(mode); putchar('m');
}

clearl(mode)
char mode;
{
	putchar(ESC); putchar('[');
	putchar(mode); putchar('K');
}

clears(mode)
char mode;
{
	putchar(ESC); putchar('[');
	putchar(mode); putchar('J');
}

clrallt()
{
	putchar(ESC); puts("[3g");
}

clrtab()
{
	putchar(ESC); puts("[g");
}

cursor(row,column)
int row, column;
{
	putchar(ESC); putchar('[');
	extdig(row); putchar(';');
	extdig(column); putchar('H');
}

diagbt(row,column,length,angle,scan_line)
int row, column, length, angle, scan_line;
{
	char diagstr[7];
	int i;

	strcpy(diagstr,"GHIJ[\\\'"); cursor(row, column); entergraph(SET_1);
	switch(angle) {
	  case ANGLE_30: switch(scan_line) {
			   case 10: i = 0; break;
			   case 8:  i = 1; break;
			   case 5:  i = 2; break;
			   case 3:  i = 3; break;
			   default: break;				
			 }
			 while (length--) {
			   putchar(diagstr[i]);
			   if (diagstr[i] == 'J') rindex();
			   if (i++ == 3) i = RESET;
			 }
			 break;
	  case ANGLE_45: if (scan_line == 10) i = 4;
			   else i = 5;
			 while (length--) {
			   putchar(diagstr[i]);
			   if (diagstr[i] == '\\') rindex();
			   if (i++ == 5) i = 4;
			 }
			 break;
	  case ANGLE_60: while (length--) { putchar(diagstr[6]); rindex(); }
			 break;
	  default:	 break;
	}
	exitgraph();
}

diagtb(row,column,length,angle,scan_line)
int row, column, length, angle, scan_line;
{
	char diagstr[7];
	int i;

	strcpy(diagstr,"CDEF]^("); cursor(row, column); entergraph(SET_1);
	switch(angle) {
	  case ANGLE_30: switch(scan_line) {
			   case 1:  i = 0; break;
			   case 3:  i = 1; break;
			   case 6:  i = 2; break;				
			   case 8:  i = 3; break;
			   default: break;
			 }
			 while (length--) {
			   putchar(diagstr[i]);
			   if (diagstr[i] == 'F') index();
			   if (i++ == 3) i = RESET;
			 }
			 break;
	  case ANGLE_45: if (scan_line == 1) i = 4;
			   else i = 5;
			 while (length--) {
			   putchar(diagstr[i]);
			   if (diagstr[i] == '^') index();
			   if (i++ == 5) i = 4;
			 }
			 break;
	  case ANGLE_60: while (length--) { putchar(diagstr[6]); index(); }
			 break;
	  default:	 break;
	}
	exitgraph();
}

dheight(row,column,string)
int row, column;
char *string;
{
	cursor(row, column); savcur();
	putchar(ESC); puts("#3"); puts(string);
	rstcur(); index();
	putchar(ESC); puts("#4"); puts(string);
}

dwidth(row,column,string)
int row, column;
char *string;
{
	cursor(row, column);
	putchar(ESC); puts("#6"); puts(string);
}

entergraph(set)
char set;
{
	putchar(ESC); putchar('('); putchar(set);
}

exitgraph()
{
	putchar(ESC); puts("(B");
}

hline(row,column,length,scan_line)
int row, column, length, scan_line;
{
	int h_line, i;

	cursor(row, column);
	switch(scan_line) {
	  case 1:       entergraph(SET_0); h_line = '\157'; break;
	  case 3:       entergraph(SET_0); h_line = '\160'; break;
	  case 5:       entergraph(SET_0); h_line = '\161'; break;
	  case 7:       entergraph(SET_0); h_line = '\162'; break;
	  case 9:       entergraph(SET_0); h_line = '\163'; break;
	  case 10:      entergraph(SET_2); h_line = '\137'; break;
	  default: break;
	}

	for (i = RESET; i < length; ++i) putchar(h_line); /* horiz line */
	exitgraph();
}

horiz(row,column,length,character,mode)
int row, column, length;
char character, mode;
{
	int i;

	cursor(row, column); chrmode(mode);
	for (i = RESET; i < length; ++i) putchar(character);
}

index()
{
	putchar(ESC); putchar('D');
}

leds(mode)
char mode;
{
	putchar(ESC); putchar('[');
	putchar(mode); putchar('q');
}

movcur(direction,length)
int length;
char direction;
{
	putchar(ESC); putchar('['); extdig(length); putchar(direction);
}

normsize(row,column,string)
int row, column;
char *string;
{
	cursor(row, column);
	putchar(ESC); puts("#5"); puts(string);
}

plot(row,column,character)
int row, column;
char character;
{
	cursor(row,column); putchar(character);
}

plotgraph(row,column,character,set)
int row,column;
char character,set;
{
	entergraph(set); plot(row,column,character); exitgraph();
}

rindex()
{
	putchar(ESC); putchar('M');
}

rstcur()
{
	putchar(ESC); putchar('8');
}

savcur()
{
	putchar(ESC); putchar('7');
}

scroll(top,bottom)
int top, bottom;
{
	putchar(ESC); putchar('[');
	extdig(top); putchar(';');
	extdig(bottom); putchar('r');
}

setmode(mode)
char mode;
{
	putchar(ESC);
	switch(mode) {

	case NORMAL : puts("[?"); putchar('3'); putchar('l'); break;
	case WIDE   : puts("[?"); putchar('3'); putchar('h'); break;
	case BLACK  : puts("[?"); putchar('5'); putchar('l'); break;
	case WHITE  : puts("[?"); putchar('5'); putchar('h'); break;
	case KEYPAD : putchar('='); break;
	case NUMERIC: putchar('>'); break;
	}
}
settab()
{
	putchar(ESC); putchar('H');
}

square(row,column,width,height,origin)
int row, column, width, height, origin;
{
	char graph_mode;
	int tl_corner, tr_corner, bl_corner, br_corner;
	int h_line, v_line;
	
	switch(origin) {
	  case 11: graph_mode = SET_1; tl_corner = '\171'; tr_corner = '\172';
		   bl_corner = '\170'; br_corner = '\167'; h_line = 1;
		   v_line = 1; break;
	  case 14: graph_mode = SET_2; tl_corner = '\171'; tr_corner = '\170';
		   bl_corner = '\167'; br_corner = '\166'; h_line = 1;
		   v_line = 4; break;
	  case 51: graph_mode = SET_2; tl_corner = '\174'; tr_corner = '\175';
		   bl_corner = '\172'; br_corner = '\173'; h_line = 5;
		   v_line = 1; break;
	  case 54: graph_mode = SET_0; tl_corner = '\154'; tr_corner = '\153';
		   bl_corner = '\155'; br_corner = '\152'; h_line = 5;
		   v_line = 4; break;
	  default: break;	
	}

	plotgraph(row,column,tl_corner,graph_mode);
	hline(row,column+1,width-2,h_line);
        plotgraph(row,column+width-1,tr_corner,graph_mode);
        vline(row+1,column,height-2,v_line);
	switch(origin) {
	  case 11: v_line = 7; h_line = 10; break;
	  case 14: h_line = 10; break;
	  case 51: v_line = 7; break;
	  default: break;
	}

        vline(row+1,column+width-1,height-2,v_line);
	plotgraph(row+height-1,column,bl_corner,graph_mode);
   	hline(row+height-1,column+1,width-2,h_line);
 	plotgraph(row+height-1,column+width-1,br_corner,graph_mode);
}

txtplot(row,column,string,mode)
int row,column;
char *string, mode;
{
	cursor(row, column); chrmode(mode); puts(string);
}

vert(row,column,length,character,mode)
int row, column, length;
char character, mode;
{
	cursor(row, column); chrmode(mode);

	int i;

	for (i = RESET; i < length; ++i) {
	savcur(); putchar(character); rstcur(); index();
	}
}

vline(row,column,length,scan_line)
int row, column, length, scan_line;
{
	int i, v_line;

	cursor(row, column);

	switch(scan_line) {
	  case 1:  v_line = '\144'; entergraph(SET_2); break;
	  case 4:  v_line = '\170'; entergraph(SET_0); break;
	  case 7:  v_line = '\150'; entergraph(SET_2); break;
	  default: break;
	}

	for (i = RESET; i < length; ++i) {
	savcur(); putchar(v_line); rstcur(); index();	/* vert line */
	}
	exitgraph();
}

extdig(plot)
int plot;
{
	int hundreds, tens, ones;

	hundreds = plot / 100; tens = (plot - (hundreds * 100)) / 10;
	if (hundreds > 0) {
	   ones = (plot - (hundreds * 100)) % (tens * 10);
	   putchar(hundreds + '0');putchar(tens + '0');
	}
	else if (tens > 0) {
	   ones = plot % (tens * 10);
	   putchar(tens + '0');
	}
	   else ones = plot;
	putchar(ones + '0');
}
