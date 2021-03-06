/*
		PROLOG
		written by Bob Pasky
		36 Wiswall Rd.
		Newton Centre, MA 01259

		as an adaptation of the TELNET program
		written by Leor Zolman

	Compiled with BDS C 1.41; you may want to use the
	modified "_spr" function (see printf.c), but it is
	not required.

	The "modem" channel may be used to control a
	PRO-LOG(tm) EPROM programmer. A .HEX file
	(or portion thereof) may be burned into EPROM starting
	at any EPROM address. A second function verifies the
	contents of the EPROM against the .HEX file.

	During operation, typing a SPECIAL character followed
	by one of the given code letters performs special
	tasks:
		q:	re-boots CP/M.
		z:	clear screen
		p:	program an EPROM.
		v:	verify an EPROM.
 		d:	display directory of hex files
*/

#include "bdscio.h"

#define SPEC '@'	/* YOUR special character */

#define CLERS 0x1A	/* character to clear screen */
#define HOME 1
#define ACK 6
#define NAK 0x15
#define EOT 4
#define ENQ 5
#define STX 2

#define SRCHD1 17	/* search directory BDOS calls */
#define SRCHDN 18

int fdb;
int buflimit;
char filnam[20];
char hextab[16];
char fcb[33];
char bbuf[BUFSIZ];
char obuf[BUFSIZ];
char *buf;

/* For your particular configuration:
** define "MMREFIO" if you use memory
** reference i/o for modem control
** (peek-poke vs. inp-outp)
*/
#ifdef	MMREFIO
#define modin peek
#define modout poke
#endif

#ifndef MMREFIO
#define modin inp
#define modout outp
#endif

main(argc,argv)
int argc;
char **argv;
{
	char c, *bufp, *i;
	int fd, j;

	putchar(CLERS);
	putchar(HOME);
	strcpy(hextab,"0123456789ABCDEF");

/*	If your uart needs initializing, do it here:
**	iuart(2);	/* set for input & output */
*/
	printf ("\n\t\t\tEPROM Programmer ver. 1.A\n\n");

	instruct();

	modin(MDATA);	/* flush any garbage on line */

	bufp = buf = endext();
	buflimit = topofmem() - 1000;	/* 1k = stack + safety factor */

	printf("\n%dK buffer space\n",(buflimit-endext())/1000 );
	printf ("\n\nGet EPROM Programmer ready\n");

loop:	if (bios(2,0)) {	/* console status */
		c = bios(3,0);	/* console in */
		if (c != SPEC) outmod(c);
		else {
			printf("\nCommand: ");
			switch (toupper(getchar())) {

			case '?':
				instruct();
				break;
			 case 'Q': printf ("uit");
				if (fd) {
					putc(CPMEOF,obuf);
					fflush(obuf);
					close(fd);
				    }
				   exit();

			 case 'Z':  putchar(CLERS);
				    break;

			 case 'P':
				printf("rogram EPROM from HEX file");
				if (getfile() == FALSE) break;
				sendhex();
				close(fdb);
				printf("\nProgramming complete.\n");
				break;

			 case 'V':
				printf("erify EPROM equals HEX file");
				if (getfile() == FALSE) break;
				comphex(buf,bufp);
				close(fdb);
				printf("\nVerificaton complete.\n");
				break;
			 case 'D':
				printf("irectory of HEX files");
				printf("\nDrive: ");
				strcat(gets(filnam),":????????.HEX");
				showfiles();
				break;
			  }}	/* end switch, end else */
		}		/* end if kbready */
		if ((modin(MSTAT) & MIMASK) ==
			(MAHI ? MIMASK : 0)) {
			c= modin(MDATA) & 0x7f;

			switch (c) {	/* some common ctrl codes */
		case STX: printf("<STX>");
			break;
		case EOT: printf("<EOT>");
			break;
		case ACK: printf("<ACK>");
			break;
		case NAK: printf("<NAK>");
			break;
		default: putchar(c);
			}	/* end switch */
		 }		/* end if modin */
		goto loop;
}				/* end main */

instruct()
{
	printf ("\nFollowing the command character (%c) use:",SPEC);
	printf ("\n\td to show directory of HEX files");
	printf ("\n\tp to program EPROM");
	printf ("\n\tv to verify EPROM");
	printf ("\n\tq to re-boot CP/M.");
	printf ("\n\tz to clear screen");
	printf ("\n\t? to show this text\n");
}

