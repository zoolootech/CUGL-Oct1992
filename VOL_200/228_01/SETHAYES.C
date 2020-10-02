/*
HEADER:         CUGXXX;
TITLE:          Hayes Smartmodem initialization program;
DATE:           3-20-86;
DESCRIPTION:    Initializes Hayes Smart-modem;
KEYWORDS:       Communications;
FILENAME:       SETHAYES.C;
WARNINGS:       None;
AUTHORS:        Lynn Long;
COMPILER:       Lattice C;
REFERENCES:     US-DISK 1310;
ENDREF
*/
/***********************************************************************
 *  This is to serve as an example of how to initialize a Hayes Smart- *
 *  modem out of Lattice C.  It initialize the baud  rate divisors  at *
 *  baud, sets the parametes at 8 data bits, no parity, and 1 stop bit *
 *  You can replace the pointer *mdmstg to the command values that you *
 *  want to set the Hayes to.					       *
 *		 Program By:					       *
 *			     Lynn Long				       *
 *			     Tulsa IBBS C-SIG			       *
 *			     300/450/1200, XMODEM, 24 hours	       *
 *			     Registration Required		       *
 ***********************************************************************/
 /*    for comm port # 1.  i will make this into a function as soon as	*
  *    i have time and pass the comm port, baud rate and setup string	*
  *    to it.	 Lynn Long   */


main()
{


	unsigned byte=0x00;
	char *mdmstg =	"ATE0X1V1Q0M0ST=45S2=255S0=1\r";

	outp(0x3fb,0x80);	 /* to access baud rate divisors */
	outp(0x3f8,0x60);	 /* set baud	rate divisor lsb for 1200 bd */
	outp(0x3f9,0x00);	 /* set baud	rate divisor msb for 1200 bd */
	outp(0x3fb,0x23);	 /* set line	control reg for 8 bits, !parity, and 1 stop bit */
	outp(0x3fc,0x03);	 /* force dtr and cs	signals on */
	outp(0x3f9,0x00);	 /* disable interrupts */

	while(*mdmstg != '\0'){		
		byte = inp(0x3fd);		  /* input line status register byte */
		byte &= 0x20;		   /* look at transmit	hold status bit */
		if(byte == 0x20){
			outp(0x3f8,*mdmstg);	/* if transmit hold reg empty then */
			*mdmstg++;		/* output a byte	and bump pointer to string */
		}
	}

}

	
