/*
HEADER: 	;
TITLE:		PC com port driver;
VERSION:	1.0;

DESCRIPTION:	"MS-DOS serial port device driver.  Provides buffered
		input and output to serial port with optional XON/XOFF
		flow control through standard read/write requests or
		interrupt 0x14.

		Mixed memory model used.  COMX.C compiled as small
		model with explicitly declared far pointers.  Front
		end program coerces the linkage editor to produce a
		tiny model executable.";

WARNINGS:	"Microsoft specific.  Startup code contained in
		COMXFE.ASM does not initialize uninitialized static
		variables to zero.";
KEYWORDS:	Serial communications, device driver;
SYSTEM: 	MS-DOS v2 or later;
FILENAME:	COMX.C;

SEE-ALSO:	COMX, COMXFE.ASM, COMXBE.C;
AUTHORS:	Hugh Daschbach;
COMPILERS:	Microsoft v5.0,v5.1;
*/
/*----------------------------------------------------------------------*/
/* comx.c: MS-DOS driver for multiple communication devices (com1 - com4).
 */
#include "comx.h"

/* Instalation:
 * This program is installed as an MS-DOS device driver by including
 * the following entry in the CONFIG.SYS file:
 *	    DEVICE = COMX.SYS xxx [xxx [xxx [xxx]]]
 * where xxx is a com port address in hexadecimal (e.g. 3fa).  At least
 * one port is required.  The driver will support as many as four.
 *
 * The communication devices are assumed to interrupt on either IRQ3 or
 * IRQ4.  More than one device can share the same interrupt line as
 * each defined port is polled on every interrupt.  Other versions of
 * this driver support dedicated interrupt vectors, eliminating the
 * polling operation at interrupt time.  However, even with the polling
 * overhead, the driver has been tested successfully at 9600 baud on a
 * 4.77 Mhz 8088.
 */

/*----------------------------------------------------------------------*/
/*			  System Declarations				*/
/*----------------------------------------------------------------------*/
#pragma pack(1)
int inp(unsigned int);
int outp(unsigned int, int);
void _enable(void);
void _disable(void);
#pragma intrinsic(inp, outp, _enable, _disable)

/*----------------------------------------------------------------------*/
/*			Local Data Declarations 			*/
/*----------------------------------------------------------------------*/
typedef unsigned char	uchar;
typedef unsigned short	ushort;

/*----------------------------------------------------------------------*/
/*		     External Function Prototypes.			*/
/*----------------------------------------------------------------------*/
extern void int10(unsigned ax, unsigned bx);

/*----------------------------------------------------------------------*/
/*		       Local function prototypes			*/
/*----------------------------------------------------------------------*/
static	void check_block(struct comport *chan);
static	int  com_int(void);
static	char *define_lines(char far *cp);
extern	void driver(struct rb far *rbp, int line_no);
static	int  eol(int c);
static	void get_char(struct comport *chan);
static	void hookvect(unsigned int intnum, struct isr_block *isr,
		      void (cdecl *handler)());
static	int  int14(ushort ax, ushort bx, ushort cx,
		   ushort dx, ushort ds, ushort es);
static	int  isspace(int c);
static	int  nop(int i);
static	void put_char(struct comport *chan);
static	void putc(int c);
static	void puts(char *s);
static	void putx(int i);
static	void setspeed(struct comport *chan, int sti);

/*----------------------------------------------------------------------*/
/*		     Driver interface declarations			*/
/*----------------------------------------------------------------------*/
enum opcodes {			/* Driver operation codes:		*/
    INIT = 0,			/* initialization			*/
    MEDIA_CHECK,		/* media check				*/
    BUILD_BPB,			/* build BIOS Parameter Block		*/
    INPUT_IOCTL,		/* input control string 		*/
    INPUT,			/* read 				*/
    NON_DEST_INPUT,		/* non destructive input		*/
    INPUT_STATUS,		/* input available status		*/
    INPUT_FLUSH,		/* flush input buffer			*/
    OUTPUT,			/* write				*/
    OUTPUT_VERIFY,		/* write with verify			*/
    OUTPUT_STATUS,		/* output busy status			*/
    OUTPUT_FLUSH,		/* flush output buffer			*/
    OUTPUT_IOCTL,		/* output control string		*/
    DEVICE_OPEN,		/* device open (removable media)	*/
    DEVICE_CLOSE,		/* device close (removable media)	*/
    REMOVEABLE_MEDIA,		/* more removable media 		*/
    GENERIC_IOCTL = 19, 	/* more ioctl				*/
    GET_LOGICAL_DEVICE = 23,	/* get logical device			*/
    SET_LOGICAL_DEVICE = 24	/* set logical device			*/
};

				/* Driver status indicators:		*/
