/*
$title ('yam1.c: Main program,command sequencer')
$date (17 nov 85)
*/

#include "yam.h"
#include <ctype.h>
#define MAXARGS 20

/****************************************************************************
FUNCTION:
	main program and command line parser.

CALLING PARAMETERS:
	argc:
		count of command line arguments.
	argv:
		pointer to string of command line arguments.
===========================================================================*/
main(argc, argv)
char *argv[];
int argc;
{
	int c;
	char *args[MAXARGS],
	     **argp,
		 *nextcmd;

		/* initialization */
	nextcmd=NULL;
	onceonly();

		/* init parmams and signon */
	init();

		/* read command line arguments */
	if(argc>MAXARGS)
		goto usage;

		/* copy given arguments to our own array */
	for (c = 0; c < argc; c++)
		args[c] = argv[c];

	for(;;)
	{
		if(argc < 2)
		{
			if(nextcmd)
				cp=nextcmd;
			else
			{
#ifdef XMODEM
				printf("\nxYAM(%c)", defdisk+'A');
#else
				printf("\n>>>%c:", defdisk+'a');
#endif
				gets(cmdbuf);
				cp=cmdbuf;
#ifdef SYSFUNC
				if(*cp=='!')
				{
					system(++cp);
					continue;
				}
#endif
			}
			if(nextcmd=index(';', cp))
				*nextcmd++ =0;	/*  ; separates commands */

				/* init arg pointer, arg count */
			argp = &args[1];
			argc = 1;
			while(1)
			{
					/* printable char means vaild argument */
				if(isgraph(*cp))
				{
					*argp++ = cp;
					argc++;

						/* skip to next arg */
					while(isgraph(*cp))
						cp++;
				}
					/* skip white space and change to string terminator */
				while(*cp==' ' || *cp=='\t')
					*cp++ =0;

					/* found another printable, get next arg */
				if(isgraph(*cp))
					continue;

					/* end of argument list */
				*cp=0;
				break;
			}
		}

			/* loop thru all parmaters */
		for(argp = &args[1]; --argc>0; )
		{
			uncaps(*argp);
			cp = *argp++;

				/* reset mode flags to default. They may have been set
				   by parms in s or r command */
			Gototerm=Batch=Crcflg=Creamfile=Echo=
				Ctlview=Chat=View=Quiet=FALSE;

			blklen=SECSIZ;
#ifdef DEFBYTL
			Bytesleft=DEFBYTL;
#endif

				/* change disk drive */
			if(index(':', cp))
			{
				chdsk(cp);
				continue;
			}

				/* evaluate arguments */
			switch(*cp++)
			{
				/* b [baudrate] */
			case 'b':

					/* bye = exit */
				if (cmdeq(cp, "ye"))
					doexit();

					/* break=send break to modem */
				if(*cp == 'r')
				{
					sendbrk();
					continue;
				}

					/* set baud rate */
				else
				{
					setbaud(atoi(*argp++));
					--argc;
				}
				continue;

			case 'c':
#ifndef RESTRICTED
				if(cmdeq(cp, "all") && --argc >=1)
				{
					uncaps(*argp);	/* make name lcase */
					if(getphone(*argp,Phone)!=ERROR)
						dial(Phone);
					else if (isdigit(**argp))
						dial_number(*argp);
					else
					{
						printf("Can't find data for %s\n", *argp++);
						goto usage;
					}
					argp++;
					term();
					continue;
				}
#endif
					/* ch=change config(parity,etc) */
				else if(cmdeq(cp, "h"))
				{
					changecnfg();
					continue;
				}
					/* chat mode */
				else if(cmdeq(cp, "hat"))
				{
					dochat();
					continue;
				}
					/* cd=change directory */
				else if(cmdeq(cp, "d"))
				{
					chgdir(argp);
					argc=0;
					continue;
				}
#ifdef BIGYAM
					/* crck */
				else if(cmdeq(cp, "rck"))
				{
					docrck( --argc, argp);
					argc=0;
					continue;
				}

					/* crc */
				else if(cmdeq(cp, "rc"))
				{
					docrc( --argc, argp);
					argc=0;
					continue;
				}
#endif
				else if(cmdeq(cp, "lose"))
				{
					dumprxbuff();
					closerx(TRUE);
					closetx(TRUE);
					continue;
				}
				goto usage;

			case 'd':
					/* delete file */
				if(cmdeq(cp, "el"))
				{
#ifdef RESTRICTED
					goto nono;
#else
					doerase( --argc, argp);
					argc=0;
					continue;
#endif
				}

					/* dir */
				if(cmdeq(cp, "ir"))
				{
					dodir(--argc, argp);
					argc=0;
					continue;
				}
#ifdef IBMPC
					/* debug=cause int 3 */
				else if(cmdeq(cp, "ebug"))
				{
					dbgint();
					continue;
				}
#endif
					/* dump */
				if(cmdeq(cp, "ump"))
				{
					dodump(--argc, argp);
					argc=0;
					continue;
				}
					/* d.. = set paramter requested false */
				else if(setval(0)==ERROR)
					goto usage;
				continue;

			case '-':
			case 'e':

					/* e.. = set paramter requested true */
				if(setval(~0)==ERROR)
					goto usage;
				continue;

			case 'f':
					/* close any already open files */
				closetx(TRUE);
					/* set parmaters if any */
				if(setval(~0)==ERROR)
					goto usage;
					/* open file.  open will compute file size */
				if(--argc<=0 || opentx(1,0,*argp++)==ERROR)
					goto usage;
				/* frame file with ^R ^T if Squelch */
				if(Squelch)
					sendbyte(022);
				term();
				if(Squelch)
					sendbyte(024);
				continue;

			case 'i':
				if(cmdeq(cp, "nit"))
				{
					init();
					argc=0;
					nextcmd=0;	
					continue;
				}
				else
					goto usage;

			case '?':
			case 'h':
				searchpath(HELPFILE,Utility.ubuf);
				listfile(NULL,Utility.ubuf);
				continue;

			case 'k':
				if(cmdeq(cp, "ill"))
				{
					clearbuff();
					continue;
				}
				else
					goto usage;

#ifndef RESTRICTED
					/* change modem port */
			case 'm':
				closetx(TRUE);
				if (chngport(atoi(cp))==ERROR)
					goto usage;
				continue;
#endif

			case 'o':
					/* turn on DTR to modem */
				if(cmdeq(cp, "n"))
				{
					offhook();
					continue;
				}
					/* hang up phone */
				onhook();
				continue;

			case 'p':
					/* pwd=print working directory */
				if(cmdeq(cp, "wd"))
					dopwd();

					/* set parmamters */
				else if(setparm(cp)==ERROR)
					goto usage;
				continue;

			case 'q':
				/* quit.  go to DOS, don't hangup */
				if(cmdeq(cp, "u"))
				{
					dumprxbuff();
					closerx(TRUE);
					closetx(TRUE);
					userexit();
					exit(0);
				}
				goto usage;

			case 'r':
				if(cmdeq(cp, "eset"))
				{
					terminit();
						/* close and flush capture buffer */
					closetx(TRUE);
					dumprxbuff();
					closerx(TRUE);
						/* reset modem port */
					userinit(TRUE);

					/* change buffer size for debugging */
					if(--argc>0)
					{
						bufend= bufst + atoi(*argp++);
						clearbuff();
						dostat();
					}
					continue;
				}
					/* replot */
				if(cmdeq(cp, "ep"))
				{
					replot(argc>1 ? (--argc,atoi(*argp++)) : 0);
					continue;
				}
					/* rewind */
				if(cmdeq(cp, "ew"))
				{
					rewindcb();
					continue;
				}
					/* set flags for receive */
				if(chkbvq()==ERROR)
					goto usage;
				if(argc<2)
					Batch=TRUE;
				psxfer(wcreceive(--argc, argp));
				if(Gototerm)
					term();
				argc=0;
				continue;

			case 's':
#ifdef BIGYAM
					/* sum */
				if (cmdeq(cp, "um"))
				{
					dosum( --argc, argp);
					argc=0;
					continue;
				}
#endif

					/* s=status */
				if(*cp==0 && argc==1)
				{
					dostat();
					continue;
				}
					/* illegal */
				if(argc<2 || chkbvq()==ERROR)
					goto usage;

					/* s *.?= wild card send */
				if( argc > 2
				  || index('?',*argp) || index('*',*argp))
					Batch=TRUE;
#ifdef XMODEM
				/* tell 'em how long it will take! */
				if(Batch)
					docomp(argc-1, argp);
#endif
				/* s filename */
				psxfer(wcsend(--argc, argp));
				if(Gototerm)
					term();
				argc=0;
				continue;

			case 't':
					/* type */
				if(cmdeq(cp, "ype"))
				{
					dolist( --argc, argp);
					argc=0;
					continue;
				}

					/* tab = set tab expansion */
				if(cmdeq(cp, "ab"))
				{
					tabstop = (char)(atoi(*argp++));
					argc=0;
						/* tab of 0 is illegal.  Also resets
						   tab back to default if tab only entered */
					if (!tabstop)
						tabstop=TABS;
					continue;
				}

					/* t [filename] */
				if(--argc > 0)
				{
					if(opencapt(*argp++)==ERROR)
						goto usage;
				}
					/* t.. */
				switch(setval(~0))
				{
					case ERROR:
						goto usage;
					case OK:
						term();
					case TRUE:
						break;
						/* remain in command mode if t..c  */
				}
				continue;

			case 'w':
#ifdef BIGYAM
					/* word count */
				if (cmdeq(cp, "c"))
				{
					dowc( --argc, argp);
					argc=0;
					continue;
				}
#endif
				dumprxbuff();
				continue;

			case 'x':
					/* exit only if x is only command on line */
				if( *cp==0 && argc==1 )
					doexit();

			case 0:
			default:
				goto usage;
			}
		}
		continue;
#ifdef RESTRICTED
nono:
		printf("Command not allowed\n");
#endif
usage:
		printf("Type HELP");
		nextcmd=0;
		argc=0;
		continue;
	}
} /* main */


