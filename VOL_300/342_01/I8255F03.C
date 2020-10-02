/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255F03.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *                  Bit Put Function
 *  ----------------------------------------------------------------------
 */

#define  I8255F03_C_DEFINED  1
#include "I8255FN.H"
#undef   I8255F03_C_DEFINED

void I8255_bitput (I8255DAT *data, int bit, int state);

/*- I8255 : Bit Put --------------------------**
 *  Set/Clear one of the bits in the 8255.
 *  A state of 1 sets the bit and a state of 0 clears the bit.
 *  The bit number should be from 1 - 24 as follows:
 *   1 = Port A Bit 0;   8 = Port A Bit 7
 *   9 = Port B Bit 0;  16 = Port B Bit 7
 *  17 = Port C Bit 0;  24 = Port C Bit 7
 *  Passed:
 *      pointer :   I8255DAT
 *      integer :   bit number
 *      integer :   state : TRUE (!0) = SET, FALSE (0) = CLEAR
 *  Returns:
 *      nothing
 *      Loads stat with appropriate error code
 */
void I8255_bitput (I8255DAT *data, int bit, int state)
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
	 *  If the bit is to be set, then the mask is bit ored (|)
	 *  with the current port value and then output.  If it is
	 *  to be cleared, then the mask is inverted and
	 *  this vlaue is bit anded (&) with the current port value.
	 */
	bit--;
	port = ( bit / 8 );     /* port number (0 - 2)          */
	bit = bit % 8;          /* modulo to get the bit number */
	mask = 0x01;            /* assume low bit of byte       */
	mask = mask << bit;     /* shift left to requested bit  */

	/*  Assuming bit is to be set:
	 *  Switch to appropriate port and set bit.
	 */
	if ( state ){           /* bit set  */
		switch ( port ){
			case 0:         /* port A   */
				data->adat = data->adat | mask;
				break;
			case 1:         /* port B   */
				data->bdat = data->bdat | mask;
				break;
			case 2:         /* port C   */
				data->cdat = data->cdat | mask;
				break;
			}
		}

	/*  Assuming bit is to be cleared:
	 *  Invert bit mask.
	 *  Switch to appropriate port and clear bit.
	 */
	else {                  /* bit clear    */

		mask = ~mask;

		switch ( port ){
			case 0:         /* port A   */
				data->adat = data->adat & mask;
				break;
			case 1:         /* port B   */
				data->bdat = data->bdat & mask;
				break;
			case 2:         /* port C   */
				data->cdat = data->cdat & mask;
				break;
			}
		}

	/*  Prepare to output data:
	 *  Obtain appropriate port data
	 *  Obtain port address.
	 */
	switch ( port ){
		case 0:         /* port A   */
			padd = I8255_PORTA (data->base);
			oval = data->adat;
			break;
		case 1:         /* port B   */
			padd = I8255_PORTB (data->base);
			oval = data->bdat;
			break;
		case 2:         /* port C   */
			padd = I8255_PORTC (data->base);
			oval = data->cdat;
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
 *  END I8255F03.C Source File
 *  ----------------------------------------------------------------------
 */
