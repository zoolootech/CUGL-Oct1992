/*
 *	K e r m i t  File Transfer Utility
 *
 *	UNIX Kermit, Columbia University, 1981, 1982, 1983
 *	Bill Catchings, Bob Cattani, Chris Maio, Frank da Cruz
 *
 *	usage: kermit [csr][dlbe line baud escapechar] [f1 f2 ...]
 *
 *	where c=connect, s=send [files], r=receive, d=debug,
 *	l=tty line, b=baud rate, e=escape char (decimal ascii code).
 *	For "host" mode Kermit, format is either "kermit r" to
 *	receive files, or "kermit s f1 f2 ..." to send f1 .. fn.
 *
 *
 *	Fixed up again for Unix, Jim Guyton 7/13/83 (Rand Corp)
 */

#include <stdio.h>			/* Standard UNIX definitions */
#include <sgtty.h>
#include <signal.h>
#include <setjmp.h>

/* Conditional Compilation: 0 means don't compile it, nonzero means do */

#define UNIX		1	/* Conditional compilation for UNIX */
#define TOPS_20 	0	/* Conditional compilation for TOPS-20 */
#define VAX_VMS 	0	/* Ditto for VAX/VMS */
#define IBM_UTS 	0	/* Ditto for Amdahl UTS on IBM systems */


/* Symbol Definitions */

#define MAXPACK 	94	/* Maximum packet size */
#define SOH		1	/* Start of header */
#define SP		32	/* ASCII space */
#define CR		015	/* ASCII Carriage Return */
#define DEL		127	/* Delete (rubout) */
#define CTRLD		4
#define BRKCHR		CTRLD	/* Default escape character for CONNECT */

#define MAXTRY		5	/* Times to retry a packet */
#define MYQUOTE 	'#'     /* Quote character I will use */
#define MYPAD		0	/* Number of padding characters I will need */
#define MYPCHAR 	0	/* Padding character I need */
#define MYEOL		'\n'    /* End-Of-Line character I need */
#define MYTIME		5	/* Seconds after which I should be timed out */
#define MAXTIM		20	/* Maximum timeout interval */
#define MINTIM		2	/* Minumum timeout interval */

#define TRUE		-1	/* Boolean constants */
#define FALSE		0


/* Global Variables */

int	size,			/* Size of present data */
	n,			/* Message number */
	rpsiz,			/* Maximum receive packet size */
	spsiz,			/* Maximum send packet size */
	pad,			/* How much padding to send */
	timint, 		/* Timeout for foreign host on sends */
	numtry, 		/* Times this packet retried */
	oldtry, 		/* Times previous packet retried */
	fd,			/* File pointer of file to read/write */
	remfd,			/* File pointer of the host's tty */
	image,			/* -1 means 8-bit mode */
	remspd, 		/* Speed of this tty */
	host,			/* -1 means we're a host-mode kermit */
	debug;			/* -1 means debugging */

char	state,			/* Present state of the automaton */
	padchar,		/* Padding character to send */
	eol,			/* End-Of-Line character to send */
	escchr, 		/* Connect command escape character */
	quote,			/* Quote character in incoming data */
	**filelist,		/* List of files to be sent */
	*filnam,		/* Current file name */
	recpkt[MAXPACK],	/* Receive packet buffer */
	packet[MAXPACK];	/* Packet buffer */

struct sgttyb
	rawmode,		/* Host tty "raw" mode */
	cookedmode,		/* Host tty "normal" mode */
	remttymode;		/* Assigned tty line "raw" mode */

jmp_buf env;			/* Environment ptr for timeout longjump */


/*
 *	m a i n
 *
 *	Main routine - parse command and options, set up the
 *	tty lines, and dispatch to the appropriate routine.
 */

