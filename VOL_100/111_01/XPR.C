/*
HEADER:		;
TITLE:		Transfer via punch and reader;
VERSION:	1.0;
DATE:		12/11/1981;

DESCRIPTION:	"Transfers files between CP/M systems, through
		BDOS, using punch and reader.";

KEYWORDS:	Communication, transfer, utility;
SYSTEM:		CP/M-80;
FILENAME:	XPR.C;
AUTHORS:	Ward Christensen;
COMPILERS:	BDS C;
*/
/*******************************************************************

Version 1.0, 12/11/1982:
		Ward Christensen

DEPENDENCIES:	S T A R T   T H E   S E N D I N G   P R O G R A M   F I R S T ,
		as there is no way to "time out", and recover, since
		RDR/PUN have no "status test" ability.

EXECUTION:	On the sending/punching machine, type:

		xpr s filename...
	or	xpr p filename...	Sends (Punches) a file to PUN

		On the receiving/reading machine, type:

		xpr r filename...	Receive (Reads) via RDR

N O T E :	"..." means multiple filenames may be specified:
		xpr s b:othello.c a:stat.com b:foo.c

DOCUMENTATION:	Protocol consists of:

	1.	Receiver sends SYNC byte to cause sender to start.

	2.	Each sector is sent as:

		SOH,SECT,<-- 128 bytes data -->,CKSUM

		Cksum is simply the 16-bit sum of the data characters,
		sent as:
			(Cksum & 0xff)+(Cksum/256)
		in order to go over an 8-bit channel.

	3.	RESPONSE is either ACK or NAK.

	4.	EOT sent at end of file.

	5.	SOH, ACK, NAK, and EOT are always sent as-is.
		When in 7-bit mode, all other data (sector #, data) is
		sent as two hex-ASCII characters.


		function directory with their line #'s

physout() 96	physin() 103	main() 112	option() 131	errexit() 156
dofile() 169	sendfile() 184	sendsec() 215	recvfile() 246	recvsec() 276
rdrchr() 314	logicalin() 323	hex() 332	punchr() 340	nibble() 353
abort() 363	testname() 375

****************************************************************************/

#include <bdscio.h>
#define	SECSIZ	128
#define	SYNC	0x27
#define	SOH	1
#define	ACK	6
#define	NAK	0x18
#define	EOT	4
#define	CANCEL	'X'
#define	CPMRDR	3
#define	CPMPUN	4

/*		GLOBALS		(start with capital letters)
*/
	char	Fbuf[BUFSIZ],	/* input/output buff		*/
		Tbuf[SECSIZ],	/* xmit/recv buffer		*/
		*Bufptr,	/* Pointer into Fbuf		*/
		Send,		/* true if sending file		*/
		Recv,		/* true if receiving file	*/
		Sevenbit,	/* true if 7-bit transfer	*/
		C;		/* general single char		*/
	int	Filenum,	/* argv index for next file 	*/
		Secno,		/* sector to send		*/
		Nerrs,		/* number of errs		*/
		Toterrs,	/* Total number of errors	*/
		Nread,		/* # sectors read		*/
		Fd,		/* File Descriptor		*/
		I;		/* general index		*/
	Unsigned Cksum;		/* block transfer cksum 	*/


/*	================================================
	>>> These are the physical character drivers <<<
	    The rest of the program is all "logical I/O "
*/

/*	--- physical char output ---
*/
physout(chr)
char	chr;
{	bdos(CPMPUN,chr);
}	

/*	--- physical char input ---
*/
physin()
{	return(bdos(CPMRDR,0));
}

/*		>>> End of physical I/O <<<
		===========================

	--- M A I N ---
*/
main(argc,argv)
int argc; char **argv;
{
	Sevenbit=
	Toterrs=
	Send=
	Recv=0;
	Filenum=2;				/* next argv name	*/
	option(argc,argv);			/* validate option	*/
	while ((argc) > Filenum)		/* while more files	*/
		dofile(argv[Filenum++]);	/* do one file		*/
	puts("Processing completed, ");
	printf("%d file(s), %d errors\n",Filenum-2,Toterrs);
	exit(0);
}

/*
	--- Process the option byte ---
*/
option(argc,argv)
int	argc;
char	**argv;
{	char c;
	if (argc < 3)				/* option + 1 or more files? */
		errexit();
	if (argv[1][1]=='7')
		Sevenbit = 1;			/* doing 7-bit transfer	*/
	c=argv[1][0];				/* get option		*/
	switch(c)				/* test it		*/
	{	case 'S':			/* Send			*/
		case 'P':			/* or Punch?		*/
			Send = 1;		/* 	show sending	*/
			return(0);
		case 'R':			/* Receive/read?	*/
			Recv = 1;		/*	show receiving	*/
			return(0);
		default:
			errexit();		/* otherwise, error exit*/
	}
}	/* end of "option" */

/*
	--- exit with usage explanation message ---
*/
errexit()
{	puts("XPR by Ward Christensen\n");
	puts("CP/M Xfer via Punch and Rdr\n\n");	
	puts("	Invalid option.  Format is:\n\
	xpr s filename1 [filename2...]	on the sending machine\n\
	xpr r filename1 [filename2...]	on the receiving machine\n\n\
	s7 or r7 may be used for 7-bit tranfers (both ends must agree)\n");
	exit(999);
}