#define DONE	0x0100		/* I/O done				*/
#define BUSY	0x0200		/* device busy				*/
#define ERROR	0x8000		/* I/O error				*/

enum errcodes { 		/* Driver error codes:			*/
    WRITE_PROTECT,		/* write protect violation		*/
    UNKNOWN_UNIT,		/* unknown unit 			*/
    NOT_READY,			/* device non ready			*/
    UNKNOWN_COMMAND,		/* unknown command			*/
    CRC_ERROR,			/* CRC error				*/
    BAD_STRUCT_LENGTH,		/* bad drive request structure length	*/
    SEEK_ERROR, 		/* seek error				*/
    UNKNOWN_MEDIA,		/* unknown media			*/
    SECTOR_NOT_FOUND,		/* sector not found			*/
    PAPER_OUT,			/* printer out of paper 		*/
    WRITE_FAULT,		/* write fault				*/
    READ_FAULT, 		/* read fault				*/
    GENERAL_FAILURE,		/* general failure			*/
    RESV_D,			/* reserved				*/
    RESV_E,			/* reserved				*/
    INVALID_DISK_CHANGE 	/* invalid disk change			*/
};

typedef struct rb {		/* I/O Request block			*/
    uchar  len; 		/* request block length 		*/
    uchar  unit;		/* unit code				*/
    uchar  op_code;		/* operation code			*/
    int    status;		/* status code				*/
    char   resv[8];		/* reserved				*/
    union {
	struct {
	    uchar    count;	/* unit count				*/
	    char far *end;	/* end of resident module		*/
	    char far *bpbp;	/* BPB array pointer			*/
	    uchar    drive;	/* drive number 			*/
	} init;
	struct {
	    uchar    media;	/* media descriptor byte		*/
	    char far *buf;	/* transfer buffer pointer		*/
	    int      count;	/* byte/sector count			*/
	    int      sector;	/* starting sector number		*/
	    char far *vol_id;	/* volume idenitifier pointer		*/
	} inout;
	char	ndi;		/* non destructive input data byte	*/
    } os;			/* operation specific			*/
} rb;

/*----------------------------------------------------------------------*/
/*	    Interrupt service routine interface declaration		*/
/*----------------------------------------------------------------------*/
typedef struct isr_block {	/* Interrupt Service Routine		*/
    uchar   push_es;
    uchar   push_ds;
    uchar   push_dx;
    uchar   push_cx;
    uchar   push_bx;
    uchar   push_ax;
    ushort  mov_ax_cs;
    ushort  mov_ds_ax;
    uchar   call;
    ushort  target;
    ushort  or_ax_ax;
    uchar   pop_ax;
    uchar   pop_bx;
    uchar   pop_cx;
    uchar   pop_dx;
    uchar   pop_ds;
    uchar   pop_es;
    ushort  jz_ip01;
    uchar   cs_prefix;
    ushort  jmp;
    void    (far **fnpp)(void);
    uchar   iret;
    void    (far *fnp)(void);
} isr_block;

/*----------------------------------------------------------------------*/
/*		    Hardware interface declarations			*/
/*----------------------------------------------------------------------*/
				/* Line Status Register definition	*/
#define LSR_RDA     1		/* receive data available		*/
#define LSR_OR	    2		/* overrun error			*/
#define LSR_PE	    4		/* parity error 			*/
#define LSR_FE	    8		/* framing error			*/
#define LSR_BI	 0x10		/* break interupt			*/
#define LSR_TBA  0x20		/* transmit holding buffer available	*/
#define LSR_TSRE 0x40		/* transmit shift buffer empty		*/

				/* Modem Control Register definitions	*/
