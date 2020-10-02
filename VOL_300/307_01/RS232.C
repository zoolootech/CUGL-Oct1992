/*
HEADER: 	;
TITLE:		INT 0x14 glue routines for COMX driver;
VERSION:	1.0;

DESCRIPTION:	"Binding routines to access COMX device driver through
		the BIOS emulation entry point";

KEYWORDS:	Serial communications;
SYSTEM: 	MS-DOS v2 or later;
FILENAME:	RS232.C;

SEE-ALSO:	COMX.C, RS232.H;
AUTHORS:	Hugh Daschbach;
COMPILERS:	Microsoft v5.0,v5.1;
*/
/*----------------------------------------------------------------------*/
/* rs232.c: int 14h (RS232) glue routines for COMX driver
 */
#include <stdio.h>
#include <ctype.h>
#include <dos.h>
#include "rs232.h"
#include "comx.h"

static int speed_tbl[] = {
    11, 15, 30, 60, 120, 240, 480, 960, 1920, 3840
};
/*----------------------------------------------------------------------*/
/*		     Low level RS232 glue routines			*/
/*----------------------------------------------------------------------*/

/*
 * Initialize channel.
 */
void rs232_init(int chan, long speed, char parity, int stopbit, int wordlen)
{
    union REGS	ir, or;
    int   sti;			    /* speed table index		*/

    ir.h.al = (char) ((stopbit > 1 ? 4 : 0 ) + (wordlen == 8 ? 3 : 2));
    if ((parity = tolower(parity)) == 'e')
	ir.h.al += 0x18;
    else if (parity == 'o')
	ir.h.al += 0x08;
    speed /= 10;
    for (sti = 0; sti < num_entries(speed_tbl); sti++)
	if ((int) speed == speed_tbl[sti])
	    break;
    ir.h.al += (char) (sti << 5);
    ir.h.ah = INIT_232;
    ir.x.dx = chan;
    int86(0x14, &ir, &or);
    if (sti > 8)
    {
	ir.h.ah = SETSPEED_232;
	ir.x.cx = sti;
	int86(0x14, &ir, &or);
    }
}

/*
 * Put a single character.  Return value less than zero indicates character
 * not tramitted because buffer was full.
 */
int  rs232_putc(int chan, int c)
{
    union REGS	ir, or;

    ir.h.ah = PUTC_232;
    ir.h.al = (char) c;
    ir.x.dx = chan;
    int86(0x14, &ir, &or);
    return(or.x.ax);
}

/*
 * Get a single character.  Return character in low byte, line status register
 * in high byte.  Returned value less than zero indicates no data available.
 */
int  rs232_getc(chan)
{
    union REGS	ir, or;

    ir.h.ah = GETC_232;
    ir.x.dx = chan;
    int86(0x14, &ir, &or);
    return(or.x.ax);
}

/*
 * Get status.	The first call after rs232_read() indicates an error will
 * return the status at the time of the error, otherwise it returns line
 * status in high byte, and modem status in low byte.
 */
int  rs232_status(int chan)
{
    union REGS	ir, or;

    ir.h.ah = STAT_232;
    ir.x.dx = chan;
    int86(0x14, &ir, &or);
    return(or.x.ax);
}

/*
 * Write string.  Returns byte transfer count with bit 15 set if output
 * buffer full.
 */
rs232_write(int chan, char *s, int n)
{
    union  REGS  ir, or;
    struct SREGS sr;
    char   far	*cfp;

    cfp = (char far *) s;
    segread(&sr);
    sr.es   = FP_SEG(cfp);
    ir.h.ah = WRITE_232;
    ir.x.bx = FP_OFF(cfp);
    ir.x.cx = n;
    ir.x.dx = chan;
    int86(0x14, &ir, &or);
    return(or.x.ax);
}

/*
 * Read string.  Returns count of bytes transfered without error.  If bit 15
 * is set an error was encountered (overrun, parity error, framing error,
 * or break) during reception.	rs232_status() will return the line status
 * register at the time of the error.
 */
