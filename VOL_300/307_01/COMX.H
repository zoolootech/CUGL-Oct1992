/*
HEADER: 	;
TITLE:		PC com port driver equates;
VERSION:	1.0;

DESCRIPTION:	"Contains manifest constants used to access COMX device
		driver.";

SYSTEM: 	MS-DOS v2 or later;
FILENAME:	COMX.H;

SEE-ALSO:	COMX.C, RS232.C, TE.C;
AUTHORS:	Hugh Daschbach;
COMPILERS:	Microsoft v5.0,v5.1;
*/
/*----------------------------------------------------------------------*/
/* comx.h: MS-DOS communication driver equates
 */

#define INIT_232	0	/* initialize port			*/
#define PUTC_232	1	/* put character			*/
#define GETC_232	2	/* get character			*/
#define STAT_232	3	/* fetch status 			*/
#define WRITE_232	4	/* write string 			*/
#define READ_232	5	/* read string				*/
#define ICOUNT_232	6	/* fetch input character count		*/
#define OCOUNT_232	7	/* fetch output buffer available	*/
#define SETPROT_232	8	/* set protocol 			*/
#define SETSPEED_232	9	/* set line speed (bits/sec/100)	*/
				/* 3, 12, 24, 48, 96, 192, 384 supported */

				/* protocols are ORable bit flags	*/
#define PROT_NONE	0	/* no protocol				*/
#define PROT_XIN	1	/* transmit xoff/xon when input fills	*/
#define PROT_XOUT	2	/* obey xoff/xon when transmitting data */

#if !defined(num_entries)	/* number of entries in an array	*/
#define num_entries(x) (sizeof(x) / sizeof(x[0]))
#endif
