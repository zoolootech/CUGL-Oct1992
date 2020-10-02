/*
    bbscport.c

    Written By Mike Kelly

    Modified to run under UNIX I/O control by Dave Stanhope.

    03/09/83 v1.0   written
    07/08/83 v1.0   Added code to look for modem carrier detect,
            and exit() if not still connected.
*/

#include "bbscdef.h"
#include <sgtty.h>
#include <sys/ioctl.h>

#define LASTDATE  " 07/08/83 "

#define PGMNAME "BBSCPORT "
#define VERSION " 1.0 "

portinstat()    /*  returns 1 if no char, 0 if char waiting */
    {
    long count  ;
        ioctl(STDIN, FIONREAD, &count) ; /* see if any previous data  */
    if(count > 0) return(0) ; else return(1) ;
    }
  
char portin()       /* get one byte from the port */
    {
    char byte;
    if(read(STDIN, &byte, 1) != 1) return(CPMEOF) ;
    return(byte & 0x7F);
    }

portsin(buf,max)    /* get a line of input max. chars long */
int max ; char *buf ;
    {
    int cnt, byte ; char bytex ;
    cnt = 0;
    byte = FALSE;
    while (++cnt <= max && byte != '\r')
        {
        while((byte = portin()) < ' ' || byte > '}')
            {
            if (byte == '\r') { break ; } /* carriage return */
            if (byte == '\b' && cnt > 1)    /* backspace */
                {
                portout(byte);
                portout(' ');
                portout(byte);
                *buf--; /* backout last char */
                cnt--;  /* decrement count too */
                }
            }
        if (byte != '\r')
            {
            *buf++ = byte;
            }
        portout(byte);  /* echo good chars only */
        }
    *buf++  = '\0';         /* tag \0 on end */
    }

portout(byte)       /* send one byte to the port */
char byte;      /* return CTL_K for those times want to check */
    {       /* if the person wants to stop sending        */
    char byte0 ;
    if (portinstat() == 0)          /* == 0 means char waiting */
        {
        if ((byte0 = portin()) == 0x13) /* ctl-S?, then */
            {           /*  simulate xon/xoff */
            portin();       /* gobble up the char */
            }
        if (byte0 == CTL_K || byte0 == 'K' || byte0 == 'k')
            {           /* look for stop sending key */
            stop_that = TRUE;   /* if so, set switch on      */
            }
        }
    byte0 = byte ; write(STDOUT,&byte0,1) ; /* send the byte */
    return(OK) ;
    }
  
portsout(string)    /* send a string to the port */
char *string ;
    {
    char byte ;

    while (byte = (*string++))
        {
        portout(byte) ;     /* send one byte at a time */
        }
    }

portlsout(string,len)  /* send a string to the port, pad to length */
char *string ; int len ;
    {
    char byte ;

    while (byte = (*string++))
        {
        portout(byte) ;     /* send one byte at a time */
        len-- ;
        }
    while (len > 0) { portout(' ') ; len-- ; } /* pad with spaces */
    }

porttype(tbuf)      /* type a file to the port */
FILE    *tbuf ;
    {
    char byte ;

    stop_that = FALSE;  /* reset switch */
    portsout("\r\nType ctl-K or K to skip this\r\n\n");
    while (((byte = getc (tbuf)) != EOF) && (byte != CPMEOF))
        {
        if(byte == '\n') portout('\r') ;
        portout(byte);
        if (stop_that)       /* received ctl-K or K */
            {
            portsout(CRLF);
            stop_that = FALSE;  /* reset switch */
            return;         /* nuf's enough */
            }
        }
    }

portinit()
    {
    /* set raw mode for this terminal */
    struct sgttyb arg;
    ioctl (STDIN, TIOCGETP, &arg);
    arg.sg_flags |= RAW ;
    arg.sg_flags &= ~ECHO;
    ioctl (STDIN, TIOCSETP, &arg);
    }

portrst()
    {
    /* set raw mode for this terminal */
    struct sgttyb arg;
    ioctl (STDIN, TIOCGETP, &arg);
    arg.sg_flags &= ~RAW;
    arg.sg_flags |= ECHO ;
    ioctl (STDIN, TIOCSETP, &arg);
    }

char gobble()               /* gobble up any answer */
    {
    int cnt = 0 ;
    while (cnt++ < 20) portin ;
    }

/*  end of program      */