main(argc,argv)
int argc;				/* Character pointer for */
char **argv;				/*  command line arguments */
{
  char *remtty,*cp;			/* tty for CONNECT, char pointer */
  int speed, cflg, rflg, sflg;		/* speed of assigned tty, */
					/* flags for CONNECT, RECEIVE, SEND */
  if (argc < 2) usage();		/* Make sure there's a command line. */

  cp = *++argv; argv++; argc -= 2;	/* Set up pointers to args */

/* Initialize this side's SEND-INIT parameters */

  eol = CR;				/* EOL for outgoing packets */
  quote = MYQUOTE;			/* Standard control-quote char "#" */
  pad = 0;				/* No padding */
  padchar = NULL;			/* Use null if any padding wanted */

  speed = cflg = sflg = rflg = 0;	/* Turn off all parse flags */
  remtty = 0;				/* Default is host (remote) mode */
  image = FALSE;			/* Default to 7-bit mode */
  escchr = BRKCHR;			/* Default escape character */

  while ((*cp) != NULL) 		/* Get a character from the cmd line */
    switch (*cp++)			/* Based on what the character is, */
     {					/*  do one of the folloing */
      case '-': break;                  /* Ignore dash (UNIX style) */
      case 'c': cflg++; break;          /* C = CONNECT command */
      case 's': sflg++; break;          /* S = SEND command */
      case 'r': rflg++; break;          /* R = RECEIVE command */
      case 'e': if (argc--)             /* E = specify escape char */
		  escchr = atoi(*argv++); /*  as ascii decimal number */
		else usage();
		if (debug) fprintf(stderr,"escape char is ascii %d\n",escchr);
		break;
      case 'l': if (argc--)             /* L = specify tty line to use */
		  remtty = *argv++;
		else usage();
		if (debug) fprintf(stderr,"line %s\n",remtty);
		break;
#if UNIX				/* This part only for UNIX systems */
      case 'b': if (argc--) speed = atoi(*argv++); /* Set baud rate */
		  else usage();
		if (debug) fprintf(stderr,"speed %d\n",speed); break;

      case 'i': image = TRUE; break;    /* Image (8-bit) mode */
#endif /* UNIX */

      case 'd': debug = TRUE; break;    /* Debug mode */
     }

/* Done parsing */

  if ((cflg+sflg+rflg) != 1) usage();	/* Only one command allowed */

  remfd = 0;				/* Start out as a host (remote) */
  host = TRUE;

  if (remtty)				/* If another tty was specified, */
   {
    remfd = open(remtty,2);		/*  open it */
    if (remfd < 0)			/*  check for failure */
     {
      fprintf(stderr,"Kermit: cannot open %s\n",remtty);
      exit(-1); 			/*  Failed, quit. */
     }
    host = FALSE;			/* Opened OK, flag local (not host) */
   }

/* Put the tty(s) into the correct modes */

  gtty(0,&cookedmode);			/* Save current mode for later */
  gtty(0,&rawmode);
  rawmode.sg_flags |= (RAW|TANDEM);
  rawmode.sg_flags &= ~(ECHO|CRMOD);

  gtty(remfd,&remttymode);		/* If local kermit, get mode of */
					/*  assigned tty */
  remttymode.sg_flags |= (RAW|TANDEM);
  remttymode.sg_flags &= ~(ECHO|CRMOD);

#if UNIX				/* Speed changing for UNIX only */
  if (speed)				/* User specified a speed? */
   {
    switch(speed)			/* Get internal system code */
     {
	case 110: speed = B110; break;
	case 150: speed = B150; break;
	case 300: speed = B300; break;
	case 1200: speed = B1200; break;
	case 2400: speed = B2400; break;
	case 4800: speed = B4800; break;
	case 9600: speed = B9600; break;
	default: fprintf(stderr,"bad line speed\n");
     }
    remttymode.sg_ispeed = speed;
    remttymode.sg_ospeed = speed;
   }
#endif /* UNIX */

  if (remfd) stty(remfd,&remttymode);	/* Put asg'd tty in raw mode */


/* All set up, now execute the command that was given. */

  if (cflg) connect();			/* CONNECT command */

  if (sflg)				/* SEND command */
   {
    if (argc--) filnam = *argv++;	/* Get file to send */
      else usage();
    filelist = argv;
    if (host) stty(0,&rawmode); 	/* Put tty in raw mode if remote */
    if (sendsw() == FALSE)		/* Send the file(s) */
      printf("Send failed.\n");         /* Report failure */
    else				/*  or */
      printf("OK\n");                   /* success */
    if (host) stty(0,&cookedmode);	/* Restore tty */
   }

  if (rflg)				/* RECEIVE command */
   {
    if (host) stty(0,&rawmode); 	/* Put tty in raw mode if remote */
    if (recsw() == FALSE)		/* Receive the file */
      printf("Receive failed.\n");      /* Report failure */
    else				/*  or */
      printf("OK\n");                   /* success */
    if (host) stty(0,&cookedmode);	/* Restore tty */
   }
 }

