/*
$TITLE ('yamsys.h: lattice c version')
$DATE ( 7 nov 85)
*/
/*
 * global equates for specific installation and modem ports
 * Other modem specific stuff is in yam5.c
 *
 */

#define BIGYAM		/* include special expanded functions */
#define IBMPC

	/* external files */
#define YAMCONFIG "YAMCFG.T"
#define HELPFILE "YAMHELP.T"
#define PHONES "YAMPHONE.T"

	/* chosen so that sleep(n) will pause n 1/10th sec */
#define LOOPS 4990	/* timing value in sleep; compiler & cpu dependent */
#define CLKMHZ 5

	/* allow system commands */
#define SYSFUNC
	/* allowable pathname separators for MS-DOS */
#define PATHSEP "\\/:"

	/* test for squeezed files in type */
/*#define USQ*/

#define TERMINIT	"\0"

/* Default Modem I/O ports */
#define DPORT 0x3f8

/* modem I/O */
#define CDO FALSE		/* dont bother with carrier detect */
#define MIREADY mirdy()		 /* value != 0 if char available */
#define MICHAR michr()	 /* get char assuming miready */
#define MOCHAR(x) mochr(x)
#define MOREADY (inp(Dport+5) & 0x20) /* != 0 if modem ready to load next char */

/* Console I/O for MS-DOS  */
#define CIREADY cnsrdy()		/* non zero if char typed */
#define CICHAR cnsin()			/* get char from keyboard */
#define COREADY TRUE			/* console out is always ready */
#define CONOUT(x) cochar(x)	/* console output-bypass some of dos strangness */

#define LPOUT(x) bdos(5,x)
#define LPREADY prtrdy()

/* terminal characteristics */
#define TWIDTH	80		/* # of columns	*/
#define TLENGTH	24		/* # of lines */
