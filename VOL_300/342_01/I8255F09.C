/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255F08.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   Toggle Output Bit Function
 *  ----------------------------------------------------------------------
 */

#define  I8255F08_C_DEFINED  1
#include "I8255FN.H"
#undef   I8255F08_C_DEFINED

void I8255_bittog (I8255DAT *data, int bit);

/*- I8255 : Toggle Output Bit ----------------**
 *  Toggle one of the output bits in the 8255.
 *  The bit state is toggled.
 *  The bit number should be from 1 - 24 as follows:
 *   1 = Port A Bit 0;   8 = Port A Bit 7
 *   9 = Port B Bit 0;  16 = Port B Bit 7
 *  17 = Port C Bit 0;  24 = Port C Bit 7
 *  Note:
 *  0 XOR 0 = 0; 1 XOR 0 = 1 => Do not affect other bits.
 *  0 XOR 1 = 1; 1 XOR 1 = 0 => Affect toggle bit.
 *  Passed:
 *      pointer :   I8255DAT
 *      integer :   bit number
 *  Returns:
 *      nothing
 *      Loads stat with appropriate error code
 */
void I8255_bittog (I8255DAT *data, int bit)
	{
	int             port;   /* port number  */
	int             padd;   /* port address */
	unsigned char   mask;   /* byte mask    */
	unsigned char   oval;   /* out value    */

	/*  Make sure the bit requested is valid.
	 *  Three ports of 8 bits each = 24 bits.
	 */
	if ( (bit < 1) || (bit > 24) ){
		data->stat = I8255_ST_BB;
		return;
		}

	/*  Decrement the bit so we have zero offset.
	 *  This is useful for lining up the port with integer
	 *  divide and it is also used for left shifting to
	 *  create a mask.
	 *  Get port number by doing integer division.  The port
	 *  number should end up as 0, 1, or 2, corresponding to
	 *  PORT A, PORT B, and PORT C respectively.
	 *  Then do modulo 8 to get the corrsponding bit
	 *  number to be used in the particular byte.  This
	 *  value will end up being 0 - 7.
	 *  Then form the bit mask, starting with 1 and
	 *  left shifting to line it up with the proper bit.
	 *  There will then be a set bit in the mask byte corresponding
	 *  to the bit in the 8255 PORT to be toggled.  All other bits in
	 *  the mask byte will be zero.  The current data is XORED with
	 *  the mask and then output.
	 */
	bit--;
	port = ( bit / 8 );     /* port number (0 - 2)          */
	bit = bit % 8;          /* modulo to get the bit number */
	mask = 0x01;            /* assume low bit of byte       */
	mask = mask << bit;     /* shift left to requested bit  */

	/*  Switch to appropriate port and
	 *  XOR port byte with mask byte.
	 *  Obtain port address.
	 */
	switch ( port ){
		case 0:         /* port A   */
			data->adat = data->adat ^ mask;
			oval = data->adat;
			padd = I8255_PORTA (data->base);
			break;
		case 1:         /* port B   */
			data->bdat = data->bdat ^ mask;
			oval = data->bdat;
			padd = I8255_PORTB (data->base);
			break;
		case 2:         /* port C   */
			data->cdat = data->cdat ^ mask;
			oval = data->cdat;
			padd = I8255_PORTC (data->base);
			break;
		default:        /* bad port number  */
			data->stat = I8255_ST_BP;
			return;
			break;
		}

	chp_portwt ( padd, oval );
	data->stat = I8255_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END I8255F09.C Source File
 *  ----------------------------------------------------------------------
 */
