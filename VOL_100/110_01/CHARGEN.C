/*	Character Generator/Editor

	by Bob Pasky
	36 Wiswall Rd.
	Newton Centre, MA 02159

	Originally written for Imsai VIO video board to create
	data in the required format for burning its EPROM
	character sets.

	The program uses the VIO graphic mode to display an
	enlarged version of a character's dot matrix.
	The character matrix is 7 dots horizontally by 10 rows
	vertically.

	Each of the 256 character positions can be displayed, one
	at a time, modified as desired, and saved in memory.
	Once all changes have been made, the data can be formatted
	and saved as a binary file named "CHARSET".

	This file, of course, can be retrieved later and unformatted
	in order to re-edit the set.

	Additionally, the entire character set can be sent to
	a line printer for a hard-copy of the character matrices.

	To begin, you should "get" the file (type 'y' to confirm),
	then "unformat" it. Advance or back up the current character
	display until you reach the character code you wish to edit.
	"Load" the charater into the display area. Move the cursor
	to the desired point in the matrix and strike 'b' or 'd' for
	a bright (dot) or dim (no dot) point at that location.
	Continue until the graphic is in its desired form, then
	"save" it back into the character inventory. Advance or back
	up to any other characters to be edited. Load it, edit it, and
	save it. When all editing is complete, you must "format" the
	inventory before "filing" it to disk.
	The inventory must be in "unformatted" form to print it.

	Compiled using BDS-C 1.41, the program uses standard library
	functions. However, certain addresses may need to be modified
	for your particular VIO and display handlers. (I do not have
	the Imsai video driver firmware which normally expects the
	VIO to be addressed at 0xE000.)

	I use several simple graphic characters for the character
	display -- open box and solid boxes formed with two adjacent
	character positions, and a rule around the character display
	area formed by horizontal and vertical rules and matching
	corners. If you don't have equivalent graphics, you might try
	using the alternative ASCII characters shown in their
	"define" statements.

	The program should be easily adapted for any
	memory-mapped display with 2708/2716-type character ROMs.

	I have, in fact, modified this program to output in the
	format required for the Hazeltine 1500 series terminals.
*/

#include "bdscio.h"

#define DISPLAY 0xA000	/* VIO display address		*/
#define CHOME 1		/* command to home VIO cursor	*/
#define BRKCHR 0x1B	/* escape from printer listing	*/

#define GRAPHMODE 0xc	/* graphics mode for VIO ctrlr	*/
/*	cursor character in two parts left & right:	*/
/*	these 2 characters next to one another form an open box	*/
#define CURLEFT 0xC3	/* alternate: '<'	*/
#define CURRIGHT 0xC9	/* alternate: '>'	*/
/*	displayed point in two parts left & right:	*/
/*	these 2 characters next to one another form a solid box	*/
#define DOTLEFT 0x86	/* alternate: '['	*/
#define DOTRIGHT 0xB0	/* alternate: ']'	*/
/*	elements of box surrounding display area	*/
#define CVERT 0xC5	/* alternate: '|'	*/
#define CHORIZ 0xCA	/* alternate: '-'	*/
#define CTOPLEFT 0xC6	/* alternate: '+'	*/
#define CTOPRIGHT 0xCC	/* alternate: '+'	*/
#define CBOTLEFT 0xC3	/* alternate: '+'	*/
#define CBOTRIGHT 0xC9	/* alternate: '+'	*/

#define BUFSIZ 3072	/* char. description buffer	*/
#define LISTOUT 5	/* CP/M printer command		*/

char	cset[256][10];		/* character set descriptions	*/
char	garbage[256][2];	/* extra space for unused lines	*/
int	cchar;		/* current character displayed	*/
int	topleft;	/* address of top left of char	*/
int 	cuaddr,		/* cursor address	*/
	homeaddr,	/* address of home	*/
	cx,cy;		/* cursor x,y position	*/
char	cuchleft,	/* cursor char. left	*/
	cuchrght;	/* cursor char. right	*/
char	hxstr[16];	/* list of hex codes	*/