#define MCR_DTR     1		/* data terminal ready			*/
#define MCR_RTS     2		/* request to send			*/
#define MCR_OUT1    4		/* reset Hayes Smartmodem		*/
#define MCR_OUT2    8		/* enable Hayes Smartmodem interrupts	*/
#define MCR_LOOP 0x10		/* loop back test enable		*/

				/* Interrupt Identification Register	*/
#define IIR_PEND 1		/* interrupt pending			*/
#define IIR_MASK 6		/* select following values:		*/
#define IIR_RLS  6		/* receiver line status (error) 	*/
#define IIR_RDA  4		/* receive data available		*/
#define IIR_TBA  2		/* transmit buffer available		*/
#define IIR_MS	 0		/* modem status 			*/

				/* Interrupt Enable Register definition */
#define IER_RDA 1		/* receive data available interrupt	*/
#define IER_TBA 2		/* transmit buffer available interrupt	*/
#define IER_RLS 4		/* receive line status interrupt	*/
#define IER_MS	8		/* modem status interrupt		*/

				/* Line Control Register definition	*/
#define LCR_WLS0    1		/* word length select bit 0		*/
#define LCR_WLS1    2		/* word length select bit 1		*/
#define LCR_STB     4		/* number of stop bits			*/
#define LCR_PEN     8		/* parity enable			*/
#define LCR_EPS    16		/* even parity select			*/
#define LCR_STICK  32		/* stick parity 			*/
#define LCR_BREAK  64		/* transmit break			*/
#define LCR_DLAB  128		/* divisor latch access bit		*/

				/* Modem Status Register definition	*/
#define MSR_DCTS 0x00001	/* delta clear to send			*/
#define MSR_DDSR 0x00002	/* delta data set ready 		*/
#define MSR_TERI 0x00004	/* trailing edge ring indicator 	*/
#define MSR_DCD  0x00008	/* carrier detect			*/
#define MSR_CTS  0x00010	/* clear to send			*/
#define MSR_DSR  0x00020	/* data set ready			*/
#define MSR_RI	 0x00040	/* ring indicator			*/
#define MSR_CD	 0x00080	/* carrier detect			*/

typedef enum comregs {		/* INS8250 registers addresses		*/
    rx = 0,			/* receive data register		*/
    tx = 0,			/* transmit data register		*/
    ie, 			/* interrupt enable register		*/
    ii, 			/* interupt identification register	*/
    lc, 			/* line control register		*/
    mc, 			/* modem control register		*/
    ls, 			/* line status register 		*/
    ms, 			/* modem status register		*/
    dl_lsb = 0, 		/* divisor latch least significant byte */
    dl_msb			/* divisor latch most  significant byte */
} comregs;

#define PIC	0x20		/* 8259A Programmable Interrupt Controller */
#define EOI	0x20		/* End of Interrupt command to PIC	*/

/*----------------------------------------------------------------------*/
/*			 Local Data Definitions 			*/
/*----------------------------------------------------------------------*/
#define OBUFSZ	    128 	    /* output character fifo size	*/
				    /* next output buffer position	*/
#define next_out(i) ((i + 1) & (OBUFSZ - 1))
				    /* pervious output buffer position	*/
#define prev_out(i) ((i - 1) & (OBUFSZ - 1))
#define IBUFSZ	    128 	    /* input character fifo size	*/
				    /* next input buffer position	*/
#define next_in(i) ((i + 1) & (IBUFSZ - 1))
				    /* pervious input buffer position	*/
#define prev_in(i) ((i - 1) & (IBUFSZ - 1))
#define THOLD_OFF   (IBUFSZ/2)	    /* input char count b4 sending XOFF */
#define THOLD_ON    (THOLD_OFF/2)   /* input char count b4 sending XON	*/
#define XON	    0x11	    /* CTRL/Q				*/
#define XOFF	    0x13	    /* CTRL/S				*/

