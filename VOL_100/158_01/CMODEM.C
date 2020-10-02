/*  HEADER:     CUGaaa.bb;
    FILENAME:   cmodem.c;
    TITLE:      Xmodem communications in c;
    COMPILERS:  C86;
    SYSTEM:     ms-dos;
    AUTHORS:    Bill Rogers;
    VERSION:    5.01;
    DATE:       08/04/1985;
    KEYWORDS:   commnications, xmodem, ms-dos, pc-dos, 8251a, 8530;
    SEE-ALSO:   local.h;
    DESCRIPTION:
        "This program implements an elementary version of xmodem for
        under ms-dos or pc-dos.";
    WARNINGS:
        "The zilog usart option is not yet tested.";
 */
/*      The calling sequence is:
 *
 *          cmodem
 *
 *      The control sequences are:
 *
 *          <ctrl-z><ctrl-z>    send <ctrl-z>
 *          <ctrl-z>b300        set baud rate to 300 baud
 *          <ctrl-z>b1200       set baud rate to 12000 baud
 *          <ctrl-z>d           dial telephone number
 *          <ctrl-z>e           exit without hangup
 *          <ctrl-z>h           hangup without exit
 *          <ctrl-z>i           initialize port & baud rate
 *          <ctrl-z>nx...x      enter telephone number x...x
 *          <ctrl-z>q           quit with hangup
 *          <ctrl-z>rx...x      receive file name x...x
 *          <ctrl-z>sx...x      send file name x...x
 *          <ctrl-z>t           trace file transmissions
 *          <ctrl-z>?           display commands
 *
 *      Revision history: Latest revision first.
 *
 *      12/Dec/84   Changed to C86 (Bill Rogers).
 *      04/Jun/82   Removed PMMI defines and ifdef for Australia.  Much 
 *                  more  extensive  documentation  added.  Modem  port 
 *                  defines now obtained from BDSCIO.H.  Code tidied up 
 *                  a  bit  and VARBAUD  ifdefs  added.  Some  messages 
 *                  changed and numerous small changes ( Bill Bolton).
 *      ??/???/81   Patched together from existing Cnet and YAM code
 *                  (Steve Passe, Cnode SYSOP, USA).
 *  SCCS IDENTIFICATION */
        static char SCCSID[] = "@(#)cmodem.c  5.0.1";
/*--------------------------------------------------------------------*/
/*  INCLUDE */
#define  EXTERN extern
#include <local.h>
#undef   EXTERN
/*--------------------------------------------------------------------*/
/*  DEFINE */

/*  Select USART interface type. */

#define     OCTAPORT            /* Lomas HAZITALL or Lomas OCTAPORT */

/*  ascii characters. */

#define     NUL         0x00
#define     SOH         0x01
#define     EOT         0x04
#define     ACK         0x06
#define     HT          0x09
#define     LF          0x0a
#define     CR          0x0d
#define     NAK         0x15
#define     CAN         0x18
#define     SUB         0x1a    /*  ^Z */
#define     ESC         0x1b

/*  Escape character for control. */

#define     ESCCHAR     0x1a    /*  ^Z */

#ifdef      HAZITALL

/*  Intel 8253 programmable interval timer definition. */

#define     CLOAD       0x8d    /*  load port. */
#define     CCTRL       0x0f    /*  control port. */
#define     CCTRLB      0x76    /*  select counter 1,
                                    load lsb, then msb,
                                    square wave generator,
                                    binary counter. */
#define     B0300       417     /*   300 baud. */
#define     B1200       104     /*  1200 baud. */

/*  Intel 8251a serial port definition. */

#define     MDATA       0x84    /*  Data port. */
#define     MSTAT       0x85    /*  Status port. */
#define     MOMASK      0x01    /*  Output ready mask. */
#define     MIMASK      0x02    /*  Input ready mask. */

#define     MCMD1       0x40    /*  Internal reset. */
#define     MMODE       0x6e    /*  One stop bit,
                                    even parity,
                                    disable parity,
                                    8 bit character length,
                                    16x baud rate factor. */
