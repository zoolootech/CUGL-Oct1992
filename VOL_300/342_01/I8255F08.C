/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255F08.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   Read All 8255 Bytes
 *  ----------------------------------------------------------------------
 */

#define  I8255F08_C_DEFINED  1
#include "I8255FN.H"
#undef   I8255F08_C_DEFINED

void I8255_load_bytes (I8255DAT *data);

/*- I8255 : Load Bytes -----------------------**
 *  Read all of the bytes of the 8255 into the data area.
 *  Passed:
 *      pointer :   I8255DAT
 *  Returns:
 *      nothing
 */
void I8255_load_bytes (I8255DAT *data)
	{
	unsigned char   ival;   /* input value  */

	/* Port A data */
	chp_portrd ( I8255_PORTA(data->base), &ival );
	data->adat = ival;

	/* Port B data */
	chp_portrd ( I8255_PORTB(data->base), &ival );
	data->bdat = ival;

	/* Port C data */
	chp_portrd ( I8255_PORTC(data->base), &ival );
	data->cdat = ival;

	data->stat = I8255_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END I8255F08.C Source File
 *  ----------------------------------------------------------------------
 */