/*
	--- Do a file ---
*/
dofile(file)
char	*file;
{	if (testname(file))
	{	puts("No ambiguous files allowed\n");
		errexit();
	}
	if (Send)
		Sendfile(file);
	if (Recv)
		Recvfile(file);
}

/*
	--- send a file ---
*/
sendfile(file)
char	*file;
{	if ((Fd=open(file,0))==ERROR)		/* open for input	*/
	{	printf("cannot open: %s\n",file);
	exit();
	}
	printf("Sending file %s ",file);

	C=physin() & 0x7f;			/* get sync byte	*/
	if (C != SYNC)
	{	printf("%x received, not SYNC; aborting\n",C);
		exit(0);
	}
	puts("Sync received\n");
	Secno=1;
	while (0< (Nread = read(Fd,Fbuf,BUFSIZ/SECSIZ)))
	{
		Bufptr=Fbuf;			/* init pointer		*/
		while(Nread--)
			sendsec();
	}
	physout(EOT);
	if (Nread == ERROR)
		abort("Read error\n");
	close(file);
	puts("\n");
}

/*
	--- Send a sector ---
*/
sendsec()
{	char *ptr;
	Nerrs=0;
	while(1)
	{	ptr=Bufptr;
		Cksum=0;
		putchar('.');
		physout(SOH);			/* send header	*/
		punchr(Secno);			/* 	secno	*/
		for (I=0;I<128;++I)
		{	punchr(*ptr);		/*	data	*/
			Cksum+=*ptr++;
		}

		punchr((Cksum & 0xff)+(Cksum/256)); /*	cksum 	*/

		if ((C=rdrchr())==ACK)
		{	Secno=(++Secno & 0x0f);
			Bufptr+=128;
			return(0);
		}
		++Toterrs;
		printf("0x%x rcd, not ACK\n",C);
		if (++Nerrs == 10)
			abort("Too many errors\n");
	}
}

/*
	--- receive a file ---
*/
recvfile(file)
char	*file;
{	int	count;
	if ((Fd=creat(file))==ERROR)		/* delete, open for out	*/
	{	printf("cannot make: %s\n",file);
	exit();
	}
	printf("Receiving file %s\n",file);
	physout(SYNC);				/* Sync transmitter	*/
	Secno=1;
	Bufptr=Fbuf;
	count=0;
	while (recvsec())			/* get a block		*/
	{	if (++count==(BUFSIZ/SECSIZ))	/* buffer full		*/
		{	if (write(Fd,Fbuf,count)!=count)
				abort("Write error\n");
			Bufptr=Fbuf;
			count=0;
		}
		punchr(ACK);
	}
	if (count > 0)
		write(Fd,Fbuf,count);
	close(Fd);
	puts("\n");
}

/*
	--- receive a sector ---
*/
recvsec()
{	char *ptr;
	Nerrs=0;
	while(1)
	{	ptr=Bufptr;
		Cksum=0;
		C=logicalin();
		if (C==CANCEL)
			abort("Program cancelled from other end\n");
		if (C==EOT)
			return(0);
		if (C!=SOH)
			abort("SOH not received\n");
		if (rdrchr()!=Secno)
			abort("Invalid sector # received\n");
		for (I=0;I<128;++I)
		{	*ptr=rdrchr();
			Cksum+=*ptr++;
		}
		C=rdrchr();
		Cksum=(((Cksum & 0xff)+(Cksum/256))&0xff);
		if (Cksum==C)				/* valid cksum?	*/
		{	Secno=(++Secno & 0x0f);
			Bufptr+=128;
			putchar('.');
			return(1);
		}
		++Toterrs;
		punchr(NAK);
		puts("Cksum\n");
		if (++Nerrs == 10)
			abort("Too many errors\n");
	}
}

/*
	--- Get a char from the reader ---
*/
rdrchr()
{	if (Sevenbit)
		return (16*hex(logicalin())+hex(logicalin()));
	else	return(logicalin());
}

/*
	--- logical char input ---
*/
logicalin()
{	if (Sevenbit)
		return(physin() & 0x7f);
	else	return(physin());
}

/*
	--- printable hex to binary conversion, 1 nibble ---
*/
hex(nib)
char	nib;
{	return( (nib>'9') ? nib-'7' : nib-'0' );
}

/*
	--- put a char to the punch ---
*/
punchr(chr)
char	chr;
{	if (Sevenbit)
	{	physout(nibble(chr / 16));
		physout(nibble(chr));
	}
	else	physout(chr);
}


/*
	--- convert a char to a nibble in printable hex --
*/
nibble(chr)
char	chr;
{	chr=chr & 0x0f;
	return( (chr<10) ? '0'+chr : '7'+chr );
}


/*
	--- Abort transmitting ---
*/
abort(msg)
char	*msg;
{	puts(msg);
	puts(">> aborting <<\n");
	printf("C=%x\n",C);
	punchr(CANCEL);
	exit();
}

/*
	--- test that a filename doesn't contain "?"s ---
*/
testname(str)
char	*str;
{	char *local,c;
	local=str;
	while(*local)			/* exit on \0	*/
	if((c=*local++)== '?'		/* is it "?"	*/
		  || c == '*')		/* or is it "*"	*/
		return(1);			/* ret if so	*/
	return(0);				/* ret, OK	*/

}	/* end of testname */