/****************************************************************************
FUNCTION:
	set flags used by R and S commands.

CALLING PARAMETERS:
	none.
===========================================================================*/
chkbvq()
{
	while(*cp)
		switch(*cp++)
		{
			case 'b':
				Batch=TRUE;
				break;
			case 'c':
				Crcflg=TRUE;
				break;
			case 'k':
				blklen=KSIZE;
				break;
			case 'q':
				Quiet=TRUE;
				break;
#ifndef RESTRICTED
			case 't':
				Gototerm=TRUE;
				break;
			case 'v':
				View=TRUE;
				break;
			case 'y':
				Creamfile=TRUE;
				break;
#endif
			default:
				return ERROR;
		}
	return FALSE;
} /* chkbvq */


/****************************************************************************
FUNCTION:
	set paramter from an e,d,f or t command.

CALLING PARAMETERS:
	value:
		true or false value to set flag to
===========================================================================*/
setval(value)
unsigned value;
{
	while(*cp)
		switch(*cp++)
		{
			case '!':
				value = 0;
				break;
#ifdef Cis02
			case '2':
				Cis02 = value;
				break;
#endif
			case 'a':
				Chat = value;
				break;
			case 'b':
				Txeoln = value&TX_BINARY;
				Txmoname = "BINARY";
				break;
			case 'd':
				Dumping = !Squelch || value;
				break;
			case 'e':
				Echo = value;
				break;
			case 'f':
				Hdx = !value;
				break;
			case 'g':
				Txgo = value;
				break;
			case 'h':
				Hdx = value;
				break;
			case 'i':
				Ignrx = value;
				break;
			case 'k':
				blklen = KSIZE;
				break;
			case 'l':
				Pflag = value;
				break;
			case 'n':
				Txeoln = value&EOL_NL;
				Txmoname = "NL ONLY";
				break;
			case 'p':
				Txeoln = value&EOL_CRPROMPT;
				Txmoname = "WAIT FOR PROMPT";
				break;
			case 'r':
				Txeoln  = value&EOL_CR;
				Txmoname = "CR ONLY";
				break;
			case 's':
				Dumping = !(Squelch=value);
				break;
			case 't':
				Waitbunch = value;
				break;
			case 'v':
				Ctlview = value;
				break;
			case 'w':
				Txeoln = value&EOL_CRWAIT;
				Txmoname = "CR WAIT FOR NL";
				break;
			case 'x':
				Exoneof = value;
				break;
			case 'z':
				Zeof = value;
				break;
			default:
				return ERROR;
		}
	return OK;
} /* setval */