usage() 				/* Give message if user makes */
{					/* a mistake in the command */
  fprintf(stderr,
     "usage: kermit [csr][di][lbe] [line] [baud] [esc char] [f1 f2 ...]\n");
  exit();
}

/*
 *	s e n d s w
 *
 *	Sendsw is the state table switcher for sending
 *	files.	It loops until either it finishes, or
 *	an error is encountered.  The routines called by
 *	sendsw are responsible for changing the state.
 *
 */

sendsw()
{
 char sinit(),sfile(),seof(),sdata(),sbreak();

 state = 'S';                           /* Send initiate is the start state */
 n = 0; 				/* Initialize message number */
 numtry = 0;				/* Say no tries yet */
 while(TRUE)				/* Do this as long as necessary */
  {
   switch(state)
    {
     case 'D':  state = sdata();  break; /* Data-Send state */
     case 'F':  state = sfile();  break; /* File-Send */
     case 'Z':  state = seof();   break; /* End-of-File */
     case 'S':  state = sinit();  break; /* Send-Init */
     case 'B':  state = sbreak(); break; /* Break-Send */
     case 'C':  return (TRUE);           /* Complete */
     case 'A':  return (FALSE);          /* "Abort" */
     default:	return (FALSE); 	 /* Unknown, fail */
    }
  }
}


/*
 *	s i n i t
 *
 *	Send Initiate: Send my parameters, get other side's back.
 */

char sinit()
{
  int num, len; 			/* Packet number, length */

  if (debug) fprintf(stderr,"sinit\n");
  if (numtry++ > MAXTRY) return('A');   /* If too many tries, give up */
  spar(packet); 			/* Fill up with init info */
  if (debug) fprintf(stderr,"n = %d\n",n);

#if UNIX
  flushinput(); 			/* Flush pending input */
#endif /* UNIX */

  spack('S',n,6,packet);                /* Send an S packet */
  switch(rpack(&len,&num,recpkt))	/* What was the reply? */
   {
    case 'N':  return(state);           /* NAK */

    case 'Y':                           /* ACK */
      if (n != num) return(state);	/* If wrong ACK, stay in S state */
      rpar(recpkt);			/* Get other side's init info */
      if (eol == 0) eol = '\n';         /* Check and set defaults */
      if (quote == 0) quote = '#';      /* Control-prefix quote */
      numtry = 0;			/* Reset try counter */
      n = (n+1)%64;			/* Bump packet count */
      if (debug) fprintf(stderr,"Opening %s\n",filnam);
      fd = open(filnam,0);		/* Open the file to be sent */
      if (fd < 0) return('A');          /* if bad file descriptor, give up */
      if (!host) printf("Sending %s\n",filnam);
      return('F');                      /* OK, switch state to F */

    case FALSE: return(state);		/* Receive failure, stay in S state */
    default: return('A');               /* Anythig else, just "abort" */
   }
 }


/*
 *	s f i l e
 *
 *	Send File Header.
 */

char sfile()
{
  int num, len; 			/* Packet number, length */

  if (debug) fprintf(stderr,"sfile\n");

  if (numtry++ > MAXTRY) return('A');   /* If too many tries, give up */
  for (len=0; filnam[len] != '\0'; len++); /* Add up the length */

  spack('F',n,len,filnam);              /* Send an F packet */
  switch(rpack(&len,&num,recpkt))	/* What was the reply? */
   {
    case 'N':                           /* NAK, just stay in this state, */
      if (n != (num=(--num<0)?63:num))	/*  unless NAK for next packet, */
	return(state);			/*  which is just like an ACK */
					/*  for this packet, fall thru to... */
    case 'Y':                           /* ACK */
      if (n != num) return(state);	/* If wrong ACK, stay in F state */
      numtry = 0;			/* Reset try counter */
      n = (n+1)%64;			/* Bump packet count */
      size = bufill(packet);		/* Get first data from file */
      return('D');                      /* Switch state to D */

    case FALSE: return(state);		/* Receive failure, stay in F state */
    default:	return('A');            /* Something esle, just "abort" */
   }
}


