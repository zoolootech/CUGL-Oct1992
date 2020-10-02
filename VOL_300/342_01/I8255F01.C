/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255F01.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00            August 1990
 *  Language    :   Microsoft Quick C   Version 2.0
 *              :   Microsoft C         Version 5.0
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   Data Area Initialization Function
 *  ----------------------------------------------------------------------
 */

#define  I8255F01_C_DEFINED  1
#include "I8255FN.H"
#undef   I8255F01_C_DEFINED

void I8255_init (I8255DAT *data, int address);

/*- I8255 : Initialize Data Space ------------**
 *  Initialize the data to safe values.
 *  Passed:
 *      pointer :   I8255DAT
 *      integer :   base address
 *  Returns:
 *      nothing
 */
void I8255_init (I8255DAT *data, int address)
	{
	data->stat = I8255_ST_OK;
	data->base = address;
	data->mode = 0;
	data->adat = 0;
	data->bdat = 0;
	data->cdat = 0;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END I8255F01.C Source File
 *  ----------------------------------------------------------------------
 */