main()
{
int x;

	init();
	home();		/* place cursor at home	*/
	swapcur();	/* turn cursor on	*/
	strcpy (hxstr,"0123456789ABCDEF");
	dcchar();	/* display ccurrent char info	*/
	setmem(cset,BUFSIZ,0);	/* clear buffer area	*/

	while ( (x=getkbd()) != 'q')
	{
	if(cuchleft!=CURLEFT) {
		poke(cuaddr,cuchleft);
		poke(cuaddr+1,cuchrght);
		}
	switch (x)
		{
	case 'h': left();	/* move cursor left */
		break;
	case 'k': right();	/* and right */
		break;
	case 'j': down();	/* or down */
		break;
	case 'u': up();		/* or up */
		break;
	case 'b': bright();	/* place dot at cursor position */
		break;
	case 'd': dim();	/* remove dot from cursor pos'n */
		break;
	case 'm': home();	/* home cursor */
		break;
	case 'x': clrchar();	/* clear all dots from character */
		break;
	case 's': store();	/* store current character */
		break;
	case 'l': load();	/* get current character */
		break;
	case '[': retreat();	/* decrement current character */
		break;
	case ']': advance();	/* increment current character */
		break;
	case 'f': file();	/* save as file CHARSET */
		break;
	case 'g': getfile();	/* get file CHARSET */
		break;
	case 'p': format();	/* format for output */
		break;
	case 'o': unformat();	/* unformat for editing */
		break;
	case 'q': poke (0xa7ff,8);	/* changes VIO-C to alpha mode	*/
		exit();		/* quit altogether */
	case 'c': convert();	/* list on console or printer */
		break;
	default: break;
		}
	swapcur();
	}
	poke (0xa7ff,8);	/* change to alpha mode	*/
}

getkbd()	/* gets char from kbd, keeping the VIO drivers	*/
		/* from cluttering up the screen with the echo	*/
		/* of the keyboard characters.			*/
{
	return tolower (bios(3,0));
}

advance()	/* advance the current displayed character	*/
{
	if ((cchar++)>255)cchar=0;
	dcchar();
}
retreat()	/* go back one displayed character		*/
{
	if ((cchar--)<0)cchar=255;
	dcchar();
}
dcchar()	/* displays current character info		*/
{
int i,hx;

	txtplot ("CHARACTER IS ",3,10,0);
	i=strlen("character is x");
	plot(3,10+i,cchar);
	hx=hxstr[cchar/16];
	plot(3,12+i,hx);
	hx=hxstr[cchar%16];
	plot(3,13+i,hx);
}

store()		/* save current character description	*/
{
int i,j;
	for (i=0;i<10;i++)
		cset[cchar][i]=pack(i);
	txtplot ("Character Stored  ",6,10,0);
}

load()		/* load character description		*/
{
int i,j,x;
	clrchar();
	while (cy<9) up();

	for (i=0; i<10; i++)
		{
		x=cset[cchar][i];
		plot(5,(3*i)+10,hxstr[x/16]);
		plot(5,(3*i)+11,hxstr[x%16]);

		for (j=0; j<7; j++)
			{
			if (! ((x>>j)&1) ) bright();
			right();
			}
		while (cx) left();
		down();
		}
	txtplot ("Character Loaded   ",6,10,0);
}

pack(i)		/* reads single "raster" line of display	*/
		/* returns byte formatted for VIO PROM		*/
int i;
{
int j,c,x;
	c=0;
	for (j=0; j<7; j++)
		{
		c=c>>1;
		x=peek(topleft+(80*i)+(2*j)+81);
		if (x==DOTLEFT) c+=64;
		if (x==DOTRIGHT) c+=64;
		}
	return ~(c|128);
}

format()	/* converts descriptions into proper order	*/
		/* for PROM burning.				*/
{
char *pp;
int i,j;
char pset[BUFSIZ];

/*	format for VIO ROMs:
	requires 3 2708 EPROMs:
	1.  top 8 rows of characters 0-127
	2.  top 8 rows of characters 128-255
	3.  bottom 2 rows of characters 0-255
	Row one of each of the 128 characters is output, then
	row two of each character, etc.
	For the 3rd ROM, the 9th row for chars 0-127 is output,
	then the 10th row. Blanks fill the next two rows, then
	the 9th and 10th rows for chars 128-255. Two more
	blank rows complete the 3rd ROM.

	The hardware uses negative logic: a 0 turns the dot on,
	a 1 means no dot. The left-most dot for each character
	is controlled by the lsb of the byte; the right-most dot
	is bit 6; bit 7 is always 0.
*/
	txtplot ("Formatting data    ",6,10,0);
	pp=pset;
	for (j=0;j<8;j++)
		for (i=0;i<128;i++)
			{
			*pp=cset[i][j];
			pp++;
			}
	for (j=0;j<8;j++)
		for (i=128;i<256;i++)
			{
			*pp=cset[i][j];
			pp++;
			}
	for (j=8;j<10;j++)
		for (i=0;i<128;i++)
			{
			*pp=cset[i][j];
			pp++;
			}
	for (i=0x100;i<0x200;i++)
		{
		*pp=0xFF;
		pp++;
		}
	for (j=8;j<10;j++)
		for (i=128;i<256;i++)
			{
			*pp=cset[i][j];
			pp++;
			}

	movmem(pset,cset,BUFSIZ);
	txtplot ("Data formatted    ",6,10,0);
}
unformat()	/* converts formatted data (from disk file)	*/
		/* so it can be manipulated by program		*/
{
char *pp;
int i,j;
char pset[BUFSIZ];

	txtplot ("Unformatting data ",6,10,0);
	pp=pset;
	movmem(cset,pset,BUFSIZ);
	for (j=0;j<8;j++)
		for (i=0;i<128;i++)
			{
			cset[i][j]=*pp;
			pp++;
			}
	for (j=0;j<8;j++)
		for (i=128;i<256;i++)
			{
			cset[i][j]=*pp;
			pp++;
			}
	for (j=8;j<10;j++)
		for (i=0;i<128;i++)
			{
			cset[i][j]=*pp;
			pp++;
			}
	pp+=0x100;
	for (j=8;j<10;j++)
		for (i=128;i<256;i++)
			{
			cset[i][j]=*pp;
			pp++;
			}

	txtplot ("Data unformatted  ",6,10,0);
}