/*
 *	s d a t a
 *
 *	Send File Data
 */

char sdata()
{
  int num, len; 			/* Packet number, length */

  if (numtry++ > MAXTRY) return('A');   /* If too many tries, give up */
  spack('D',n,size,packet);             /* Send a D packet */

  switch(rpack(&len,&num,recpkt))	/* What was the reply? */
   {
    case 'N':                           /* NAK, just stay in this state, */
      if (n != (num=(--num<0)?63:num))	/*  unless NAK for next packet, */
	return(state);			/*  which is just like an ACK */
					/*  for this packet, fall thru to... */
    case 'Y':                           /* ACK */
      if (n != num) return(state);	/* If wrong ACK, fail */
      numtry = 0;			/* Reset try counter */
      n = (n+1)%64;			/* Bump packet count */
      if ((size = bufill(packet)) == EOF) /* Get data from file */
	return('Z');                    /* If EOF set state to that */
      return('D');                      /* Got data, stay in state D */

    case FALSE: return(state);		/* Receive failure, stay in D */
    default:	return('A');            /* Anything else, "abort" */
   }
}


/*
 *	s e o f
 *
 *	Send End-Of-File.
 */

char seof()
{
  int num, len; 			/* Packet number, length */
  if (debug) fprintf(stderr,"seof\n");
  if (numtry++ > MAXTRY) return('A');   /* If too many tries, "abort" */
  spack('Z',n,0,packet);                /* Send a 'Z' packet */
  if (debug) fprintf(stderr,"seof1 ");
  switch(rpack(&len,&num,recpkt))	/* What was the reply? */
   {
    case 'N':                           /* NAK, fail */
      if (n != (num=(--num<0)?63:num))	/* ...unless for previous packet, */
	return(state);			/*  in which case, fall thru to ... */
    case 'Y':                           /* ACK */
      if (debug) fprintf(stderr,"seof2 ");
      if (n != num) return(state);	/* If wrong ACK, hold out */
      numtry = 0;			/* Reset try counter */
      n = (n+1)%64;			/* and bump packet count */
      if (debug) fprintf(stderr,"closing %s, ",filnam);
      close(fd);			/* Close the input file */
      if (debug) fprintf(stderr,"ok, getting next file\n");
      if (gnxtfl() == FALSE)		/* No more files go? */
	return('B');                    /* if not, break, EOT, all done */
      if (debug) fprintf(stderr,"new file is %s\n",filnam);
      return('F');                      /* More files, switch state to F */

    case FALSE: return(state);		/* Receive failure, stay in state Z */
    default:	return('A');            /* Something else, "abort" */
   }
}


/*
 *	s b r e a k
 *
 *	Send Break (EOT)
 */

char sbreak()
{
  int num, len; 			/* Packet number, length */
  if (debug) fprintf(stderr,"sbreak\n");
  if (numtry++ > MAXTRY) return('A');   /* If too many tries "abort" */

  spack('B',n,0,packet);                /* Send a B packet */
  switch (rpack(&len,&num,recpkt))	/* What was the reply? */
   {
    case 'N':                           /* NAK, fail */
      if (n != (num=(--num<0)?63:num))	/* ...unless for previous packet, */
	return(state);			/*  in which case, fall thru to ... */

    case 'Y':                           /* ACK */
      if (n != num) return(state);	/* If wrong ACK, fail */

      numtry = 0;			/* Reset try counter */
      n = (n+1)%64;			/* and bump packet count */
      return('C');                      /* switch state to Complete */

    case FALSE: return(state);		/* Receive failure, stay in state B */
    default:	return ('A');           /* Other, "abort" */
   }
}


/*
 *	r e c s w
 *
 *	This is the state table switcher for receiving files.
 */