getfile()
{
	printf("\nFilename: ");
	strcat(gets(filnam),".HEX");
	fdb=fopen(filnam,bbuf);
	if (fdb==ERROR) {
	    printf("\nOpen error on %s\n",filnam);
	    return FALSE;
	   }
	return TRUE;
}
showfiles()
{
	char c,i;

	setfcb(&fcb,filnam);
	if ((c = bdos(SRCHD1,&fcb)) == 0xFF) {
		printf("-- no files --\n");
		return; }
	else	showit(c);
	i = 1;
	while ((c = bdos(SRCHDN,&fcb)) != 0xFF) {
		putchar ((i++ % 4) ? ' ' : '\n');
		showit(c); }
	printf("\nDirectory complete\n");
}

/* The .HEX file format --
**	:	 - start of block character,
**	hh	- two hex digits = # of bytes in this block
**		  a 00 count indicates end of file,
**	hhhh	- four hex digits = load address of this block,
**	hh	- two hex digits = unused,
**	hh	- pair of hex digits = first data byte
**	...	- pairs of hex digits = remaining data bytes
**	hh	- last pair of hex digits = sumcheck for block,
**		  (ignored by this program)
**	<cr><lf>- carriage return, line feed.
*/
/* The Pro-Log expects --
**	hhh	- start address (for 8k to 32k EPROMS),
**	hhh	- end address,
**	P	- upper case 'P' = program.
** The Pro-Log returns --
**	<ACK> or <NAK>
**		- a NAK says the chip is not blank in the
**		  section to be programmed,
**	<STX>	- says start sending data.
** The Pro-Log expects --
**	hh	- two hex digits = data byte, <space> or <cr> is ignored.
** The Pro-Log returns --
**	<ACK> or <NAK> or <EOT>
**		- an ACK = sucessful program of location,
**		  NAK = error, reading location did not return data expected,
**		  EOT = end address has been reached.
** ...continue for each data byte until end address reached.
*/

sendhex()
{
char a1,a2,c,h1,h2,m;
int f,l,pe,ps,d1,i;
int count,errcnt,total;
char inputbuf[16];

	errcnt=total=0;
	if (!getaddress(&f,&l,&ps,&pe)) return;	/* user quits */

	/* start by sending PROLOG the start and end addresses */
	twohexout (ps,pe);
	outmod ('P');	/* a P puts PROLOG into program mode */

	/* The PROLOG will send a NAK if the PROM is not blank */
	while((m=getmod())!=ACK && m!=NAK && m!=ENQ);
	if (m==NAK) {
		printf("\nNOT ERASED, continue? ");
		if (tolower(getchar())!='y') goto endaddr;
		}
	if (m==ENQ) return;
	/*
	** Now read the .HEX file until the first address 
	** to be programmed is found
	*/
	while ((c=getc(bbuf)) != CPMEOF)
		{
		if (c != ':') continue;	/* look for block start char */
		a1 = getc(bbuf)-'0';	/* get # of bytes in block */
		a2 = getc(bbuf)-'0';
		if (a1==0 && a2==0) break; /* end of file */
		count = (a1<<4) + a2;	/* convert count to integer */

		for (i = 0; i<4; i++) {
			inputbuf[i]=getc(bbuf);
			}
		inputbuf[4]='\0';	/* end the string */
		sscanf(inputbuf,"%x",&d1);	/* convert to integer */
		printf ("\nBlock address: %x",d1);

		getc(bbuf); getc(bbuf); /* ignore next 2 bytes */

	/* looking for first location to be programmed: */
		if ( d1+count < f ) continue;
		for (i=0; i<count; i++,d1++)
			{
			if ( d1 > l ) {
				printf ("\nEnd address reached.");
				goto endaddr; }
			putchar (i%16 ? ' ' : '\n');

			h1=getc(bbuf);	/* get next 2 hex chars */
			h2=getc(bbuf);

			if (d1 < f) printf ("xx"); /* byte not being sent */
			else {			/* send byte to Prolog */
				printf ("%c%c",h1,h2);
				outmod(h1); outmod(h2);
				total++;

				while ((m=getmod()) != ACK &&
					m != NAK && m != EOT) ;
				if (m==NAK) { putchar ('#'); errcnt++; }
				if (m==EOT) {
					printf("\n<EOT>");
					goto endaddr; }
				} /* end else */
			} /* end for */
		} /* end while */
endaddr:
	printf("\n%4d error",errcnt);
	if (errcnt!=1) putchar ('s');
	printf (" out of %d location",errcnt,total);
	if (total!=1) putchar ('s');
	printf (" programmed.");

} /* end sendhex */

/* VERIFY EPROM
	Compares .HEX file with List data from PROLOG
*/
comphex(buf,bufp)
char *bufp, buf[];

