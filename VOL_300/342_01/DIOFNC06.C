/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOFNC06.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *                  8255 Get Byte Function
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Change int to short.
 *  070490 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define     DIOFNC06_C_DEFINED  1
#include    "DIOLIB.H"
#undef      DIOFNC06_C_DEFINED

void dio_get_byte (DIODAT *data, short port, unsigned char *p_dat);

/*- DIO : Byte Get ---------------------------**
 *  Read one of the bytes in the 8255.
 *  The port number should be 0 - 2 as follows:
 *  Use the defines (DIOLIB.H):
 *  DIO_PORTA = 0 = Port A
 *  DIO_PORTB = 1 = Port B
 *  DIO_PORTC = 2 = Port C
 *  Reads the 8255 and returns data in variable as well as
 *  loading port data area.
 *  Passed:
 *      pointer :   DIODAT
 *      short   :   port number
 *      pointer :   unsigned char : returned port data
 *  Returns:
 *      nothing
 *      Loads stat with appropriate error code.
 *      Loads p_dat with returned data.
 */
void dio_get_byte (DIODAT *data, short port, unsigned char *p_dat)
	{
	unsigned char ival;

	/*  Make sure the port requested is valid.
	 *  Three ports.
	 */
	if ( (port < DIO_PORTA) || (port > DIO_PORTC) ){
		data->stat = DIO_ST_BP;
		return;
		}

	/*  Input the data.
	 *  Save the input data.
	 *  Store to return buffer.
	 *  Indicate all is okay.
	 */
	dio_bget ( data->base + port, &ival );
	data->pdat[port] = ival;
	data->stat = DIO_ST_OK;
	*p_dat = ival;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOFNC06.C Source File
 *  ----------------------------------------------------------------------
 */