recsw()
{
  char rinit(),rdata(),rfile(); 	/* Use these procedures */

  state = 'R';                          /* Receive is the start state */
  n = 0;				/* Initialize message number */
  numtry = 0;				/* Say no tries yet */

  while(TRUE) switch(state)		/* Do until done */
   {
    case 'D':   state = rdata(); break; /* Data receive state */
    case 'F':   state = rfile(); break; /* File receive state */
    case 'R':   state = rinit(); break; /* Send initiate state */
    case 'C':   return(TRUE);           /* Complete state */
    case 'A':   return(FALSE);          /* "Abort" state */
    }
  }

/*
 *	r i n i t
 *
 *	Receive Initialization
 */

char rinit()
{
  int len, num; 			/* Packet length, number */

  if (numtry++ > MAXTRY) return('A');   /* If too many tries, "abort" */
  switch(rpack(&len,&num,packet))	/* Get a packet */
  {
   case 'S':                            /* Send-Init */
     rpar(packet);			/* Get the other side's init data */
     spar(packet);			/* Fill up packet with my init info */
     spack('Y',n,6,packet);             /* ACK with my parameters */
     oldtry = numtry;			/* Save old try count */
     numtry = 0;			/* Start a new counter */
     n = (n+1)%64;			/* Bump packet number, mod 64 */
     return('F');                       /* Enter File-Send state */

   case FALSE:	return (state); 	/* Didn't get a packet, keep waiting */
   default:	return('A');            /* Some other packet type, "abort" */
  }
}


/*
 *	r f i l e
 *
 *	Receive File Header
 */

char rfile()
{
 int num, len;				/* Packet number, length */

 if (numtry++ > MAXTRY) return('A');    /* "abort" if too many tries */
 switch(rpack(&len,&num,packet))	/* Get a packet */
  {
   case 'S':                            /* Send-Init, maybe our ACK lost */
     if (oldtry++ > MAXTRY) return('A'); /* If too many tries, "abort" */
     if (num == ((n==0)?63:n-1))	/* Previous packet, mod 64? */
      { 				/* Yes, ACK it again */
       spar(packet);			/* with our Send-Init parameters */
       spack('Y',num,6,packet);         /*  ... */
       numtry = 0;			/* Reset try counter */
       return(state);			/* Stay in this state */
      }
     else return('A');                  /* Not previous packet, "abort" */

   case 'Z':                            /* End-Of-File */
     if (oldtry++ > MAXTRY) return('A');
     if (num == ((n==0)?63:n-1))	/* Previous packet, mod 64? */
      { 				/* Yes, ACK it again. */
       spack('Y',num,0,0);
       numtry = 0;
       return(state);			/* Stay in this state */
      }
     else return('A');                  /* Not previous packet, "abort" */

   case 'F':                            /* File Header, */
     if (num != n) return('A');         /* which is what we really want */
					/* The packet number must be right */
     if (!getfil(packet))		/* Try to open a new file */
      {
       fprintf(stderr,"Could not create %s\n"); /* Give up if can't */
       return('A');
      }
     else				/* OK, give message */
       if (!host) printf("Receiving %s\n",packet);
     spack('Y',n,0,0);                  /* Acknowledge the file header */
     oldtry = numtry;			/* Reset try counters */
     numtry = 0;			/* ... */
     n = (n+1)%64;			/* Bump packet number, mod 64 */
     return('D');                       /* Switch to Data state */

   case 'B':                            /* Break transmission (EOT) */
     if (num != n) return ('A');        /* Need right packet number here */
     spack('Y',n,0,0);                  /* Say OK */
     return('C');                       /* Go to complete state */

   case FALSE: return(state);		/* Couldn't get packet, keep trying */
   default:	return ('A');           /* Some other packet, "abort" */
  }
}


/*
 *	r d a t a
 *
 *	Receive Data
 */