typedef struct comport {
    unsigned port;		/* base port address			*/
    uchar    *out_buf;		/* output buffer pointer		*/
    unsigned *in_buf;		/* input buffer pointer 		*/
    uchar    flow;		/* flow control enabled 		*/
    uchar    in_block;		/* we've transmitted X-OFF		*/
    uchar    out_block; 	/* we've received X-OFF 		*/
    int      out_head;		/* index to next empty buffer position	*/
    int      out_tail;		/* index to last printed character	*/
    int      in_head;		/* index to next empty buffer position	*/
    int      in_tail;		/* index to last returned character	*/
} comport;
comport comx[4] = { { 0 }, { 0 }, { 0 }, { 0 } };

static int port_count;		/* defined port count			*/

static unsigned speed_tbl[] = { /* baud rate generator divisor latch	*/
    1047, 768, 384, 192, 96, 48, 24, 12, 6, 3
};

isr_block isr0c;		/* interrupt 0x0c service routine	*/
isr_block isr0b;		/* interrupt 0x0b service routine	*/
isr_block isr14;		/* interrupt 0x14 service routine	*/

#ifdef DEBUG
/*----------------------------------------------------------------------*/
/*		       Diagnostic output routines			*/
/*----------------------------------------------------------------------*/
#define  putchar(c)  int10((14 << 8) + c, 0)
#define  printnl(s) { puts(#s " = "); putx(s); puts("\n\r"); }
#define  print(s) { puts(#s " = "); putx(s); puts("  "); }
#define  dumpc(c) { puts(#c " = "); putc(c); puts("  "); }
char *to_print = "0123456789abcdef";

static void putc(int c)
{
    putchar(to_print[c >> 4 & 0xf]);
    putchar(to_print[c & 0xf]);
}

static void puts(char *s)
{
    for (; *s; s++)
	putchar(*s);
}

static void putx(int  i)
{
    putchar(to_print[i >> 12 & 0xf]);
    putchar(to_print[i >>  8 & 0xf]);
    putchar(to_print[i >>  4 & 0xf]);
    putchar(to_print[i	     & 0xf]);
}
#endif

/*----------------------------------------------------------------------*/
/*			Common service routines 			*/
/*----------------------------------------------------------------------*/
static int  nop(i)		/* flush instruction prefetch queue	*/
{				/* for timing btwn inp & outp		*/
    return(i);
}

static void setspeed(register comport *chan, int sti)
{
    int  lcr;
    lcr = inp(chan->port + lc); 			(void) nop(0);
    outp(chan->port + lc, lcr + LCR_DLAB);		(void) nop(0);
    outp(chan->port + dl_msb, speed_tbl[sti] >> 8);	(void) nop(0);
    outp(chan->port + dl_lsb, speed_tbl[sti] & 0xff);	(void) nop(0);
    outp(chan->port + lc, lcr);
}

/*----------------------------------------------------------------------*/
/*		       Interrupt service routines			*/
/*----------------------------------------------------------------------*/
static int  com_int()
{
    register comport *chan;	/* current comx[] element pointer	*/
    int  int_id;
    char active;

    _enable();			/* reenable interrupts			*/
    do {
	active = 0;
	for (chan = comx; chan->port; chan++)
	{
	    while (((int_id = inp(chan->port + ii)) & IIR_PEND) == 0)
	    {
		active = 1;
		switch (int_id & IIR_MASK)
		{
		case IIR_RDA:			/* rx data avalable	*/
		    get_char(chan);
		    break;
		case IIR_RLS:			/* receiver line status */
		    chan->in_buf[chan->in_head] = inp(chan->port + ls) << 8;
		    chan->in_head = next_in(chan->in_head);
		    break;
		case IIR_TBA:			/* tx buffer available	*/
		    put_char(chan);
		    break;
		case IIR_MS:			/* Modem status change	*/
		    inp(chan->port + ms);
		    break;
		}
	    }
	}
    } while (active);
    outp(PIC, EOI);		/* reenable PIC interrupts		*/
    return(0);			/* skip the BIOS routines on exit	*/
}

