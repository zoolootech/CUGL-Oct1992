/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOFNC01.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *                  Data Area Initialization Function
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Change int to short.
 *  070490 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define     DIOFNC01_C_DEFINED  1
#include    "DIOLIB.H"
#undef      DIOFNC01_C_DEFINED

void dio_init (DIODAT *, short);

/*- DIO : Initialize Data Space --------------**
 *  Initialize the structure data to 'safe' values.
 *  Passed:
 *      pointer :   DIODAT
 *      short   :   base address
 *  Returns:
 *      nothing
 */
void dio_init (DIODAT *data, short address)
	{
	short   i;

	data->stat = DIO_ST_OK;
	data->base = address;
	data->mode = 0;
	for ( i = 0; i < DIO_MAXCH; i++ )
		data->pdat[i] = (unsigned char)0;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOFNC01.C Source File
 *  ----------------------------------------------------------------------
 */
