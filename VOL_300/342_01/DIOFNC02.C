/*-
 *  ----------------------------------------------------------------------
 *  File        :   DIOFNC02.C
 *  Creator     :   Blake Miller
 *  Version     :   01.01.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *                  Set 8255 Mode Function
 *  ----------------------------------------------------------------------
 *  Revision History:
 *  022891 BVM  :   Change int to short.
 *  070490 BVM  :   Creation
 *  ----------------------------------------------------------------------
 */

#define     DIOFNC02_C_DEFINED  1
#include    "DIOLIB.H"
#undef      DIOFNC02_C_DEFINED

void dio_config (DIODAT *, short, short, short, short);

/*- DIO : Configure --------------------------**
 *  Configure the 8255 for all ports in Mode 0.
 *  Pass a short representing the IN or OUT state for each port.
 *  TRUE (!0) will mean input, and FALSE (0) will mean output.
 *  Passed:
 *      pointer :   DIODAT
 *      short   :   direction Port A
 *      short   :   direction Port B
 *      short   :   direction Port C Low
 *      short   :   direction Port C High
 *  Returns:
 *      nothing
 */
void dio_config (DIODAT *data, short pa_dir, short pb_dir,
							   short cl_dir, short ch_dir)
	{

	/*  Initially set mode to DIO_SET since we are
	 *  going to configure the 8255.
	 *  Then OR in the IO direction bits as appropriate.
	 */

	data->mode = DIO_SET;

	if ( pa_dir )   data->mode |= DIO_PA_IN;
	if ( pb_dir )   data->mode |= DIO_PB_IN;
	if ( cl_dir )   data->mode |= DIO_CL_IN;
	if ( ch_dir )   data->mode |= DIO_CH_IN;

	dio_bput ( data->base + DIO_CNTRL, data->mode );
	data->stat = DIO_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END DIOFNC02.C Source File
 *  ----------------------------------------------------------------------
 */