/*----------------------------------------------------------------------*/
static void get_char(chan)
register comport *chan; 	/* current comx[] element pointer	*/
{
    unsigned c;
    int   line_stat;
    int   count;

    if ((line_stat = inp(chan->port + ls)) & LSR_RDA)
    {
	c = (unsigned) inp(chan->port + rx);
	if ((chan->flow & PROT_XOUT) != 0 && c == XOFF)
	    chan->out_block = 1;
	else if ((chan->flow & PROT_XOUT) != 0 && c == XON)
	{
	    chan->out_block = 0;
	    put_char(chan);
	}
	else
	{
	    if (next_in(chan->in_head) == chan->in_tail)
		chan->in_buf[chan->in_head] |= LSR_OR << 8;
	    else
	    {
		chan->in_buf[chan->in_head] = c | ((unsigned) line_stat << 8);
		chan->in_head = next_in(chan->in_head);
	    }
	    if ((chan->flow & PROT_XIN) != 0)
	    {
		if ((count = chan->in_head - chan->in_tail) < 0)
		    count += IBUFSZ;
		if (count > THOLD_OFF && !chan->in_block)
		{
		    chan->out_tail = prev_out(chan->out_tail);
		    chan->out_buf[chan->out_tail] = XOFF;
		    put_char(chan);
		    chan->in_block = 1;
		}
	    } /* flow control enabled */
	} /* received XON or XOFF */
    } /* receive data available */
}

/*----------------------------------------------------------------------*/
static void put_char(chan)
register comport *chan; 	/* current comx[] element pointer	*/
{
    if (inp(chan->port + ls) & LSR_TBA &&
	!chan->out_block &&
	chan->out_head != chan->out_tail)
    {
	outp(chan->port + tx, chan->out_buf[chan->out_tail]);
	chan->out_tail = next_out(chan->out_tail);
    }
}

/*----------------------------------------------------------------------*/
static void check_block(chan)	/* should we transmit XON ?		*/
register comport *chan;
{
    int count;

    if (chan->in_block)
    {
	if ((count = chan->in_head - chan->in_tail) < 0)
	    count += IBUFSZ;
	if (count < THOLD_ON)
	{
	    chan->out_tail = prev_out(chan->out_tail);
	    chan->out_buf[chan->out_tail] = XON;
	    put_char(chan);
	    chan->in_block = 0;
	}
    }
}

/*----------------------------------------------------------------------*/
/*			     Initialization				*/
/*----------------------------------------------------------------------*/
static int eol(c)     { return(c == '\r' || c == '\n'); }
static int isspace(c) { return(c == ' '  || c == '\t'); }

static char *define_lines(char far *cp)
{
    extern char near bss_end;	/* defined in comxbe.c			*/
    register comport *chan;	/* current comx[] element pointer	*/
    char   *bp; 		/* buffer pointer			*/
    int    port;		/* port address 			*/

    bp = &bss_end;		/* initial buffer pointer		*/
    chan = comx;
    while (!eol(*cp) && isspace(*cp))
	cp++;
    while (!eol(*cp) && !isspace(*cp))
	cp++;
    while (!eol(*cp) && isspace(*cp))
	cp++;
    while (!eol(*cp))
    {
	port = 0;
	while (!eol(*cp) && !isspace(*cp))
	{
	    port = (port << 4) +
		((*cp >= '0' && *cp <= '9') ? *cp - '0' :
		 (*cp >= 'a' && *cp <= 'f') ? *cp - 'a' + 10 :
		 (*cp >= 'A' && *cp <= 'F') ? *cp - 'A' + 10 : 0);
	    cp++;
	}
	chan->port = port;
	outp(chan->port + mc, MCR_DTR | MCR_RTS | MCR_OUT2);
	chan->in_buf = (unsigned *) bp;
	bp += IBUFSZ * sizeof(unsigned);
	chan->out_buf = (uchar *) bp;
	bp += OBUFSZ * sizeof(uchar);
	chan++;
	while (!eol(*cp) && isspace(*cp))
	    cp++;
    }
    port_count = chan - comx;
    return(bp);
}

