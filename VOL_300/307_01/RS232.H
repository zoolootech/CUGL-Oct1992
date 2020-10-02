/*
HEADER: 	;
TITLE:		COMX driver glue routine prototypes;
VERSION:	1.0;

DESCRIPTION:	"Contains function prototypes and definitions used
		to access COMX through the glue routines contained
		in RS232.C";

SYSTEM: 	MS-DOS v2 or later;
FILENAME:	RS232.H

SEE-ALSO:	RS232.C, COMX.C;
AUTHORS:	Hugh Daschbach;
COMPILERS:	Microsoft v5.0,v5.1;
*/
/*----------------------------------------------------------------------*/
/* rs232.h: Function prototypes for the "glue" routines that interface
 *   C functions with the COMX driver.
 */

/*
 * Function prototypes.
 */
extern	int  rs232_getc(int chan);
extern	int  rs232_icount(int chan);
extern	void rs232_init(int chan,    long speed, char parity,
			int stopbit, int  wordlen);
extern	int  rs232_ocount(int chan);
extern	void rs232_prot(int chan, int prot);
extern	int  rs232_putc(int chan, int c);
extern	int  rs232_read(int chan, char *s, int n);
extern	void rs232_speed(int chan, long speed);
extern	int  rs232_status(int chan);
extern	int  rs232_write(int chan, char *s, int n);

/*
 * Line status codes - rs232_getc() or rs232_stat().
 */
#define LSR_RDA  0x0100 	/* receive data available		*/
#define LSR_OR	 0x0200 	/* overrun error			*/
#define LSR_PE	 0x0400 	/* parity error 			*/
#define LSR_FE	 0x0800 	/* framing error			*/
#define LSR_BI	 0x1000 	/* break interupt			*/
#define LSR_TBA  0x2000 	/* transmit holding buffer available	*/
#define LSR_TSRE 0x4000 	/* transmit shift buffer empty		*/
#define LSR_TO	 0x8000 	/* no data available (time out) 	*/

			/* Modem status returned only from rs232_stat() */
#define MSR_DCTS 0x00001	/* delta clear to send			*/
#define MSR_DDSR 0x00002	/* delta data set ready 		*/
#define MSR_TERI 0x00004	/* trailing edge ring indicator 	*/
#define MSR_DCD  0x00008	/* carrier detect			*/
#define MSR_CTS  0x00010	/* clear to send			*/
#define MSR_DSR  0x00020	/* data set ready			*/
#define MSR_RI	 0x00040	/* ring indicator			*/
#define MSR_CD	 0x00080	/* carrier detect			*/

