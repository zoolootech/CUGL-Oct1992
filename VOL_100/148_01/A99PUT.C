/*
	TMS9900/99105  Cross-Assembler  v. 1.0

	January, 1985

	Original 6800 version Copyright (c) 1980 William C. Colley, III.
	Modified for the TMS9900/99105  Series by Alexander Cameron.

File:	a99put.c

List and hex output routines.
*/

/*  Get globals:  */

#include "a99.gbl"

/*
Function to form the list output line and put it to
the list device.  Routine also puts the line to the
console in the event of an error.
*/

lineout()
{
	char tbuf[25], *tptr, *bptr, count;
	setmem(tbuf,24,' ');
	tbuf[24] = '\0';
	tptr = tbuf;
	*tptr++ = errcode;
	tptr++;
	if (hexflg != NOCODE) puthex4(address,&tptr);
	else tptr += 4;
	tptr += 3;
	count = 0;
	bptr = binbuf;
	while (TRUE)
	{
		if (count == nbytes || count != 0 && count % 4 == 0)
		{
			putlin(tbuf,&lstbuf);
			if (count > 4) putchr('\n',&lstbuf);
			else putlin(linbuf,&lstbuf);
			if (lstbuf.fd != CONO && errcode != ' ')
			{
				puts(tbuf);
				if (count >= 5) putchar('\n');
				else
				 	{linbuf[28]='\n';
					 linbuf[29]='\0';	
					  puts(linbuf);
					}
			}

			tptr = tbuf + 2;
			puthex4(address,&tptr);
			setmem(tptr,14,' ');
			tptr += 3;
		}
		if (count == nbytes) return;
		count++;
		address++;
		puthex2(*bptr++,&tptr);
		if( count % 2 == 0)tptr++;
	}
}

/*
Function to form the hex output line and put it to
the hex output device.
*/

hexout()
{
	char count, *bptr;
	switch (hexflg)
	{
		case PUTCODE:	bptr =binbuf;
				for (count = 1; count <= nbytes; count++)
				{
					puthex2(*bptr,&hxlnptr);
					chksum += *bptr++;
					if (++hxbytes == 16) flshhbf(pc+count);
				}

		case NOCODE:	return;

		case FLUSH:	flshhbf(pc);
				return;

		case NOMORE:	flshhbf(0);
				putlin(":0000000000\n\032",&hexbuf);
				flush(&hexbuf);
				return;
	}
}

/*
Function to put a line of intel hex to the appropriate
device and get a new line started.
*/

flshhbf(loadaddr)
unsigned loadaddr;
{
	if (hxbytes != 0)
	{
		puthex2(-(chksum+hxbytes),&hxlnptr);
		*hxlnptr++ = '\n';
		*hxlnptr++ = '\0';
		hxlnptr = hxlnbuf + 1;
		puthex2(hxbytes,&hxlnptr);
		putlin(hxlnbuf,&hexbuf);
	}
	hxbytes = 0;
	hxlnptr = hxlnbuf;
	*hxlnptr++ = ':';
	hxlnptr += 2;
	puthex4(loadaddr,&hxlnptr);
	puthex2(0,&hxlnptr);
	chksum = (loadaddr >> 8) + (loadaddr & 0xff);
}

/*
Function to put a 4-digit hex number into an output line.
*/

puthex4(number,lineptr)
unsigned number;
char **lineptr;
{
	puthex2(number>>8,lineptr);
	puthex2(number,lineptr);
}

/*
Function to put a 2-digit hex number into an output line.
*/

puthex2(number,lineptr)
char number, **lineptr;
{
	if ((**lineptr = (number >> 4) + '0') > '9') **lineptr += 7;
	if ((*++*lineptr = (number & 0xf) + '0') > '9') **lineptr += 7;
	++(*lineptr);
}

/*
Function to put a decimal number into an output line.
*/

putdec(number,lineptr)
unsigned number;
char **lineptr;
{
	if (number == 0) return;
	putdec(number/10,lineptr);
	*(*lineptr)++ = number % 10 + '0';
}

/*
Function to move a line to a disk buffer.  The line is pointed to
by line, and the disk buffer is specified by its disk I/O buffer
structure dskbuf.
*/

putlin(line,dskbuf)
char *line;
struct diskbuf *dskbuf;
{
	while (*line != '\0') putchr(*line++,dskbuf);
}

/*
Function to put a character into a disk buffer.  The character
is sent in char, and the disk buffer is specified by the address
of its structure.  Newline characters (LF's) are converted to
CR/LF pairs.
*/

putchr(byte,dskbuf)
char byte;
struct diskbuf *dskbuf;
{	
	char c;
	byte &= 0x7f;
	if(kbhit())
	{
		c = getchar();
		if(c == CTLC) wipeout("\n");
		if(c == CTLS) while(kbhit() == 0) getchar();
	}
	switch (dskbuf -> fd)
	{
		case CONO:	if (byte != CPMEOF) putchar(byte);

		case NOFILE:	return;

		case LST:	if (byte != CPMEOF)
				{
					if (byte == '\n') bdos(LISTOUT,'\r');
					bdos(LISTOUT,byte);
				}
				else putchr('\n',dskbuf);
				return;

		default:	if(dskbuf->fd >= 20){printf("fd=%u\n",dskbuf->fd);return;}
				if(byte == '\n') putchr('\r',dskbuf);
				if (dskbuf -> pointr >= dskbuf -> space + (NSECT * 128))
				{	if (write(dskbuf->fd,dskbuf->space,NSECT) == -1)
						{
						printf("\nDisk write error ++%s\n",errmsg(errno()));
						wipeout("\n");
						}
					dskbuf -> pointr = dskbuf -> space;
				}
				*(dskbuf -> pointr)++ = byte;
				return; 
				
	}
}

/*
Function to flush a disk buffer.
*/

flush(dskbuf)
struct diskbuf *dskbuf;
{
	int t;
	if (dskbuf -> fd < LODISK) return;
	t = dskbuf -> pointr - dskbuf -> space;
	t = (t % 128 == 0) ? t / 128 : t / 128 + 1;
	while (dskbuf -> pointr < &dskbuf -> space[NSECT * 128]) *(dskbuf -> pointr)++ = 0;
	if (write(dskbuf->fd,dskbuf->space,t) == -1) wipeout("\nDisk write error in flush.\n");
	if (close(dskbuf->fd) == -1) wipeout("\nError closing file.\n");
}

