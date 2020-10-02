/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255F04.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   Bit Get Function
 *  ----------------------------------------------------------------------
 */

#define  I8255F04_C_DEFINED  1
#include "I8255FN.H"
#undef   I8255F04_C_DEFINED

void I8255_bitget (I8255DAT *data, int bit, int *state);

/*- I8255 : Bit Get --------------------------**
 *  Read one of the bits in the 8255.
 *  A state of 1 indicates a set bit and a state of 0
 *  indicates a clear bit.
 *  The bit number should be from 1 - 24 as follows:
 *   1 = Port A Bit 0;   8 = Port A Bit 7
 *   9 = Port B Bit 0;  16 = Port B Bit 7
 *  17 = Port C Bit 0;  24 = Port C Bit 7
 *  Passed:
 *      pointer :   I8255DAT
 *      integer :   bit number
 *      pointer :   integer :   state : 1 = SET, 0 = CLEAR
 *  Returns:
 *      nothing
 *      Loads stat with approrpiate error code.
 *      Loads state with 0 or 1.
 */
void I8255_bitget (I8255DAT *data, int bit, int *state)
	{
	int             port;   /* port number  */
	int             padd;   /* port address */
	unsigned char   ival;   /* in value     */

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
	 */
	bit--;
	port = ( bit / 8);      /* port number (0 - 2)          */
	bit = bit % 8;          /* modulo to get the bit number */

	/*  Input data:
	 *  Obtain port address.
	 */
	switch ( port ){
		case 0:         /* port A   */
			padd = I8255_PORTA(data->base);
			break;
		case 1:         /* port B   */
			padd = I8255_PORTB(data->base);
			break;
		case 2:         /* port C   */
			padd = I8255_PORTC(data->base);
			break;
		default:        /* bad port number  */
			data->stat = I8255_ST_BP;
			return;
			break;
		}

	chp_portrd ( padd, &ival );     /* read in byte     */

	/*  Right shift data to line it up with the proper bit.
	 */
	ival = ival >> bit;             /* left shift input data        */
	ival = ival & 0x01;             /* mask out upper bits          */
	*state = (unsigned int) ival;   /* load value                   */

	data->stat = I8255_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END I8255F04.C Source File
 *  ----------------------------------------------------------------------
 */