file()		/* saves file, but just in case user hit key by */
		/* mistake, it asks confirmation first		*/
{
int fd;
	txtplot ("Save file? (y/n)  ",6,10,0);
	if(getkbd()!='y') {
		txtplot ("                  ",6,10,0);
		return; }
	unlink("CHARSET");
	if((fd=creat("CHARSET"))<0) {
		printf("\nERROR: Can't create file.");
		exit();
		}
	write (fd,cset,BUFSIZ/128);
	close (fd);
	txtplot ("CHARSET saved      ",6,10,0);
}

getfile()	/* retrieves a file, but just in case user hits	*/
		/* key by mistake, asks confirmation first.	*/
{
int fd;
	txtplot ("Get file? (y/n)      ",6,10,0);
	if(getkbd()!='y') {
		txtplot ("                     ",6,10,0);
		return; }
	fd=open ("CHARSET",0);
	read (fd,cset,BUFSIZ/128);
	close (fd);
	txtplot ("CHARSET retrieved    ",6,10,0);
}

bright()	/* turn on dot		*/
{
	cuchleft=CURLEFT;
	cuchrght=CURRIGHT;
	poke(cuaddr,DOTLEFT);
	poke(cuaddr+1,DOTRIGHT);
	txtplot ("Bright            ",6,10,0);

}

dim()		/* turn off dot		*/
{
	poke (cuaddr,' ');
	cuchleft=CURLEFT;
	cuchrght=CURRIGHT;
	poke(cuaddr+1,' ');
	txtplot ("Dim               ",6,10,0);
}

cdisplay()		/* inserts cursor display	*/
{
	if(cuchleft!=CURLEFT) {
		poke (cuaddr,cuchleft);
		poke (cuaddr+1,cuchrght);
		}
	getchar();
	cchar++;
	clrchar();
}

left()		/* move cursor left	*/
{
	if (cx==0) return;
	cx--;
	cuaddr-=2;
	cuchleft=CURLEFT;
	cuchrght=CURRIGHT;
	txtplot ("                  ",6,10,0);
}

right()		/* move cursor right	*/
{
	if (cx>=6) return;
	cx++;
	cuaddr+=2;
	cuchleft=CURLEFT;
	cuchrght=CURRIGHT;
	txtplot ("                  ",6,10,0);
}

up()		/* move cursor up	*/
{
	if (cy>=9) return;
	cy++;
	cuaddr-=80;
	cuchleft=CURLEFT;
	cuchrght=CURRIGHT;
	txtplot ("                  ",6,10,0);
}

down()		/* move cursor down	*/
{
	if (cy==0) return;
	cy--;
	cuaddr+=80;
	cuchleft=CURLEFT;
	cuchrght=CURRIGHT;
	txtplot ("                  ",6,10,0);
}

home()		/* move cursor to home pos'n	*/
{
	cuaddr=homeaddr;
	cx=0;
	cy=3;
	cuchleft=CURLEFT;
	cuchrght=CURRIGHT;
	txtplot ("                  ",6,10,0);
}

clrchar()	/* clear out character display	*/
{
int i,j;
	putchar(1);
	for (i=1;i<11;i++)
		for  (j=1;j<15;j++)
			plot (10+i,36+j,' ');
	for (i=1;i<15;i++)	plot (18,36+i,'.');
	home();
	txtplot ("Character cleared ",6,10,0);
}

swapcur()	/* swap cursor with what's "under" it	*/
{
char temp;
	temp=peek(cuaddr);
	poke(cuaddr,cuchleft);
	cuchleft=temp;
	temp=peek(cuaddr+1);
	poke(cuaddr+1,cuchrght);
	cuchrght=temp;
}

