/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOFNC03.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *                  Bit Put Function
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Change int to short.
 *  070490 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define     DIOFNC03_C_DEFINED  1
#include    "DIOLIB.H"
#undef      DIOFNC03_C_DEFINED

void dio_bitput (DIODAT *data, short bit, short state);

/*- DIO : Bit Put ----------------------------**
 *  Set/Clear one of the bits in the 8255.
 *  A state of !0 sets the bit and a state of 0 clears the bit.
 *  The bit number should be from 0 - 23 as follows:
 *   0 = Port A Bit 0;   7 = Port A Bit 7
 *   8 = Port B Bit 0;  15 = Port B Bit 7
 *  16 = Port C Bit 0;  23 = Port C Bit 7
 *  Passed:
 *      pointer :   DIODAT
 *      short   :   bit number
 *      short   :   state : TRUE (!0) = SET, FALSE (0) = CLEAR
 *  Returns:
 *      nothing
 *      Loads stat with appropriate error code
 */
void dio_bitput (DIODAT *data, short bit, short state)
	{
	short           port;   /* port number  */
	unsigned char   mask;   /* byte mask    */

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
	 *  Then form the bit mask, starting with 1 and
	 *  left shifting to line it up with the proper port bit.
	 *  If the bit is to be set, then this is bit ored (|)
	 *  with the current port value and then output.  If it is
	 *  to be cleared, then the mask bits are inverted and
	 *  this vlaue is bit anded (&) with the current port value.
	 */
	port = ( bit / 8);      /* port number (0 - 2)          */
	bit = bit % 8;          /* modulo to get the bit number */
	mask = 0x01;            /* assume low bit of byte       */
	mask = mask << bit;     /* shift left to requested bit  */

	/*  If bit is to be set:
	 *  Switch to appropriate port and set bit.
	 */
	if ( state ){
		data->pdat[port] = data->pdat[port] | mask;
		}

	/*  Otherwise bit is to be cleared:
	 *  Invert bit mask.
	 *  Modify appropriate port data by clearing bit.
	 */
	else {
		mask = ~mask;
		data->pdat[port] = data->pdat[port] & mask;
		}

	/*  Prepare to output data:
	 *  Obtain appropriate port data
	 *  Obtain port address.
	 */
	dio_bput ( data->base + port, data->pdat[port] );
	data->stat = DIO_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOFNC03.C Source File
 *  ----------------------------------------------------------------------
 */