/*----------------------------------------------------------------------*/
static void hookvect(unsigned	intnum,
		     isr_block	*isr,
		     void	(*handler)())
{
    void (far *(far *vect_ad))();
    extern isr_block isr_prototype;

    *isr = isr_prototype;
    isr->target = (char *) handler - (char *) &isr->call - 3;
    vect_ad = (void (far *(far *))()) ((long) (intnum * 4));
    isr->fnp  = *vect_ad;
    isr->fnpp = &isr->fnp;
    *vect_ad = (void (far *)()) isr;
}

/*----------------------------------------------------------------------*/
/*			       DOS driver				*/
/*----------------------------------------------------------------------*/
void driver(rbp, line_no)
rb far *rbp;
int    line_no;
{
    comregs  *(far *chan_ptr) = (comregs *(far *)) 0x00400006l;
    register comport  *chan;	/* current comx[] element pointer	*/
    static   char *endp;	/* end of driver pointer		*/
    int      i;

    chan = comx + line_no;
    switch (rbp->op_code)
    {
    case INIT:
	if (line_no == 0)
	{
	    endp = define_lines(rbp->os.init.bpbp);
	    hookvect(0x0c, &isr0c, com_int);
	    hookvect(0x0b, &isr0b, com_int);
	    hookvect(0x14, &isr14, int14);
	}
	rbp->os.init.end = (char far *) endp;
	break;
    case INPUT_IOCTL:
	if ((i = chan->in_head - chan->in_tail) < 0)
	    i += IBUFSZ;
	if (rbp->os.inout.count == 2)
	    *((int far *) rbp->os.inout.buf) = i;
	else
	    rbp->status |= ERROR | READ_FAULT;
	break;
    case INPUT:
	while (chan->in_head == chan->in_tail)
	    ;
	for (i = 0;
	     i < rbp->os.inout.count && chan->in_head != chan->in_tail;
	     i++)
	{
	    if (chan->in_buf[chan->in_tail] &
		((LSR_OR | LSR_PE | LSR_FE | LSR_BI) << 8))
	    {
		rbp->status |= ERROR | READ_FAULT;
		break;
	    }
	    rbp->os.inout.buf[i] = (char) chan->in_buf[chan->in_tail];
	    chan->in_tail = next_in(chan->in_tail);
	}
	rbp->os.inout.count = i;
	check_block(chan);
	break;
    case NON_DEST_INPUT:
	if (chan->in_head == chan->in_tail)
	    rbp->status |= BUSY;
	else
	    rbp->os.ndi = (char) chan->in_buf[chan->in_tail];
	break;
    case INPUT_STATUS:
	rbp->status |= (chan->in_head != chan->in_tail) ? 0 : BUSY;
	break;
    case INPUT_FLUSH:
	chan->in_tail = chan->in_head;
	break;
    case OUTPUT:
    case OUTPUT_VERIFY:
	for (i = 0; i < rbp->os.inout.count; i++)
	{
	    if (next_out(chan->out_head) == chan->out_tail)
	    {
		rbp->status |= BUSY;
		break;
	    }
	    chan->out_buf[chan->out_head] = rbp->os.inout.buf[i];
	    chan->out_head = next_out(chan->out_head);
	}
	rbp->os.inout.count = i;
	break;
    case OUTPUT_STATUS:
	rbp->status |= (chan->out_head == chan->out_tail) ? 0 : BUSY;
	break;
    case OUTPUT_FLUSH:
	chan->out_tail = chan->out_head;
	break;
    default:
	rbp->status |= ERROR | UNKNOWN_COMMAND;
    }
				/* it seems wasteful to reset the interrupt
				 * masks on every I/O operation.  But
				 * usurping communication programs do not
				 * always reset the hardware regs correctly
				 * upon termination.  we pay the price here.
				 */
    outp(PIC + 1, inp(PIC + 1) & 0xe7);
				/* (re)loading the UART interrupt enable
				 * register pops a tx bufffer available
				 * interrupt (if it actually is).  This
				 * initiates output data transfer after
				 * data is written to the output buffer.
				 */
    outp(chan->port + ie, IER_RDA | IER_TBA | IER_RLS);
    rbp->status |= DONE;
}