{
char *i;
char a1,a2,c,h1,h2,j;
int f,l,pe,ps,d1,d2;
int count,errcnt,total;
char inputbuf[10];

	i = buf;
	errcnt=total=0;
	if (!getaddress(&f,&l,&ps,&pe)) return;

	twohexout(ps,pe);
	outmod ('L');
	while ((c=getmod()) != 'L' && c != ENQ) ; /* wait for echo of L */
	if (c == ENQ) return;
	bufp=buf;
	while ((c=getmod())!=EOT)
		{
		if ( c & 0x7f ) *bufp++ = c;
		if (bufp == buflimit)
			{
			printf("\nBuffer full.\n");
			break;
			}
		} /* end while */
	while ((c=getc(bbuf))!= CPMEOF)
		{
		if (c != ':') continue;
		a1 = getc(bbuf)-'0';
		a2 = getc(bbuf)-'0';
		count = (a1<<4) + a2;
		if (count == 0) break;

		for (j = 0; j < 4; j++) {
			inputbuf[j] = getc(bbuf);
			}
		inputbuf[4] = '\0';
		sscanf(inputbuf,"%x",&d1);
		if ( d1+count < f ) continue;

		getc(bbuf); getc(bbuf);	/*ignore next 2 bytes	*/

		for (a1 = 0; a1 < count; a1++,d1++)
		{
			if (d1 > 1) {
				printf("\nEnd address reached.");
				goto endcomp;
			}
			putchar( a1%16 ? ' ' : '\n');
			h1 = getc(bbuf);
			h2 = getc(bbuf);

			if (d1 < f) printf("xx");
			else {
				printf("%c%c",h1,h2);
				if (i >= bufp) break;
				while (*i < '0' || *i >'F') i++;

				c = *i++;	/* get first pair */
				if (c != h1 || *i != h2) {
					printf("#%c%c",c,*i);
					errcnt++;
				}
				i++;
			} /* end else */
		} /* end for */
	} /* end while */

endcomp:
	printf("\n%4d error",errcnt);
	if(errcnt != 1) putchar ('s');
} /* end comphex */

getaddress(first,last,pstart,pend)
int *first,*last,*pstart,*pend;
{
char c,inputbuf[16];

esa:	*pstart = *first = 0;
	*last = 0x7FF;	/* default to 16k EPROM	*/
	printf("\nEnter program starting address: (0000) ");
	gets(inputbuf);
	if(inputbuf[0]) sscanf(inputbuf,"%x",first);

	printf("\nEnter program ending address:   (07FF) ");
	gets(inputbuf);
	if(inputbuf[0]) sscanf(inputbuf,"%x",last);

	if(*last < *first) {
		printf("\nStart higher than end. Try again.");
		goto esa;
		}

	printf("\nEnter PROM starting address: (0000) ");
	gets(inputbuf);
	if(inputbuf[0]) sscanf(inputbuf,"%x",pstart);
	*pstart &= 0xFFF;	/* mask for 16k PROM	*/
	*pend = (*last - *first + *pstart) & 0xFFF;

	printf("\nProgram address: %04x", *first);
	printf("to %04x", *last);
	printf("\ninto PROM location:  %03x", *pstart);
	printf("to  %03x", *pend);	printf("\nAddresses ok? (Yes/No/Quit) ");
	if( (c = tolower(getchar()) ) == 'q') return(0);
	if(c!='y') goto esa;
	return(1);
} /* end getaddress */

twohexout(ps,pe)
int ps,pe;
{
	outmod(hextab[ (ps >> 8) & 0xF ]);
	outmod(hextab[ (ps >> 4) & 0xF ]);
	outmod(hextab[ ps & 0xF ]);
	outmod(hextab[ (pe >> 8) & 0xF ]);
	outmod(hextab[ (pe >> 4) & 0xF ]);
	outmod(hextab[ pe & 0xF ]);
}

getmod()
{
	int time;

	time = 0;
	while( (modin(MSTAT) & MIMASK) != (MAHI ? MIMASK : 0) )
		if (!++time) {
			printf("time out on rec'v\n");
			return (ENQ); 
		}
	return ( modin(MDATA) & 0x7F);
}

/* send char to serial port */
outmod(c)
char c;
{
	int time;

	time = 0;
	while( (modin(MSTAT) & MOMASK) == (MAHI ? 0 : MOMASK) )
		if (!++time) {
			printf("time out on xmit\n");
			return;
		}	modout(MDATA,c);

}

/* Prints a filename from a directory sector, assumed to be at BASE+80h
**	offset is the file number returned by a bdos call to find an
**	ambiguous (wildcarded) filename, but need only be a value
**	between 0 and 63.
*/
showit(offset)
int offset;
{
	/* find start of filename within dma buffer address.
	   there are 4 files per sector or 32 bytes per entry.
	*/
	offset = BASE + 0x80 + ((offset & 3) * 32);
	printf ("%14s",offset+1);
		/* this works only 'cause there is a zero byte
		   after the file type (i.e. the extent = 0);	*/
}
/* end of program - prolog */



 