/****************************************************************************
FUNCTION:
	Clear the circular buffer without saving it to disk 

CALLING PARAMETERS:
	none.
===========================================================================*/
clearbuff()
{
	Xoffflg=Wrapped=FALSE;
	buffcdq=bufcq=bufcdq=bufpcdq=bufmark=bufst;
	Bufsize=Nfree=bufend-bufst-1;
} /* clearbuff */


/****************************************************************************
FUNCTION:
	Display status of flags and parameters

CALLING PARAMETERS:
	none.
===========================================================================*/
dostat()
{
		/* name of file being received */
	printf("Receiving %s ",Rfile?Rname:"<nil>");

		/* mode of file receive */
	if(Squelch)
		printf("^R ^T Squelch ");
	if(Zeof)
		printf("EOF on ^Z");
	printf("\n");

		/* set default mode name */
	if(Txeoln==EOL_NOTHING)
		Txmoname="normal";

		/* transmit mode */
	printf("%sSending in %s mode\n", Txgo? "" : "Pausing in ", Txmoname);
	printf("%sWaiting %d loops every %u chars  Pause=%u",
			Waitbunch?"":"NOT ",Throttle, Waitnum, Tpause);

	printf(" GOchar=");
	if (GOchar < ' ')
		printf("'^%c'\n",GOchar|0x40);
	else
		printf("'%c'\n");

		/* printer mode and port mode */
	printf("Printer:%s Duplex:%s Tabstop:%d\n",
			 Pflag?"ON":"OFF", Hdx?"HALF":"FULL",tabstop);
	printf("baud:%u data port:%d\n", Baudrate, commport);

		/* buffer stats */
	printf("%u of %u chars used %u free%s\n",
	  Bufsize-Nfree, Bufsize+1, Nfree+1, Wrapped?" POINTERS WRAPPED":"");

		/* buffer stats for debug */
/*	printf("bufst=%x bufcq=%x bufcdq=%x buffcdq=%x bufpcdq=%x bufend=%x\n",
	 bufst, bufcq, bufcdq, buffcdq, bufpcdq, bufend);
*/
		/* total errors in tx, rx */
	if (toterrs)
	{
		printf("%u Total errors detected\n", toterrs);
		toterrs = 0;
	}
		/* print space left on disk */
	printdfr();
} /* dostat */