#define     MCMD2       0x37    /*  NA,
                                    internal reset disabled,
                                    rts enabled,
                                    error reset enabled,
                                    break disabled,
                                    receive enabled,
                                    dtr enabled,
                                    send enabled. */

#endif

#ifdef      OCTAPORT

/*  Zilog 8530 serial port definition, including timer. */

#define     B0300       830
#define     B1200       206

#define     MDATA       0x20    /*  Data port. */
#define     MSTAT       0x21    /*  Status port. */
#define     MCMND       0x22    /*  Command port. */
#define     MOMASK      0x04    /*  Output ready mask. */
#define     MIMASK      0x01    /*  Input ready mask. */

#define     MDEFSNUM    10
static unsigned char MDEFS[MDEFSNUM] = {
    0x09,   /*  Select wr09 - master interrupt control. */
    0xc0,   /*      7-6: force hardware reset. 
                    5-0: - */
    0x0f,   /*  Select wr15 - external status/control. */
    0x40,   /*      7-0: Disable all interrupts. */
    0x04,   /*  Select wr04 - receiver and transmitter control. */
    0x44,   /*      7-6: x16 clock,
                    5-4: 0
                    3-2: 1 stop bits,
                    1-1: even parity,
                    0-0: no parity. */
    0x05,   /*  Select wr05 - transmitter control. */
    0xea,   /*      7-7: assert dtr,
                    6-5: transmit 8 bits,
                    4-4: disable break,
                    3-3: enable transmit. 
                    2-2: 0
                    1-1: assert rts. 
                    0-0: - */
    0x03,   /*  Select wr03 - receiver control. */
    0xc1    /*      7-6: receive 8 bits,
                    5-5: no autoenables,
                    4-1: 0,
                    0-0: enable receive. */
};

#define     MBAUDNUM    10
#define     MBAUDLOW    3       /*  Index to baudrate low byte. */
#define     MBAUDHIGH   5       /*  Index to baudrate high byte. */
static unsigned char MBAUD[MBAUDNUM] = {
    0x0e,   /*  Select wr14- miscellaneous control. */
    0x00,   /*      7-1: -
                    0-0: Disable baudrate generator. */
    0x0c,   /*  Select wr12 - low byte of baudrate constant. */
    11,     /*      7-0: 1200 baud. */
    0x0d,   /*  Select wr13 - high byte of baudrate constant. */
    0,      /*      7-0: 1200 baud. */
    0x0b,   /*  Select wr11 - clock mode control. */
    0x56,   /*      7-7: no crystal,
                    6-5: receive clock = baudrate generator output,
                    4-3: transmit clock = baudrate generator output,
                    2-2: -
                    1-0: trxc-out=baudrate generator. */
    0x0e,   /*  Select wr14 - miscellaneous control. */
    0x03    /*      7-2: -
                    1-1: pclk source of clock,
                    0-0: Enable baudrate generator. */
};
#endif

/*  Hayes modem command strings. */

#define     ESCAPE_STR  "+++"
#define     HANGUP_STR  "AT H0 \r"
#define     DIAL_STR    "AT DT "
#define     CR_STR      "\r"

/*  DOS interrupts. */

#define     DIR_IO      0x06    /*  DOS function request - direct i/o */
#define     INPUT       0xff    /*  DOS function sub-request - accept */

/*  Flag values. */

#define     OK          ( 0)
#define     ERROR       (-1)
#define     WCEOT       (-2)
#define     TIMEOUT     (-3)
#define     EMPTY       (-4)

#define     SECSIZ      128     /*  Block size. */
 
#define     RETRYMAX    10
#define     PATHLEN     64      /*  Send/receive file pathname length */
#define     CLKMHZ      8       /*  CPU speed in Mhz */
#define     TIMECON     311     /*  Constant to convert CLKMHZ to ticks.
                                 */

#define     CONSTAT     2
#define     CONIN       3
#define     NORMAL      0x1c
#define     ORIG        0x01
#define     ANSWER      0x02
#define     READY       0x5f
#define     OPT300      0x20
#define     OPT600      0x00
#define     CLEAR       0x3f
/*--------------------------------------------------------------------*/
static int          BAUDRATE;
static string       PHONENUM[33] = "1,201-866-2258";
static bool         TRACEFLAG    = false;

