/* SCOPER.C    VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:45 PM */
/*
%CC1 $1.C 
*/
/* 
Description:

Library of functions for full screen input.

Minor modification of SCOPE, from CUG distribution disk:
	changed some control character representations;
	added tabs;
	clear and unprotect screen at entry, before set NOROLL and
		display template in <crtbuf>.

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

/*The SCOPE function is a full screen processor designed for a terminal with
a 24 x 80 capability. It currently runs on a SOROK 120 or 140 but can be 
adapted to run on other terminals such as TELEVIDEO and/or also changed to 
allow for different control characters. The general purpose of SCOPE is
twofold. It has one mode where it will allow n screenfulls of data to be 
manipulated (n is controlled by parameter size) and this data may be 
modified in typical full screen editor format. A delete and insert character,
delete and insert line, pick and put a line, and goto first or last page
capability is provided. Also an erase back one character is provided. The
up and down arrows as well as the carriage return may be used to control
scrolling. (up scrolling is nice but unfortunately down scrolling is not)
The left and right arrows will rotate on a single line.  There is built in 
error checks to prevent the arrows from going off either the beginning or
end of the data.  Also notice that there are no nulls on the scope, blanks
are used so that the arrows may be positioned at will without affecting the 
display. To return to the calling function you must do one of three special
sequences (RETURN1,RETURN2,RETURN3). Which sequence is chosen is returned to
the calling function. Option is set to zero.
   A second option of SCOPE is a read-only mode so that SCOPE may be used to
display lists from which a choice is to be made.  In this mode only the curser
control keys are active and any other key will be ignored. The SCOPE program 
returns the character position to which the curser is last set so that the 
procedure is merely to set the curser to the line of interest and return to
the calling program. Option is set to non-zero.
   The other two parameters are crtbuf, an array large enough to hold all
of the characters of interest. e.g. if a 2-screenful system is applied then
array should be at least 3840 characters and size should be set to 3840.
   Notice that SCOPE is completely controlled by function 6 of CPM or MPM.
(It has different code for each OS) and therefore any character is legal.
An abort is recognized but it merely returns a -1 to the calling function
and must be dealt with there.
*/

	/* page eject */

#define LOAD bdos(6,'\033');bdos(6,'=');
#define NOROLL bdos(6,'\033');bdos(6,'&');
#define ROLL bdos(6,'\033');bdos(6,'\047');
#define CLRSCOPE bdos(6,'\033');bdos(6,'*');
#define BLANK 32
#define HOME 30			/*CTRL-^*/
#define UP 11			/*four arrow keys*/
#define DOWN 10         	
#define FORWARD 12		
#define BACK 8			
#define CR 13 
#define TAB 9			/*CTRL-I*/
#define FIRST 'R' - 0x40
#define LAST 'C' - 0x40
#define OPEN 'N' - 0x40
#define CLOSE 'Y' - 0x40
#define PICK 'Q' - 0x40
#define PUT 'W' -0x40
#define INSERT 'V' - 0x40
#define GOBBLE 'G' - 0x40
#define RUB 127			/*DEL*/
#define ABORT 'Z' -0x40
#define OUT 0x1b		/*ESC*/	
#define OUT1 0x1b		/*ESC*/
#define OUT2 'U' - 0x40
#define OUT3 0x1b		/*ESC*/