init()		/* initialization	*/
{
	int i,j,k;

	setmem (cset,BUFSIZ,0);	/* clear character set vector */
	setplot(DISPLAY,24,80);	/* set up for 80x24	*/
	clrplot();
	poke (DISPLAY+0x7FF,GRAPHMODE);/* set VIO graphics mode	*/
	topleft=DISPLAY+(10*80)+36;
	homeaddr=topleft+1+(80*7);

	plot (10,36,CTOPLEFT);	/* set up outline of char. display */
	for (i=1;i<15;i++)	plot (10,36+i,CHORIZ);
	plot (10,36+15,CTOPRIGHT);
	for (i=1;i<11;i++)
		{
		plot (10+i,36,CVERT);
		plot (10+i,36+15,CVERT);
		}
	plot (21,36,CBOTLEFT);
	for (i=1;i<15;i++)	plot (21,36+i,CHORIZ);
	plot (21,36+15,CBOTRIGHT);
	cchar=0x20;
	clrchar();
	cuchleft=CURLEFT;
	cuchrght=CURRIGHT;
	home();
	putchar(1);	/* tell monitor's cursor to go home	*/
	txtplot("c - cset list",7,0,0);
	txtplot("d - dim",8,0,0);
	txtplot("f - file cset",9,0,0);
	txtplot("g - get cset",10,0,0);
	txtplot("h - left cursor",11,0,0);
	txtplot("j - down cursor",12,0,0);
	txtplot("k - right cursor",13,0,0);
	txtplot("l - load char",14,0,0);
	txtplot("m - home cursor",15,0,0);
	txtplot("o - unformat",16,0,0);
	txtplot("p - format",17,0,0);
	txtplot("q - quit",18,0,0);
	txtplot("s - store char",19,0,0);
	txtplot("u - up cursor",20,0,0);
	txtplot("x - clear char",21,0,0);
	txtplot("] - increment char",22,0,0);
	txtplot("[ - decrement char",23,0,0);

}

int listf;

convert()	/* once converted all characters to	*/
		/* a different format. Now its used to	*/
		/* print them on display and optionally,*/
		/* a line printer			*/
{
int i,j,k;

	txtplot ("Printing cset      ",6,10,0);

/*	to send listing to a file instead of printer:
**	unlink("CHARSET.LST");
**	if((fdc=fcreat("CHARSET.LST",iobuf))<0) {
**		printf("\nERROR: can't create LST file");
**		goto endconvert;
**		}
*/
	txtplot("Listing output?   ",6,10,0);
	if(getkbd()=='y') listf=1;
	else listf=0;
	for (i=0;i<256;i+=4)
		{
		if ((!bios(2,0)) && (bios(3,0) == BRKCHR)) break;
		if (!(i % 16)) putf(0xC);
		putf(' '); putf(' ');
		for (k=i;k<i+4;k++)
			{
			putf(hxstr[k/16]); putf(hxstr[k%16]);
			if ((k+1)%4) hspace();
			}
		hline();
		for (j=0;j<10;j++)
			{
			putf(0xD); putf (0xA);
			sendchr(cset[i][j]);
			sendchr(cset[i+1][j]);
			sendchr(cset[i+2][j]);
			sendchr(cset[i+3][j]);
			}
		hline();
		sendspc();
		}
/*	end of file output (if list to file is enabled):
**	fflush(iobuf);
**	close (fdc);
*/
	if (listf) init();	/* set up screen again */

	txtplot ("Listing done   ",6,10,0);
endconvert: return;
}

hline()		/* do a line of output to printer	*/
{
int i,j;
	putf(0xD); putf(0xA);
	for (i=0;i<4;i++)
		{
		putf (' '); putf (' ');
		for (j=0;j<17;j++)
			putf('-');
		putf (' ');
		}
}

hspace()	/* spacing		*/
{
int i;
	for (i=0;i<18;i++)
		putf(' ');
}

sendchr(c)	/* prints a single character description	*/
char c;
{
int i,j,k;
	putf(' '); putf(' '); putf('|'); putf (' ');
	for (i=0;i<7;i++)
		{
		if ( (c>>i) & 1) putf (' ');
		else putf ('*');
		putf (' ');
		}
	putf ('|'); putf(' ');
}

sendspc()	/* vertical spaces	*/
{
	putf(0xD); putf(0xA);
	putf(0xD); putf(0xA);
	putf(0xD); putf(0xA);
}

putf(c)		/* byte output, goes to display and	*/
		/* optionally to printer		*/
char c;
{
	if (c!=0xA) putchar(c);
	if (listf) bdos(LISTOUT,c);
}

