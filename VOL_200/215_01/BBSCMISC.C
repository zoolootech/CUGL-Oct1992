/*
	bbscmisc.c

	Support routines used by BBSc.c.
				Mike Kelly

	06/12/83 v1.0	written
	07/07/83 v1.0	updated
*/

#include "bdscio.h"
#include "bbscdef.h"


#define LASTDATE  " 07/07/83 "

#define PGMNAME "BBSCMISC "
#define VERSION " 1.0 "


strfill(buf,fillchar,length)	/* fill a string with fillchar */
char	*buf;			/*  for length -1 */
int	fillchar,
	length;
{
	while(--length)		/* really is length -1 */
	{
		*buf++ = fillchar;
	}
	*buf++ = '\0';		/* need room for this */
}


prthex(string)		/* printf a string showing each char and it's */
char	*string;	/*  hex value  ie.  A<41> */
{
	char	byte,
		byte1;

	while (*string)
	{
		byte = (*string++);
		byte1 = byte;
		if (byte < 0x20)
		{
			byte1 = ' ';
		}
		printf("%c<%02x>",byte1,byte);
	}
	printf("\n");
}

prtihex(integer)	/* like prthex, but for integers */
int	integer;
{

	printf("%09d<%04x>",integer,integer);
	printf("\n");
}

substr(from,to,start,length)	/* moves chars from "from" to "to" */
char	*from,			/*  starting at "start" for */
	*to;			/*  "length" number of chars */
int	start,		/* for beginning of string use 1, not 0 */
	length;
{
	int	cnt;

	cnt = 0;

	while(--start)		/* adjust sending field pointer */
	{
		from++;		
	}

	while((cnt < length) && (*to++ = *from++))	/* do the moving */
	{
		cnt++;		
	}
	
	*to = '\0';

}

putd(byte)	/* do direct console output */
char	byte;
{
	bdos(6,byte);
}	
/*	end of function		*/


savepos()		/* save current cursor position */
{
	putch(ESC);
	putch('j');
}
/*	end of function		*/

retpos()		/* returns cursor to previously saved position */
{
	putch(ESC);
	putch('k');
}
/*	end of function		*/

to(linex,coly)		/* positions cursor to line and column given */
int	linex,
	coly;
{
	if (linex == 25)
	{
		on25();
	}
	putch(ESC);
	putch('Y');
	putch(linex+31);
	putch(coly+31);
}
/*	end of function		*/

on25()			/* enable line 25 */
{
	putch(ESC);
	putch('x');
	putch('1');
}
/*	end of function		*/

off25()			/* disables line 25 */
{
	putch(ESC);
	putch('y');
	putch('1');
}
/*	end of function		*/

puts(s)			/* this function replaces the one in the */
char	*s;		/*   bds c library */
{
	while (*s)
	{
		putch(*s++);	/* putch instead of putchar */
	}			/*  don't want to look for keyboard hit */
}				/*  during put of a char */
/* 	end of function		*/


getline(buf,max)	/* get a line of input max. chars long */
int	max;
char	*buf;
{
	int	cnt,
		byte;
	char	bytex;

	cnt = 0;
#ifdef DEBUG
	if (debug)
	{
		printf("in getline\n");
		printf("  max=%05d  cnt=%05d\n",max,cnt);
	}
#endif
	byte = FALSE;
	while (++cnt <= max && byte != '\r' && byte != '\n')
	{
		while((byte = getd()) < ' ' || byte > '}')
		{
			if (byte == '\r')
			{
				*buf++ = byte;
				byte = '\n';
				break;
			}
		}
		*buf++ = byte;
		putd(byte);		/* echo good chars only */
	}

	if (cnt >= max)
	{
		byte = '\r';		/* <cr> */
		*buf++ = byte;
		putd(byte);

		byte = '\n';		/* <lf> */
		*buf++ = byte;
		putd(byte);
	}

	*buf++	= '\0';			/* tag \0 on end */
#ifdef DEBUG
	if (debug)
	{
		printf("  cnt=%05d\n",cnt);
	}
#endif
}
/*	end of function		*/

getd()		/* do direct console input - returns when char present */
{
	int	bytei;

	while ((bytei = bdos(6,0xff)) == 0x00)
	{
		;
	}
#ifdef DEBUG
	if (debug)
	{
		printf("<%02x>",bytei);
	}
#endif
	return(bytei);
}
/*	end of function		*/

/* putd(byte)	/* do direct console output */
char	byte;
{
	bdos(6,byte);
/*	if (debug)
	{
		printf("<%02x>",byte);
	}
*/
}	
/*	end of function		*/
*/


fldcpy(to,from)		/* just like strcpy, except puts \0 at end */
char	*to,		/*   of to string */
	*from;
{
	while(*to++ = *from++);
	*++to = '\0';		/* put \0 in to string */
}
/*	end of function	*/


clear()		/*	clear screen	*/
{
	putch(ESC);
	putch(CLEAR);
}
/*	end of function		*/

char putst(string)
char *string; 
{
/*	printf("\nstring=");
*/
	while(*string)
	{
		putchar(*string++);
	}
	putchar("\n");
}
/*	end of function		*/


itoa(str,n)		/* taken from float.c */
char *str;
{
	char *sptr;
	sptr = str;
	if (n<0) { *sptr++ = '-'; n = -n; }
	_uspr(&sptr, n, 10);
	*sptr = '\0';
	return str;
}
/*	end of function		*/


/*	end of program  	*/
