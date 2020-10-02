/* DIO.H         VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:36:29 PM */
/* 
Description:

IO redirection header from BDS C and Van Nuys Toolkit, 
by Eugene H. Mallory.

Minor modification:
	avoid CR-CR pair before LF by patch in putchar() of DIOR.c, and by 
		adding declaration of _putc_last in DIO.H;

By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/
/************************ DIO.H *****************************/
CHAR _pipeflag, _appendflag, _screenflag;
CHAR _printerflag, _dioinflag, _rdrflag;
INT _pargc;  	/* Pipe argc */
CHAR **_pargv;  	/* Pipe argv */
FILE _dibuf, _dobuf;  	/* Input and output buffers */
INT _iobyte;  	/* IOBYTE save */
CHAR _conline;  	/* Buffered input buffer */
CHAR _concount;
CHAR _conbuf[MAXLINE];
CHAR *_conbufp;
INT *_outptr;  	/* Output I/O block ptr */
CHAR _oldfile[20];
CHAR _infile[20];
#DEFINE DIOIN (_dioinflag)
#DEFINE DIOOUT (((_outptr==_dobuf)||(_outptr==STDLST))?1:0)

CHAR _putc_last;  	/*added by JAR to avoid CR-CR pair before LF*/
/********************************************************************/
