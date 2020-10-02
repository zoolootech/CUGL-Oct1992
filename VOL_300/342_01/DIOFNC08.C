/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOFNC08.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *                  Read All 8255 Bytes
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Change int to short.
 *  070490 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define     DIOFNC08_C_DEFINED
#include    "DIOLIB.H"
#undef      DIOFNC08_C_DEFINED

void dio_load_bytes (DIODAT *data);

/*- DIO : Load Bytes -------------------------**
 *  Read all of the bytes of the 8255 into the data area.
 *  Passed:
 *      pointer :   DIODAT
 *  Returns:
 *      nothing
 */
void dio_load_bytes (DIODAT *data)
	{
	short           port;   /* port counter */
	unsigned char   ival;   /* input value  */

	for ( port = 0; port < DIO_MAXCH; port++ ){
		dio_bget ( data->base + port, &ival );
		data->pdat[port] = ival;
		}
	data->stat = DIO_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOFNC08.C Source File
 *  ----------------------------------------------------------------------
 */
