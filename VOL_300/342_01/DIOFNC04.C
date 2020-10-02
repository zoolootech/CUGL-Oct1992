/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOFNC04.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *                  Bit Get Function
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Change int to short.
 *  070490 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define     DIOFNC04_C_DEFINED  1
#include    "DIOLIB.H"
#undef      DIOFNC04_C_DEFINED

void dio_bitget (DIODAT *data, short bit, short *state);

/*- DIO : Bit Get ----------------------------**
 *  Read one of the bits in the 8255.
 *  A state of 1 indicates a set bit and a state of 0
 *  indicates a clear bit.
 *  The bit number should be from 0 - 23 as follows:
 *   0 = Port A Bit 0;   7 = Port A Bit 7
 *   8 = Port B Bit 0;  15 = Port B Bit 7
 *  16 = Port C Bit 0;  23 = Port C Bit 7
 *  Passed:
 *      pointer :   DIODAT
 *      short   :   bit number
 *      pointer :   short :   state : 1 = SET, 0 = CLEAR
 *  Returns:
 *      nothing
 *      Loads stat with approrpiate error code.
 *      Loads state with 0 or 1.
 */
void dio_bitget (DIODAT *data, short bit, short *state)
	{
	short           port;   /* port number  */
	unsigned char   ival;   /* in value     */

	/*  Make sure the bit requested is valid.
	 *  Three ports of 8 bits each = 24 total bits.
	 *  This will also validate the port number used later.
	 */
	if ( (bit < 0) || (bit > 23) ){
		data->stat = DIO_ST_BB;
		return;
		}

	/*  The bit number should have zero offset.
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
	port = ( bit / 8);      /* port number (0 - 2)          */
	bit = bit % 8;          /* modulo to get the bit number */

	/*  Input data.
	 *  Right shift data to line it up with the proper bit.
	 */
	dio_bget ( data->base + port, &ival );
	ival = ival >> bit;         /* left shift input data    */
	ival = ival & 0x01;         /* mask out upper bits      */
	*state = (short) ival;      /* load value               */

	data->stat = DIO_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOFNC04.C Source File
 *  ----------------------------------------------------------------------
 */