/****************************************************************************
FUNCTION:
	index returns a pointer to the first occurrence of c in s,
	NULL otherwise.

CALLING PARAMETERS:
	c:
		character to look for
	*s:
		pointer to string to search.
===========================================================================*/
char *index(c, s)
char c,*s;
{
	for(; *s; s++)
		if(c== *s)
			return s;
	return NULL;
}


/****************************************************************************
FUNCTION:
	display status of transfer

CALLING PARAMETERS:
	status:
===========================================================================*/
psxfer(status)
int status;
{
#ifdef XREM
	printf("File transfer(s) ");
#else
	printf("\007File transfer(s) ");
#endif
	if (status==ERROR)
		printf("ABORTED BY ERROR\n\007");
	else
		printf("successful\n");

		/* print space left on disk */
	printdfr();
} /* psxfer */


/****************************************************************************
FUNCTION:
	set paramter from a 'p' command.

CALLING PARAMETERS:
	*p:
		pointer to string of parmaters to set
===========================================================================*/
setparm(p)
char *p;
{
	if ( !isdigit(p[1]))
		return ERROR;
	firstch = atoi(p + 1);		
	switch(*p)
	{
	case 'g':
		GOchar = firstch;
		break;
	case 'p':
		Tpause = firstch;
		break;
	case 't':
		Throttle = firstch;
		break;
	case 'w':
		Waitnum = firstch;
		break;
	default:
		return ERROR;
	}
	return 0;
} /* setparm */


/****************************************************************************
FUNCTION:
	make string s lower case

CALLING PARAMETERS:
	*s:
		pointer to string to convert to upper case.
===========================================================================*/
uncaps(s)
char *s;
{
	for( ; *s; ++s)
		*s = tolower(*s);
}


/****************************************************************************
FUNCTION:
	perform exit cleanup to return to dos

CALLING PARAMETERS:
	none
===========================================================================*/
doexit()
{
		/* hang up phone */
	onhook();
		/* dump capture buffer */
	dumprxbuff();
		/* close xmit and rx files */
	closerx(TRUE);
	closetx(TRUE);
	purgeline();
	userexit();
	exit(0);
}
