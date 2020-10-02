/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255F07.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   Write All Bytes
 *  ----------------------------------------------------------------------
 */

#define  I8255F07_C_DEFINED  1
#include "I8255FN.H"
#undef   I8255F07_C_DEFINED

void I8255_dump_bytes (I8255DAT *data);

/*- I8255 : Dump Bytes -----------------------**
 *  Write all of the bytes from the data area to the 8255.
 *  Passed:
 *      pointer :   I8255DAT
 *  Returns:
 *      nothing
 */
void I8255_dump_bytes (I8255DAT *data)
	{
	/* Port A data */
	chp_portwt ( I8255_PORTA(data->base), data->adat );
	/* Port B data */
	chp_portwt ( I8255_PORTB(data->base), data->bdat );
	/* Port C data */
	chp_portwt ( I8255_PORTC(data->base), data->cdat );

	data->stat = I8255_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END I8255F07.C Source File
 *  ----------------------------------------------------------------------
 */