static string       YN[65];
static bool         SENDOPEN;             
static bool         RCVOPEN;          
static string       SENDNAME[PATHLEN + 1];    
static string       RCVNAME[PATHLEN + 1]; 
static file        *SENDFILE;      
static file        *RCVFILE;   

static int          T1PAUSE;
static char         CHECKSUM;
static char         LASTRX;
static int          WCJ;
static int          FIRSTCH;

extern file        *fopen();
/*--------------------------------------------------------------------*/
static void DELAY(P_DELAY)

    int P_DELAY;                /*  In. */

    /*  Delay, in units of 0.01 sec. */
{
    static int  TICKS = 465;   /*  For 8mhz 8086. */
    int         I;
    int         J;
    int         X = 0;
/*  begin */
    for (I = 0 ; I < P_DELAY ; I++ ) {
        for (J = 0 ; J < TICKS ; J++) {
            X = X;
        }
    }
}
/*--------------------------------------------------------------------*/
static void PRNALLCH(P_C)

    metachar P_C;               /*  In. */

    /*  Print character. 

        Char      Print

        eof       ^* 
        < 0       ^:
        control   ^x  (x = '@'...'_')
        tilde     ^>
        del       ^?
        >del      .
    */
{
/*  begin */
    if (P_C == -1) {
        putchar('^');
        putchar('*');
    } else if (P_C < 0) {
        putchar('^');
        putchar(':');
    } else if (P_C == LF) {
        putchar(P_C);
    } else if (P_C == CR) {
        putchar(P_C);
    } else if (P_C < ' ') {
        putchar('^');
        putchar(P_C + 0x40);
    } else if (P_C <= '}') {
        putchar(P_C);
    } else if (P_C <= 0x7f) {
        putchar('^');
        putchar(P_C + 0x20);
    } else {
        putchar('.');
    }
}
/*--------------------------------------------------------------------*/
static metachar IN_MODEM()

    /*  Receive byte from modem port. */
{
    metachar        RTN;
    extern metachar inportb();
/*  begin */
    if (inportb(MSTAT) & MIMASK) {      /*  status & char ready bit */
        RTN = inportb(MDATA) & 0x00ff;  /*  ok, get the char */
    } else {
        RTN = EMPTY;                    /*  return empty */
    }
    return RTN;
}
/*--------------------------------------------------------------------*/
static void OUT_MODEM(P_OUT_CHAR)

    char P_OUT_CHAR;

    /*  Send byte to modem port. */
{
    extern metachar inportb();
    extern void     outportb();
/*  begin */
    while ( ! (inportb(MSTAT) & MOMASK)) {
        ;                                /* wait */
    }
    outportb(MDATA,P_OUT_CHAR);    /* finally, send it */
}
/*--------------------------------------------------------------------*/
static char GETCH()

    /*  Accept character from keyboard,
        no wait, no ^C, no echo, no buffer flush. */
{
    char            RTN;
    extern metachar bdos();
/*  begin */
    RTN = bdos(DIR_IO,INPUT) & 0x007f;
    return RTN;
}
/*--------------------------------------------------------------------*/
static metachar READBYT(P_DELAY)
    
    int P_DELAY;

    /*  Receive byte. Delay in units of 0.01 sec. */
{
    metachar        RTN;
    static int      TICKS = 10;     /* For 8mhz 8086. */
    int             I;
    int             J;
    extern metachar inportb();
/*  begin */
    for (I = 0 ; I < P_DELAY ; I++) {
        for (J = 0 ; J < TICKS ; J++) {
            RTN = IN_MODEM();
            if (RTN  != EMPTY) {
                return RTN;
            }
            if (GETCH()) {      /*  Local forced timeout. */
                RTN = TIMEOUT;
                return RTN;
            }
        }
    }
    RTN = TIMEOUT;
    return RTN;
}
/*--------------------------------------------------------------------*/
static void DELAY_ECHO(P_DELAY)

    int P_DELAY;

    /*  Wait for echo, and echo to screen. Delay in units of 0.01 sec.*/
{
    static int  TICKS = 130;    /*  For 8mhz 8086. */
    int         I;
    int         J;
    int         X = 0;
    metachar    C;
    metachar    IN_MODEM();
/*  begin */
    for (I = 0 ; I < P_DELAY ; I++) {
        for (J = 0 ; J < TICKS ; J++) {
            C = IN_MODEM();
            if (C != EMPTY) {
                if (C != TIMEOUT) { 
                    PRNALLCH(C);
                }
            }
        }
    }
}
/*--------------------------------------------------------------------*/
static void OUT_STR(P_OUT_STR)    

    string  P_OUT_STR[];

    /*  Send string to modem port. */
{
    int         I;
    metachar    C;
/*  begin */
    I = 0;
    while (P_OUT_STR[I] != '\0') {
        OUT_MODEM(P_OUT_STR[I]);
        C = READBYT(10);
        if ( ! ((C == EMPTY) || (C == TIMEOUT))) {
            PRNALLCH(C);
        }
        I++;
    }
}
/*--------------------------------------------------------------------*/
static void PURGELINE()

    /*  Drain modem input. */
{
    metachar        TEMP;
    extern metachar inportb();
/*  begin */
    while (inportb(MSTAT) & MIMASK) {
        TEMP = inportb(MDATA);
    }
}
/*--------------------------------------------------------------------*/
static int  FILBUF(P_BUF,P_COUNT)

    char       *P_BUF;
    int         P_COUNT;

    /*  fill block from send file with count chars padding with ^Z for 
        CPM */
{
    metachar        C;
    int             M;
    extern metachar fgetc();
/*  begin */
    M = P_COUNT;
    while ((C = fgetc(SENDFILE)) != EOF) {
        *P_BUF++ = C;
        if (--M == 0) {
            break;
        }
    }
    if (M == P_COUNT) {
        return 0;
    } else {
        
        /*  won't pad properly under cp/m because of two different EOF's
            (EOF & CPMEOF), and the fact that cp/m doesn't know type of 
            file instinctively, fix later */

        while(--M >= 0) {
            *P_BUF++ = SUB;         /*  ^Z. */
        }
    }
    return P_COUNT;
}
/*--------------------------------------------------------------------*/
static int  OPENTX()

    /*  Open send file. */
{
    int RTN;
/*  begin */
    SENDFILE = fopen(SENDNAME,opnreadb);
    if (SENDFILE == null) {
        printf("\n**** Send File '%s' Failed to Open ****\n",SENDNAME);
        RTN = ERROR;
    } else {
        printf("\n#### Send File '%s' Opened ####\n",SENDNAME);
        SENDOPEN = true;
        RTN = OK;
    }
    return (RTN);
}
/*--------------------------------------------------------------------*/
static void CLOSETX()

    /*  Close send file. */
{
    int FLAG;
/*  begin */
    if (SENDOPEN) {
        FLAG = fclose(SENDFILE);
        if (FLAG == ERROR) {
            printf("\n**** Send File '%s' Failed to Close ****\n",
                SENDNAME);
            exit(1);
        } else {
            printf("\n#### Send File '%s' Closed ####\n",SENDNAME);
            SENDOPEN = false;
        }
    }
}
/*--------------------------------------------------------------------*/
static int  OPENRX()

    /*  Open receive file. */
{
    int RTN;
/*  begin */
    RCVFILE = fopen(RCVNAME,opnreadb);
    if (RCVFILE != null) {
        fclose(RCVFILE);
        printf(
    "\n**** Receive File '%s' Already Exists, Try Another Name ****\n",
            RCVNAME);
        RTN = ERROR;
    } else {
        RCVFILE = fopen(RCVNAME,crewritb);
        if (RCVFILE == null) {
            printf("\n**** Receive File '%s' Cannot Be Created ****\n",
                RCVNAME);
            RTN = ERROR;
        } else {
            printf("\n#### Receive File '%s' Opened ####\n",
                RCVNAME);
            RCVOPEN = true;
            RTN = OK;
        }
    }
    return (RTN);
}
/*--------------------------------------------------------------------*/
static void CLOSERX()

    /*  Close receive file. */
{
    int FLAG;
/*  begin */
    if (RCVOPEN) {
        fflush(RCVFILE);
        FLAG = fclose(RCVFILE);
        if (FLAG == ERROR) {
            printf("\n**** Receive File '%s' Failed to Close ****\n",
                RCVNAME);
            exit(1);
        } else {
            printf("\n#### Receive File '%s' Closed ####\n",
                RCVNAME);
            RCVOPEN = false;
        }
    }
}
/*--------------------------------------------------------------------*/
static int  WCPUTSEC(P_TXBUF,P_SECTNUM)

    char   *P_TXBUF;
    int     P_SECTNUM;

    /*  Send block. */
{
    char    ATTEMPTS;
    int     I;
    char    XBYT;
/*  begin */
    FIRSTCH = 0;    /* part of logic to detect CAN CAN */
    for (ATTEMPTS = 0 ; ATTEMPTS <= RETRYMAX ; ATTEMPTS++) {
        LASTRX = FIRSTCH;
        OUT_MODEM(SOH);
        OUT_MODEM(P_SECTNUM);
        OUT_MODEM(~P_SECTNUM);  /*  <tilde>P_SECTNUM */
        CHECKSUM = 0;
        for (I = 0 ; I < SECSIZ ; I++) {
            OUT_MODEM(P_TXBUF[I]);
            if (TRACEFLAG) {
                PRNALLCH(P_TXBUF[I]);
            }
            CHECKSUM = CHECKSUM + P_TXBUF[I];
        }
        OUT_MODEM(CHECKSUM);
        PURGELINE();

        FIRSTCH = READBYT(1000);
        if (FIRSTCH == CAN && LASTRX == CAN) {
CANCAN:
            printf("\n#### Receiver CANcelled Sending ####\n");
            return ERROR;
        } else if (FIRSTCH == ACK) {
            return OK;
        } else if (FIRSTCH == TIMEOUT) {
            printf("\n**** Timeout on Block ACK Attempt %4d ****\n",
                ATTEMPTS);
        } else {
            printf("\n**** Got '%02x' for Block ACK Attempt %4d ****\n",
                FIRSTCH,ATTEMPTS);
            for( ; ; ) {
                LASTRX = FIRSTCH;
                if ((FIRSTCH = READBYT(10)) == TIMEOUT) {
                    break;
                }
                if (FIRSTCH == CAN && LASTRX == CAN) {
                    goto CANCAN;
                }
            }
        }
    }
    printf("\n**** No ACK on Block - Abort ****\n");
    return ERROR;
}
/*--------------------------------------------------------------------*/
static int  WCTX()

    /*  Send. */
{
    int     SECTNUM;
    char    ATTEMPTS;
    char    TXBUF[SECSIZ + 1];
/*  begin */
    printf("\n#### Awaiting Initial NAK . . . ####\n");
    while ((FIRSTCH = READBYT(1000)) != NAK     && 
            FIRSTCH                 != TIMEOUT && 
            FIRSTCH                 != CAN) {
        printf("\n**** Got '%02x', not NAK ****\n",FIRSTCH);
                                /*  let user see it if strange char */
    }
    if (FIRSTCH == CAN) {
        return ERROR;
    }
    if (FIRSTCH == TIMEOUT) {
        printf("\n**** Timeout on Initial NAK! ****\n");
        return ERROR;
    }
    SECTNUM = 1;
    while (FILBUF(TXBUF,SECSIZ)) {
        printf("\n#### Send Block #%4d ####\n",SECTNUM);
        if (WCPUTSEC(TXBUF,SECTNUM) == ERROR) {
            return ERROR;
        } else {
            SECTNUM++;
        }
    }
    ATTEMPTS = 0;
    do {
        printf("\n#### Sending EOT . . . ####\n");
        OUT_MODEM(EOT);
        PURGELINE();            /*  why? */
        ATTEMPTS++;
    } while ((FIRSTCH = (READBYT(1000)) != ACK) && ATTEMPTS < RETRYMAX);
                                /*  wait for ACK */

    if (ATTEMPTS == RETRYMAX) {
        printf("\n**** No ACK on EOT, Abort ****\n");
        return ERROR;
    } else {
        return OK;
    }
}
/*--------------------------------------------------------------------*/
static void UPLOAD()

    /*  Send from local computer to remote computer. */
{
    int I;
/*  begin */
    if (OPENTX() == ERROR) {
        ;
    } else {
        if (WCTX() == ERROR) {
            printf("\n**** Abort ****\n");
            for (I = 0 ; I < 10 ; I++) {
                OUT_MODEM(CAN);
            }
        }
        CLOSETX();
    }
}
/*--------------------------------------------------------------------*/
static int  WCGETSEC(P_RXBUF,P_TIME)

    char   *P_RXBUF;
    int     P_TIME;
    
    /*  This routine fetches a Ward Christensen type block.
        Returns block number encountered or ERROR if valid block not 
        received,
        or CAN CAN received
        or WCEOT if eot block
        P_TIME is timeout for first char, set to 4 seconds thereafter
                ***** NO ACK IS SENT IF SECTOR IS RECEIVED OK ****
        (Caller must do that when he is good and ready to get next 
        block). */
{
    int     SECTCURR;
    int     ERRORS;
    char   *CP;
    int     I;
/*  begin */
    for (LASTRX = ERRORS = 0 ; ERRORS < RETRYMAX ; ERRORS++) {
        do {
            FIRSTCH = READBYT(P_TIME);
        } while (FIRSTCH != SOH     && 
                 FIRSTCH != TIMEOUT && 
                 FIRSTCH != EOT     && 
                 FIRSTCH != CAN);   /*  wait for one of these */
        if (FIRSTCH == SOH) {
            SECTCURR = READBYT(10);
            if ((SECTCURR + READBYT(10)) == 255) {
                CHECKSUM = 0;
                for (CP = P_RXBUF,WCJ = 128 ; --WCJ >= 0 ; ) {
                    *CP = READBYT(10);
                    if (TRACEFLAG) {
                        PRNALLCH(*CP);
                    }
                    CHECKSUM += (*CP++);
                }
                if  (((CHECKSUM - READBYT(10)) & 0xff) == 0) {
                    return SECTCURR;
                } else {
                    printf("\n**** Checksum Error - Errors %4d ****\n",
                        ERRORS);
                }
            } else {
            printf("\n**** Block Number Garbled - Errors %4d ****\n",
                    ERRORS);
            }
        } else if (FIRSTCH == EOT) {
            printf("\n#### Received EOT ####\n");
            return WCEOT;
        } else if (FIRSTCH == CAN) {
            if (LASTRX == CAN) {
                printf("\n#### Sender CANcelled ####\n");
                return ERROR;
            } else {
                LASTRX = CAN;
                continue;
            }
        } else if (FIRSTCH == TIMEOUT) {
            printf("\n**** SOH Timeout - Errors %4d ****\n",ERRORS);
        }
        LASTRX = 0;
        while (READBYT(10) != TIMEOUT) {
            ;
        }
        OUT_MODEM(NAK);
        P_TIME = 1000;
    }

    /* try to stop the bubble machine. */

    for (I = 0 ; I < 10 ; I++) {
        OUT_MODEM(CAN);
    }
    return ERROR;
}
/*--------------------------------------------------------------------*/
static int  WCRX()
    
    /*  Receive. */
{
    int             I;
    int             SECTNUM;
    int             SECTCURR;
    char            RXBUF[SECSIZ + 1];
    char            SENDCHAR;
    int             TIMER;
    extern metachar fputc();
/*  begin */
    SECTNUM  = 0;
    SENDCHAR = NAK;
    TIMER    = 1000;
    printf("\n#### Sending Initial NAK . . . ####\n");
    for ( ; ; ) {
        printf("\n#### Receive Block #%4d ####\n",SECTNUM);
        OUT_MODEM(SENDCHAR);    /* send it now, we're ready! */
        SECTCURR = WCGETSEC(RXBUF,TIMER);
        TIMER = 200;
        if (SECTCURR == (SECTNUM + 1 & 0xff)) {    /* mask low byte */
            SECTNUM++;
            for (I = 0 ; I < SECSIZ ; I++) {
                if (fputc(RXBUF[I],RCVFILE) == ERROR) {
                    printf("\n**** Receive File '%s' Error ****\n",
                        RCVNAME);
                    return ERROR;
                }
            }
            SENDCHAR = ACK;
        } else if (SECTCURR == SECTNUM) {
            printf("\n**** Received Duplicate Block %4d ****\n",
                SECTCURR);
            SENDCHAR = ACK;
        } else if (SECTCURR == WCEOT) {
            OUT_MODEM(ACK);

            /* don't pad the file any more than it already is */

            return OK;
        } else {
            printf("\n**** Synchronization Error ****\n");
            return ERROR;
        }
    }
}
/*--------------------------------------------------------------------*/
static void DOWNLOAD()

    /*  Receive from remote computer to local computer. */
{
    int I;
/*  begin */
    if (OPENRX() == ERROR) {
        ;
    } else {
        if (WCRX() == ERROR) {
            printf("\n**** Abort ****\n");
            for (I = 0 ; I < 10 ; I++) {
                OUT_MODEM(CAN);
            }
        }
        CLOSERX();
    }
}
/*--------------------------------------------------------------------*/
static void DIAL()

    /*  Dial telephone number. */
{
/*  begin */
    printf("\n#### Dialing Telephone Number '%s' . . . ####\n",
        PHONENUM);
    DELAY(150);
    OUT_STR(ESCAPE_STR);
    DELAY_ECHO(150);
    OUT_STR(DIAL_STR);
    OUT_STR(PHONENUM);
    OUT_STR(CR_STR);
    DELAY_ECHO(300);
}
/*--------------------------------------------------------------------*/
static void HANGUP()

    /*  Hang up telephone. */
{
/*  begin */
    printf("\n#### Hanging Up Telephone . . . ####\n");
    DELAY(150);
    OUT_STR(ESCAPE_STR);
    DELAY_ECHO(150);
    OUT_STR(HANGUP_STR);
    DELAY_ECHO(300);
}
/*--------------------------------------------------------------------*/
static void DOS()

    /*  Exit to DOS. Do not hang up telephone. */
{
/*  begin */
    printf("\n#### Exit to DOS. Do not Hang Up Telephone ####\n");
    exit(0);
}
/*--------------------------------------------------------------------*/
static void QUIT()

    /*  Exit to DOS. Hang up telephone. */
{
/*  begin */
    printf("\n #### Exit to DOS. Hang Up Telephone ####\n");
    HANGUP();
    exit(0);
}
/*--------------------------------------------------------------------*/
static void BAUD()

    /*  Set baud rate. */
{
    unsigned int    LOAD_INT;

#ifdef  HAZITALL
    unsigned int    LOAD_LOW_BYTE;
    unsigned int    LOAD_HIGH_BYTE;
#endif

#ifdef  OCTAPORT
    int I;
#endif

/*  begin */
    printf("\n#### Set Baud Rate ####\n");
    if (BAUDRATE == 300) {
        LOAD_INT = B0300;
    } else {
        LOAD_INT = B1200;
    }

#ifdef  HAZITALL
    LOAD_HIGH_BYTE = (LOAD_INT >> 8) & 0x00ff;
    LOAD_LOW_BYTE  =  LOAD_INT       & 0x00ff;
    outportb(CCTRL,CCTRLB);
    outportb(CLOAD,LOAD_LOW_BYTE);
    outportb(CLOAD,LOAD_HIGH_BYTE);
#endif

#ifdef  OCTAPORT
    MDEFS[MBAUDLOW]  = (LOAD_INT >> 8) & 0x00ff;
    MDEFS[MBAUDHIGH] =  LOAD_INT       & 0x00ff;
    for (I = 0 ; I < MBAUDNUM ; I++) {
        outportb(MCMND,MBAUD[I]);
    }
#endif

}
/*--------------------------------------------------------------------*/
static void INITIALIZE_PORT()

    /*  Initialize the modem port. */
{
    extern void outportb();

#ifdef  OCTAPORT
    int I;
#endif

/*  begin */
    printf("\n#### Initialize Modem Port ####\n");

#ifdef  HAZITALL
    outportb(MSTAT,NUL);
    outportb(MSTAT,NUL);
    outportb(MSTAT,NUL);
    outportb(MSTAT,MCMD1);
    outportb(MSTAT,MMODE);   
    outportb(MSTAT,MCMD2);   
#endif

#ifdef  OCTAPORT
    for (I = 0 ; I < MDEFSNUM ; I++) {
        outportb(MCMND,MDEFS[I]);
    }
#endif

}
/*--------------------------------------------------------------------*/
static void INIT()

    /*  Initialize. */
{
/*  begin */
    printf("\n#### Initialize ####\n");
    INITIALIZE_PORT();
    BAUD();
}
/*--------------------------------------------------------------------*/
static void help()

    /*  Display COMMANDS. */
{
/*  begin */
    printf("\n    COMMANDS:\n");
    printf("\n        'b'aud       <baudrate>");
    printf("\n        'd'ial       telephone number");
    printf("\n        'e'xit       without hangup");
    printf("\n        'h'angup     without exit");
    printf("\n        'i'nitialize baudrate & port");
    printf("\n        'n'umber     <enter telephone number>");;
    printf("\n        'q'uit       with hangup");
    printf("\n        'r'eceive    <filename>");
    printf("\n        's'end       <filename>");
    printf("\n        't'race      transmissions");
    printf("\n");
}
/*--------------------------------------------------------------------*/
static void COMMANDS(P_CMD)

    char P_CMD;

    /*  Interpret command. */
{
/*  begin */
    switch (tolower(P_CMD)) {
        case 'b':
            printf("\nBaudrate: ");
            scanf("%d",&BAUDRATE);
            BAUD();
            break;
        case 'd':
            DIAL();
            break;
        case 'e':
            DOS();
            break;
        case 'h':
            HANGUP();
            break;
        case 'i':
            INIT();
            break;
        case 'n':
            printf("\nTelephone Number: ");
            scanf("%s",PHONENUM);
            break;
        case 'q':
            QUIT();
            break;
        case 'r':
            printf("\nReceive File: ");
            scanf("%s",RCVNAME);
            DOWNLOAD();
            break;
        case 's':
            printf("\nSend File: ");
            scanf("%s",SENDNAME);
            UPLOAD();
            break;
        case 't':
            printf("\nTrace Transmissions (y,n): ");
            scanf("%s",YN);
            if ((YN[0] = 'Y') || (YN[0] = 'y')) {
                TRACEFLAG = true;
            } else if ((YN[0] = 'N') || (YN[0] = 'n')) {
                TRACEFLAG = false;
            }
            break;
        case '?':
            help();
            break;
        default:
            printf("\nBad Command: '%c'\n",P_CMD);
    }
}
/*--------------------------------------------------------------------*/
main()
{
    metachar        RECEIVED;
    metachar        TO_SEND;
    bool            ESCFLAG;
/*  begin */
    SENDOPEN    = false;
    RCVOPEN     = false;
    T1PAUSE     = TIMECON * CLKMHZ;
    ESCFLAG     = false;
    BAUDRATE    = 1200;
    printf(
    "\n\nCMODEM--A 'Christensen Protocol' File Transfer Program\n\n");
    while (true) {
        if ((RECEIVED = IN_MODEM()) != EMPTY) {
             putchar(RECEIVED);
        } else if (TO_SEND = GETCH()) {
            if (TO_SEND == ESCCHAR ) {
                if (ESCFLAG) {
                    ESCFLAG = false;
                    OUT_MODEM(TO_SEND);
                } else {
                    ESCFLAG = true;
                }                
            } else {
                if (ESCFLAG) {
                    ESCFLAG = false;
                    COMMANDS(TO_SEND);
                } else {
                    OUT_MODEM(TO_SEND);
                }
            }
        }
    }
    exit(0);
}
/*--------------------------------------------------------------------*/
/*  end cmodem.c */
/*--------------------------------------------------------------------*/