scope(crtbuf, curser, size, opt)
char crtbuf[];
int curser;
int size;
int opt;
{
	char b;
	int c;
	int first;
	int insert;
	char stack[21];
	char pickbuf[80];

	stack[20] = call(5, 0, 0, 12, 0) > 255;
	CLRSCOPE
	    NOROLL
	    insert = first = 0;
	setmem(pickbuf, 80, ' ');
	stack[0] = stack[1] = 2;
	outbuf(crtbuf, first, curser);
	/*OUT = ESC*/
	while ((b = getbyte(stack)) != OUT)
	{
		/*OUT1 = control X, OUT2 = control U, OUT3 = ESC*/
		if (b == OUT1 || b == OUT2 || b == OUT3)
			break;
		inbyte(stack);
		switch (b)
		{
		case 0 :        /*nothing typed*/
			continue;
		case LAST :        /*last page (control C)*/
			fix(crtbuf, curser, insert);
			first = curser = size - 1920;
			outbuf(crtbuf, first, curser);
			break;
		case ABORT :        /*control Z (abort)*/
			CLRSCOPE
			    return (-1);
		case PUT :        /*put (control W)*/
			if (opt)
				continue;
			c = open1(crtbuf, curser, size);
			movmem(pickbuf, &crtbuf[c], 80);
			outbuf(crtbuf, first, curser);
			break;
		case PICK :        /*pick (control Q)*/
			if (opt)
				continue;
			c = (curser / 80) * 80;
			movmem(&crtbuf[c], pickbuf, 80);
			continue;
		case BACK :        /*left arrow*/
			fix(crtbuf, curser, insert);
			if ((curser % 80) == 0)
			{
				curser += 79;
				setcurs(curser, first);
				break;
			}
			curser--;
			bdos(6, b);
			break;
		case GOBBLE :        /*gobble (control G)*/
			if (opt)
				continue;
			c = 79 - (curser % 80);
			movmem(&crtbuf[curser + 1], &crtbuf[curser], c);
			crtbuf[curser + c] = ' ';
			liner(crtbuf, curser, c + 1, first, stack);
			setcurs(curser, first);
			break;
		case TAB :        /*tab (control I)*/
			fix(crtbuf, curser, insert);
			curser += (8 - (((curser % 80) % 8)));
			if ((curser % 80) < 8)
				curser -= 80;
			setcurs(curser, first);
			break;
		case FORWARD :        /*right arrow*/
			fix(crtbuf, curser, insert);
			if ((++curser % 80) == 0)
			{
				curser -= 80;
				setcurs(curser, first);
				break;
			}
			bdos(6, b);
			break;
		case UP :        /*up arrow*/
			fix(crtbuf, curser, insert);
			if ((curser -= 80) < 0)
			{
				curser += 80;
				continue;
			}
			if (curser < first)
			{
				first -= 80;
				outbuf(crtbuf, first, curser);
				break;
			}
			bdos(6, b);
			break;
		case DOWN :        /*down arrow*/
			fix(crtbuf, curser, insert);
			curser += 80;
			if (curser >= size)
			{
				curser -= 80;
				continue;
			}
			if (curser >= (first + 1920))
			{
				first += 80;
				outline(crtbuf, first + 1840, 80, first);
				setcurs(curser, first);
				break;
			}
			bdos(6, b);
			break;
		case CR :        /*carriage return*/
			if (opt)
				return (curser);
			fix(crtbuf, curser, insert);
			c = curser % 80;
			curser = curser + 80 - c;
			if (curser >= size)
			{
				curser = curser - 80 + c;
				continue;
			}
			if (curser >= (first + 1920))
			{
				first += 80;
				outline(crtbuf, first + 1840, 80, first);
				setcurs(curser, first);
				break;
			}
			bdos(6, b);
			bdos(6, DOWN);
			break;
		case FIRST :        /*first page (control R)*/
			fix(crtbuf, curser, insert);
			first = curser = 0;
			outbuf(crtbuf, first, curser);
			break;
		case CLOSE :        /*close (control Y)*/
			if (opt)
				continue;
			fix(crtbuf, curser, insert);
			c = (curser / 80) * 80;
			movmem(&crtbuf[c + 80], &crtbuf[c], size - c);
			setmem(&crtbuf[size - 80], 80, ' ');
			outbuf(crtbuf, first, curser);
			break;
		case OPEN :        /*open (control N)*/
			if (opt)
				continue;
			open1(crtbuf, curser, size);
			outbuf(crtbuf, first, curser);
			break;
		case INSERT :        /*insert (control V)*/
			if (opt)
				continue;
			if ((++insert & 1) == 0)
			{
				bdos(6, crtbuf[curser]);
				bdos(6, BACK);
			}
			break;
		case HOME :        /*home (control ^)*/
			fix(crtbuf, curser, insert);
			curser = first;
			setcurs(0, 0);
			break;
		case RUB :        /*rub (delete key)*/
			if (opt)
				continue;
			if ((curser % 80) == 0)
				continue;
			fix(crtbuf, curser, insert);
			bdos(6, BACK);
			bdos(6, BLANK);
			bdos(6, BACK);
			crtbuf[--curser] = ' ';
			break;
		default :
			/*printing characters */
			if (opt || b < 32 || b > 126)
				continue;
			if (insert & 1)
			{
				if ((c = curser % 80) == 79)
					continue;
				movmem(&crtbuf[curser], &crtbuf[curser + 1], 79 - c);
				crtbuf[curser] = b;
				bdos(6, b);
				liner(crtbuf, ++curser, 79 - c, first, stack);
				setcurs(curser, first);
				break;
			}
			if (++curser >= (first + 1920))
			{
				crtbuf[--curser - 1] = b;
				bdos(6, b);
				setcurs(curser, first);
				break;
			}
			crtbuf[curser - 1] = b;
			bdos(6, b);
			break;
		}
		if (insert & 1)
		{
			bdos(6, 60);
			bdos(6, BACK);
		}
	}
	CLRSCOPE
	    if (opt)
		return (curser);
	return (b);
}

	/* page eject */

setcurs(curser, first)
int curser;
int first;
{

	curser = curser - first;
	LOAD
	    bdos(6, curser / 80 + 32);
	bdos(6, curser % 80 + 32);
}

outbuf(crtbuf, first, curser)
char crtbuf[];
int first;
int curser;
{
	int i;

	bdos(6, HOME);
	for (i = 0; i < 1920; i++)
		bdos(6, crtbuf[i + first]);
	setcurs(curser, first);
}

outline(crtbuf, spot, nbr, first)
char crtbuf[];
int spot;
int nbr;
int first;
{
	int i;

	ROLL
	    bdos(6, DOWN);
	NOROLL
	    setcurs(spot, first);
	for (i = 0; i < nbr; i++)
		bdos(6, crtbuf[i + spot]);
}

fix(crtbuf, curser, insert)
char crtbuf[];
int curser;
int insert;
{

	if (insert & 1)
	{
		bdos(6, crtbuf[curser]);
		bdos(6, BACK);
	}
}

liner(crtbuf, spot, nbr, first, stack)
char crtbuf[];
int spot;
int nbr;
int first;
char stack[];
{
	int i;

	setcurs(spot, first);
	for (i = 0; i < nbr; i++)
	{
		bdos(6, crtbuf[i + spot]);
		inbyte(stack);
	}

}

getbyte(stack)
char stack[];
{
	char b;

	if (stack[0] == stack[1])
		return (0);
	b = stack[stack[1]];
	if (++stack[1] == 20)
		stack[1] = 2;
	return (b);
}

inbyte(stack)
char stack[];
{

	if (stack[20])
		if (bdos(6, 254) == 0)
			return (0);
	if ((stack[stack[0]] = bdos(6, 255)) == 0)
		return (0);
	if (++stack[0] == 20)
		stack[0] = 2;
	return (1);
}

open1(crtbuf, curser, size)
char crtbuf[];
int curser;
int size;
{
	int c;

	c = (curser / 80) * 80;
	movmem(&crtbuf[c], &crtbuf[c + 80], size - c - 80);
	setmem(&crtbuf[c], 80, ' ');
	return (c);
}