char rdata()
{
 int num, len;				/* Packet number, length */

 if (numtry++ > MAXTRY) return('A');    /* "abort" if too many tries */
 switch(rpack(&len,&num,packet))	/* Get packet */
  {
   case 'D':                            /* Got Data packet */
     if (num != n)			/* Right packet? */
      { 				/* No */
       if (oldtry++ > MAXTRY) return('A'); /* If too many tries, give up */
       if (num == ((n==0)?63:n-1))	/* Else check packet number */
	{				/* Previous packet again? */
	 spack('Y',num,6,packet);       /* Yes, re-ACK it */
	 numtry = 0;			/* Reset try counter */
	 return(state); 		/* Stay in D, don't write out data! */
	}
       else return('A');                /* sorry wrong number */
      }
					/* Got data with right packet number */
     bufemp(packet,fd,len);		/* Write the data to the file */
     spack('Y',n,0,0);                  /* Acknowledge the packet */
     oldtry = numtry;			/* Reset the try counters */
     numtry = 0;			/* ... */
     n = (n+1)%64;			/* Bump packet number, mod 64 */
     return('D');                       /* Remain in data state */

   case 'F':                            /* Got a File Header */
     if (oldtry++ > MAXTRY) return('A'); /* If too many tries, "abort" */
     if (num == ((n==0)?63:n-1))	/* Else check packet number */
      { 				/* It was the previous one */
       spack('Y',num,0,0);              /* ACK it again */
       numtry = 0;			/* Reset try counter */
       return(state);			/* Stay in Data state */
      }
     else return('A');                  /* Not previous packet, "abort" */

   case 'Z':                            /* End-Of-File */
     if (num != n) return('A');         /* Must have right packet number */
     spack('Y',n,0,0);                  /* OK, ACK it. */
     close(fd); 			/* Close the file */
     n = (n+1)%64;			/* Bump packet number */
     return('F');                       /* Go back to Receive File state */

   case FALSE:	return(state);		/* No packet came, keep waiting */
   default:	return('A');            /* Some other packet, "abort" */
  }
}

/*
 *	c o n n e c t
 *
 *	Establish a virtual terminal connection with the remote host, over an
 *	assigned tty line.
 *
 */

connect()
{
  int parent;				/* Fork handle */
  char c = NULL, r = '\r';

  if (host)		/* If in host mode, nothing to connect to */
   {
    fprintf(stderr,"Kermit: nothing to connect to\n");
    return;
   }

  parent = fork();	/* Start fork to get typeout from remote host */

  if (parent)		/* Parent passes typein to remote host */
   {
    printf("Kermit: connected.\r\n");   /* Give message */
    stty(0,&rawmode);			/* Put tty in raw mode */
    read(0,&c,1);			/* Get a character */
    while ((c&0177) != escchr)		/* Check for escape character */
     {					/* Not it */
      write(remfd,&c,1);		/* Write the character on screen */
      c = NULL; 			/* Nullify it */
      read(0,&c,1);			/* Get next character */
     }					/* Until escape character typed */
    kill(parent,9);			/* Done, get rid of fork */
    stty(0,&cookedmode);		/* Restore tty mode */
    printf("\nKermit: disconnected.\n"); /* Give message */
    return;				/* Done */
   }
  else			/* Child does the reading from the remote host */
   {
    while(1)				/* Do this forever */
     {
      read(remfd,&c,1);
      write(1,&c,1);
     }
   }
}

/*
 *	KERMIT utilities.
 */

clkint()				/* Timer interrupt handler */
{
 longjmp(env,TRUE);			/* Tell rpack to give up */
}

/* tochar converts a control character to a printable one by adding a space */

char tochar(ch)
char ch;
{
  return(ch + ' ');             /* make sure not a control char */
}


/* unchar undoes tochar */

char unchar(ch)
char ch;
{
  return(ch - ' ');             /* restore char */
}


/*
 * ctl turns a control character into a printable character by toggling the
 * control bit (ie. ^A becomes A and A becomes ^A).
 */

char ctl(ch)
char ch;
{
 return(ch ^ 64);		/* toggle the control bit */
}


/*
 *	s p a c k
 *
 *	Send a Packet
 */