/*----------------------------------------------------------------------*/
/*			 BIOS int 14h interface 			*/
/*----------------------------------------------------------------------*/
static int int14(ax, bx, cx, dx, ds, es)
ushort ax, bx, cx, dx, ds, es;
{
    static   unsigned status;
    register comport  *chan;	 /* current comx[] element pointer	*/
    char far *cfp;

    _enable();
    if (dx > port_count)
	return(0);
    chan = comx + dx;
    switch (ax >> 8)
    {
    case INIT_232:		/* initialize port			*/
	outp(PIC + 1, inp(PIC + 1) & 0xe7);
	outp(chan->port + mc, 0);
	setspeed(chan, (ax >> 5) & 7);
	outp(chan->port + lc, ax & 0x1f);
	outp(chan->port + mc, MCR_DTR | MCR_RTS | MCR_OUT2);
	break;
    case PUTC_232:		/* put character			*/
	if (next_out(chan->out_head) == chan->out_tail)
	{
	    ax = 0x8000;
	    break;
	}
	chan->out_buf[chan->out_head] = (char) (ax & 0xff);
	chan->out_head = next_out(chan->out_head);
	outp(chan->port + ie, IER_RDA | IER_TBA | IER_RLS);
	break;
    case GETC_232:		/* get character			*/
	if (chan->in_head == chan->in_tail)
	{
	    ax = 0x8000;
	    break;
	}
	ax = chan->in_buf[chan->in_tail];
	chan->in_tail = next_in(chan->in_tail);
	check_block(chan);
	break;
    case STAT_232:		/* fetch status.  Use status saved by	*/
				/* string read if any, or current	*/
				/* hardware if not.			*/
	if (status == 0)
	    ax = (inp(chan->port + ls) << 8) + inp(chan->port + ms);
	else
	{
	    ax = status;
	    status = 0;
	}
	break;
    case WRITE_232:		/* write string 			*/
	cfp = (char far *) ((unsigned long) es << 16) + bx;
	for (ax = 0; ax < cx; ax++)
	{
	    if (next_out(chan->out_head) == chan->out_tail)
	    {
		ax |= 0x8000;
		break;
	    }
	    chan->out_buf[chan->out_head] = *cfp++;
	    chan->out_head = next_out(chan->out_head);
	}
	outp(chan->port + ie, IER_RDA | IER_TBA | IER_RLS);
	break;
    case READ_232:		/* read string				*/
	cfp = (char far *) ((unsigned long) es << 16) + bx;
	for (ax = 0; ax < cx; ax++)
	{
	    if (chan->in_buf[chan->in_tail] &
		((LSR_OR | LSR_PE | LSR_FE | LSR_BI) << 8))
	    {
		ax |= 0x8000;	/* signal error, save status for	*/
				/* subsequent inquiry			*/
		status = chan->in_buf[chan->in_tail] & ~0xff +
			 inp(chan->port + ms);
		chan->in_tail = next_in(chan->in_tail);
		break;
	    }
	    *cfp++ = (char) chan->in_buf[chan->in_tail];
	    chan->in_tail = next_in(chan->in_tail);
	}
	check_block(chan);
	break;
    case ICOUNT_232:		/* fetch input character count		*/
	if ((ax = chan->in_head - chan->in_tail) < 0)
	    ax += IBUFSZ;
	break;
    case OCOUNT_232:		/* fetch output buffer available	*/
	ax = (chan->out_head >= chan->out_tail) ?
	      OBUFSZ - (chan->out_head - chan->out_tail) :
	      chan->out_tail - chan->out_head;
	break;
    case SETPROT_232:		/* set protocol 			*/
	chan->flow = (uchar) ax;
	break;
    case SETSPEED_232:		/* set line speed			*/
	if (cx > num_entries(speed_tbl))
	    ax = 0x8000;	/* report invalid speed 		*/
	else
	    setspeed(chan, cx);
	break;
    }
    return(0);
}
