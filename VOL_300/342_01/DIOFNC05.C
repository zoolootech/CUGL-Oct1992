/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOFNC05.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *                  8255 Byte Put Function
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Change int to short.
 *  070490 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define     DIOFNC05_C_DEFINED  1
#include    "DIOLIB.H"
#undef      DIOFNC05_C_DEFINED

void dio_put_byte (DIODAT *data, short port, unsigned char p_dat);

/*- DIO : Byte Put ---------------------------**
 *  Write one of the bytes in the 8255.
 *  The port number should be 0 - 2 as follows:
 *  Use the defines (DIOLIB.H):
 *  DIO_PORTA = 0 = Port A
 *  DIO_PORTB = 1 = Port B
 *  DIO_PORTC = 2 = Port C
 *  Passed:
 *      pointer         :   DIODAT
 *      short           :   port number
 *      unsigned char   :   port data
 *  Returns:
 *      nothing
 *      Loads stat with any error ID.
 */
void dio_put_byte (DIODAT *data, short port, unsigned char p_dat)
	{
	/*  Make sure the port requested is valid.
	 *  Three ports.
	 *  The port number should already have zero offset.
	 *  Valid port port number = ( 0 - 2 )
	 */
	if ( (port < DIO_PORTA) || (port > DIO_PORTC) ){
		data->stat = DIO_ST_BP;
		return;
		}

	data->pdat[port] = p_dat;
	dio_bput ( data->base + port, p_dat );
	data->stat = DIO_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOFNC05.C Source File
 *  ----------------------------------------------------------------------
 */