spack(type,num,len,data)
char type, *data;
int num, len;
{
  int i;				/* Character loop counter */
  char chksum, buffer[100];		/* Checksum, packet buffer */
  register char *bufp;			/* Buffer pointer */

  bufp = buffer;			/* Set up buffer pointer */
  for (i=1; i<=pad; i++) write(remfd,&padchar,1); /* Issue any padding */

  *bufp++ = SOH;			/* Packet marker, ASCII 1 (SOH) */
  chksum = tochar(len+3);		/* Initialize the checksum */
  *bufp++ = tochar(len+3);		/* Send the character count */
  chksum = chksum + tochar(num);	/* Init checksum */
  *bufp++ = tochar(num);		/* Packet number */
  chksum = chksum + type;		/* Accumulate checksum */
  *bufp++ = type;			/* Packet type */

  for (i=0; i<len; i++) 		/* Loop for all data characters */
   {
    *bufp++ = data[i];			/* Get a character */
    chksum = chksum+data[i];		/* Accumulate checksum */
   }
  chksum = (chksum + (chksum & 192) / 64) & 63; /* Compute final checksum */
  *bufp++ = tochar(chksum);		/* Put it in the packet */
  *bufp = eol;				/* Extra-packet line terminator */
  write(remfd, buffer,bufp-buffer+1);	/* Send the packet */
  if (debug) putc('.',stderr);
}

/*
 *  r p a c k
 *
 *  Read a Packet
 *
 */

rpack(len,num,data)
int *len, *num; 			/* Packet length, number */
char *data;				/* Packet data */
{
 int i, done;				/* Data character number, loop exit */
 char chksum, t, type;			/* Checksum, current char, pkt type */

#if UNIX				/* TOPS-20 can't handle timeouts... */
 if (setjmp(env)) return FALSE; 	/* Timed out, fail */
 signal(SIGALRM,clkint);		/* Setup the timeout */
 if ((timint > MAXTIM) || (timint < MINTIM)) timint = MYTIME;
 alarm(timint);

 flushinput();				/* Flush any pending input */
#endif /* UNIX */

 while (t != SOH)			/* Wait for packet header */
  {
   read(remfd,&t,1);
   if (!image) t &= 0177;		/* Handle parity */
  }

 done = FALSE;				/* Got SOH, init loop */
 while (!done)				/* Loop to get a packet */
  {
   read(remfd,&t,1);			/* Get character */
   if (!image) t &= 0177;		/* Handle parity */
   if (t == SOH) continue;		/* Resynchronize if SOH */

   chksum = t;				/* Start the checksum */
   *len = unchar(t)-3;			/* Character count */

   read(remfd,&t,1);			/* Get character */
   if (!image) t &= 0177;		/* Handle parity */
   if (t == SOH) continue;		/* Resynchronize if SOH */
   chksum = chksum + t; 		/* Accumulate checksum */
   *num = unchar(t);			/* Packet number */

   read(remfd,&t,1);			/* Get character */
   if (!image) t &= 0177;		/* Handle parity */
   if (t == SOH) continue;		/* Resynchronize if SOH */
   chksum = chksum + t; 		/* Accumulate checksum */
   type = t;				/* Packet type */

   for (i=0; i<*len; i++)		/* The data itself, if any */
    {					/* Loop for character count */
     read(remfd,&t,1);			/* Get character */
     if (!image) t &= 0177;		/* Handle parity */
     if (t == SOH) continue;		/* Resynch if SOH */
     chksum = chksum + t;		/* Accumulate checksum */
     data[i] = t;			/* Put it in the data buffer */
    }
   data[*len] = 0;			/* Mark the end of the data */

   read(remfd,&t,1);			/* Get last character (checksum) */
   if (!image) t &= 0177;		/* Handle parity */
   if (t == SOH) continue;		/* Resynchronize if SOH */
   done = TRUE; 			/* Got checksum, done */
  }
#if UNIX
 alarm(0);				/* Disable the timer interrupt */
#endif

 chksum = (chksum + (chksum & 192) / 64) & 63; /* Fold bits 7,8 into chksum */
 if (chksum != unchar(t)) return(FALSE); /* Check the checksum, fail if bad */

 return(type);				/* All OK, return packet type */
}



/*
 *	b u f i l l
 *
 *	Get a bufferful of data from the file that's being sent.
 *	Only control-quoting is done; 8-bit & repeat count prefixes are
 *	not handled.
 */