rs232_read(int chan, char *s, int n)
{
    union  REGS  ir, or;
    struct SREGS sr;
    char   far	*cfp;

    cfp = (char far *) s;
    segread(&sr);
    sr.es   = FP_SEG(cfp);
    ir.h.ah = READ_232;
    ir.x.bx = FP_OFF(cfp);
    ir.x.cx = n;
    ir.x.dx = chan;
    int86(0x14, &ir, &or);
    return(or.x.ax);
}

/*
 * Get character ready count.  Returns byte count of data in receive buffer.
 */
rs232_icount(int chan)
{
    union REGS	ir, or;

    ir.h.ah = ICOUNT_232;
    ir.x.dx = chan;
    int86(0x14, &ir, &or);
    return(or.x.ax);
}

/*
 * Get output buffer available count.
 */
rs232_ocount(int chan)
{
    union REGS	ir, or;

    ir.h.ah = OCOUNT_232;
    ir.x.dx = chan;
    int86(0x14, &ir, &or);
    return(or.x.ax);
}

/*
 * Set channel protocol.  Enables XON/XOFF processing for either input or
 * output.
 */
void rs232_prot(int chan, int prot)
{
    union REGS ir, or;

    ir.h.ah = SETPROT_232;
    ir.h.al = (char) prot;
    ir.x.dx = chan;
    int86(0x14, &ir, &or);
}

/*
 * Change line speed.
 */
void rs232_speed(int chan, long speed)
{
    union REGS ir, or;
    int   sti;

    speed /= 10;
    for (sti = 0; sti < num_entries(speed_tbl); sti++)
	if ((int) speed == speed_tbl[sti])
	    break;
    ir.h.ah = SETSPEED_232;
    ir.x.cx = (char) sti;
    ir.x.dx = chan;
    int86(0x14, &ir, &or);
    if (or.h.ah & 0x80)
	fprintf(stderr, "rs232_speed(): Invalid line speed - %d\n", speed);
}

/*----------------------------------------------------------------------*/
/*	     Test program for glue routines and comx driver		*/
/*----------------------------------------------------------------------*/

#if 0

#include <conio.h>

static int chan = 0;

static void wait_putc(chan, c)
{
    while (rs232_putc(chan, c) < 0)
	;
}

main(int argc, char *argv[])
{
    int     i, j, n;
    char    *cp, buf[128];

    if (argc > 1)
	chan = atoi(argv[1]);
    rs232_init(chan, 9600, 'n', 1, 8);
    rs232_prot(chan, PROT_XOUT);
#if 0
    rs232_speed(chan, 9600);
#endif
#if 1				/* read test				*/
    do
    {
	while (!kbhit())
	    ;
	if ((n = rs232_icount(chan)) <= 0)
	    printf("%d characters available ???\n", n);
	else
	{
	    if ((i = rs232_read(chan, buf, n)) != n)
		printf("read %d, expected %d\n", i, n);
	    write(1, buf, i);
	}
    } while (getch() != 'q');
#endif
#if 0				/* write test				*/
    for (i = 32; i < 128; i++)
    {
	cp = buf;
	for (j = i; j < i + 80; j++)
	    *cp++ = j < 128 ? j : j - 128 + 32;
	*cp++ = '\r';
	*cp++ = '\n';
	printf("%4d ", rs232_ocount(chan));
	for (j = 0; j < cp - buf; j += n & 0x7fff)
	    n = rs232_write(chan, buf + j, cp - buf - j);
    }
#endif
#if 0				/* put/get test 			*/
    for (i = 32; i < 128; i++)
    {
	for (j = i; j < i + 80; j++)
	    wait_putc(chan, j < 128 ? j : j - 128 + 32);
	wait_putc(chan, '\r');
	wait_putc(chan, '\n');
    }
    do
    {
	while ((i = rs232_getc(chan)) < 0)
	    ;
	putchar(i);
    }
    while ((i & 0xff) != 'q');
#endif
}
#endif
