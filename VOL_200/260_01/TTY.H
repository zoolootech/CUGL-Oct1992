/* @(#)tty.h 1.0  Delta Date: 00:18:04 11/24/87 */
/*
 * (c) Francois Bergeon 1987
 * All rights reserved
 */

#define CBAUD  0000740
#define B0     0000740
#define B110   0000000
#define B150   0000040
#define B300   0000100
#define B600   0000140
#define B1200  0000200
#define B2400  0000240
#define B4800  0000300
#define B9600  0000340
#define B19200 EXTA
#define EXTA   0000400
#define EXTB   0000440
#define CSIZE  0000003
#define CS7    0000002
#define CS8    0000003
#define CSTOPB 0000004
#define PARENB 0000030
#define PARODD 0000008

#define SSPEED  B1200   /* default speed: 1200 baud */

#define sleep(n)  _tick((n)*18)

#ifdef LINT_ARGS	/* argument checking enabled */
# ifndef NO_EXT_KEYS	/* extended keywords are enabled */
int cdecl tty_open(char *, int);
int cdecl tty_close(int);
int cdecl tty_write(int, char *, int);
int cdecl tty_read(int, char *, int);
#else			/* extended keywords not enabled */
int tty_open(char *, int);
int tty_close(int);
int tty_write(int, char *, int);
int tty_read(int, char *, int);
#endif	/* NO_EXT_KEYS */

#else			/* argument checking not enabled */

#ifndef NO_EXT_KEYS	/* extended keywords are enabled */
int cdecl tty_open();
int cdecl tty_close();
int cdecl tty_write();
int cdecl tty_read();
#else			/* extended keywords not enabled */
int tty_open();
int tty_close();
int tty_write();
int tty_read();
#endif	/* NO_EXT_KEYS */

#endif	/* LINT_ARGS */
