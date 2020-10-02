/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOFNC07.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *                  Write All Bytes
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Change int to short.
 *  070490 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define     DIOFNC07_C_DEFINED
#include    "DIOLIB.H"
#undef      DIOFNC07_C_DEFINED

void dio_dump_bytes (DIODAT *data);

/*- DIO : Dump Bytes -------------------------**
 *  Write all of the bytes from the data area to the 8255.
 *  Passed:
 *      pointer :   DIODAT
 *  Returns:
 *      nothing
 */
void dio_dump_bytes (DIODAT *data)
	{
	short   port;   /* counter  */

	for ( port = 0; port < DIO_MAXCH; port++ ){
		dio_bput ( data->base + port, data->pdat[port] );
		}
	data->stat = DIO_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOFNC07.C Source File
 *  ----------------------------------------------------------------------
 */