bufill(buffer)
char buffer[];				/* Buffer */
 {
  int i;				/* Loop index */
  char t,t7;
  i = 0;				/* Init data buffer pointer */
  while(read(fd,&t,1) > 0)		/* Get the next character */
   {
    if (image)				/* In 8-bit mode? */
     {
      t7 = t & 0177;			/* Yes, look at low-order 7 bits */
      if (t7 < SP || t7==DEL || t7==quote) /* Control character? */
       {				/*    Yes, */
	buffer[i++] = quote;		/*    quote this character */
	if (t7 != quote) t = ctl(t);	/* and uncontrollify */
       }
     }
    else				/* Else, ASCII text mode */
     {
      t &= 0177;			/* Strip off the parity bit */
      if (t < SP || t == DEL || t == quote) /* Control character? */
       {				/* Yes */
	if (t=='\n')                    /* If newline, squeeze CR in first */
	 {
	  buffer[i++] = quote;
	  buffer[i++] = ctl('\r');
	 }
	buffer[i++] = quote;		/* Insert quote */
	if (t != quote) t=ctl(t);	/* Uncontrollified the character */
       }
     }
    buffer[i++] = t;			/* Deposit the character itself */
    if (i >= spsiz-8) return(i);	/* Check length */
   }
  if (i==0) return(EOF);		/* Wind up here only on EOF */
  return(i);				/* Handle partial buffer */
}

/*
 *	b u f e m p
 *
 *	Get data from an incoming packet into a file.
 */

bufemp(buffer,fd,len)
char buffer[];				/* Buffer */
int fd, len;				/* File pointer, length */
 {
  int i;				/* Counter */
  char t;				/* Character holder */

  for (i=0; i<len; i++) 		/* Loop thru the data field */
   {
    t = buffer[i];			/* Get character */
    if (t == MYQUOTE)			/* Control quote? */
     {					/* Yes */
      t = buffer[++i];			/* Get the quoted character */
      if ((t & 0177) != MYQUOTE)	/* Low order bits match quote char? */
	t = ctl(t);			/* No, uncontrollify it */
     }
    if (image || (t != CR))		/* Don't pass CR in text mode */
      write(fd,&t,1);			/* Put the char in the file */
   }
 }


/*
 *	g e t f i l
 *
 *	Open a new file
 */

getfil(filenm)				/* Bizarre logic, as this is called */
char *filenm;				/* only with packet as the arg! */
{
  if (filenm[0] == '\0')
    fd = creat(packet,0644);		/* If filename known, use it */
  else
    fd = creat(filenm,0644);		/* else use sourcefile name */
  return (fd > 0);			/* Return false if file won't open */
}

/*
 *	g n x t f l
 *
 *	Get next file in a file group
 */

gnxtfl()
{
  if (debug) fprintf(stderr, "gnxtfl\n");
  filnam = *(filelist++);
  if (filnam == 0) return FALSE;	/* If no more, fail */
  else return TRUE;			/* else succeed */
}

/*
 *	s p a r
 *
 *	Fill the data array with my send-init parameters
 *
 */

spar(data)
char data[];
{
  data[0] = tochar(MAXPACK);		/* Biggest packet I can receive */
  data[1] = tochar(MYTIME);		/* When I want to be timed out */
  data[2] = tochar(MYPAD);		/* How much padding I need */
  data[3] = ctl(MYPCHAR);		/* Padding character I want */
  data[4] = tochar(MYEOL);		/* End-Of-Line character I want */
  data[5] = MYQUOTE;			/* Control-Quote character I send */
}


/*	r p a r
 *
 *	Get the other host's send-init parameters
 *
 */

rpar(data)
char data[];
{
  spsiz = unchar(data[0]);		/* Maximum send packet size */
  timint = unchar(data[1]);		/* When I should time out */
  pad = unchar(data[2]);		/* Number of pads to send */
  padchar = ctl(data[3]);		/* Padding character to send */
  eol = unchar(data[4]);		/* EOL character I must send */
  quote = data[5];			/* Incoming data quote character */
}

#if UNIX
flushinput()
{
  long int count;			/* Number of bytes ready to read */
  long int i;				/* Number of bytes to read in loop */

  ioctl(remfd, FIONREAD, &count);	/* See how many bytes pending read */
  if (!count) return;			/* If zero, then no input to flush */

  while (count) {			/* Loop till all are flushed */
      i = (count<sizeof(recpkt)) ?	/* Read min of count and size of */
	       count : sizeof(recpkt);	/*  the read buffer */
      read(remfd, recpkt, i);		/* Read a bunch */
      count -= i;			/* Subtract from amount to read */
  }
}
#endif /* UNIX */
