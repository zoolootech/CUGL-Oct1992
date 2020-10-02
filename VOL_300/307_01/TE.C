/*
HEADER: 	;
TITLE:		Simple terminal emulator for COMX driver;
VERSION:	1.0;

DESCRIPTION:	"Demonstration/test program for COMX device driver.";

KEYWORDS:	Serial communications, terminal emulator;
SYSTEM: 	MS-DOS v2 or later;
FILENAME:	TE.C;

SEE-ALSO:	COMX, COMX.C, RS232.C;
AUTHORS:	Hugh Daschbach;
COMPILERS:	Microsoft v5.0,v5.1;
*/
/*----------------------------------------------------------------------*/
/* te.c: Simple terminal emulator for use with COMX.SYS
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <conio.h>
#include "rs232.h"
#include "comx.h"

static int port = 0;
static int quit = 0;

static void sigint(sig)
{
    quit = 1;
}

main(int argc, char *argv[])
{
    int i;

    if (argc > 1)
	port = atoi(argv[1]);
    signal(SIGINT, sigint);
    rs232_init(port, 9600L, 'e', 1, 7);
    rs232_prot(port, PROT_XIN | PROT_XOUT);
    while (rs232_getc(port) >= 0)
	;
    while (quit == 0)
    {
	while ((i = rs232_getc(port)) >= 0)
	    putch(i);
	if (kbhit())
	    rs232_putc(port, getch() & 0x7f);
    }
    return(0);
}
