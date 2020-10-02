/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255F02.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   Set 8255 Mode Function
 *  ----------------------------------------------------------------------
 */

#define  I8255F02_C_DEFINED  1
#include "I8255FN.H"
#undef   I8255F02_C_DEFINED

void I8255_config (I8255DAT *data, int pa_dir, int pb_dir,
								   int cl_dir, int ch_dir);

/*- I8255 : Configure ------------------------**
 *  Configure the 8255 for all ports in Mode 0.
 *  Pass an integer representing the IN or OUT state for each port.
 *  TRUE (!0) will mean input, and FALSE (0) will mean output.
 *  Passed:
 *      pointer :   I8255DAT
 *      integer :   direction Port A
 *      integer :   direction Port B
 *      integer :   direction Port C Low
 *      integer :   direction Port C High
 *  Returns:
 *      nothing
 */
void I8255_config (I8255DAT *data, int pa_dir, int pb_dir,
								   int cl_dir, int ch_dir )
	{

	/*  Initially set mode to I8255_SET since we are
	 *  going to configure the 8255.
	 *  Then OR in the IO direction bits as appropriate.
	 */

	data->mode = I8255_SET;

	if ( pa_dir )   data->mode |= I8255_PA_IN;
	if ( pb_dir )   data->mode |= I8255_PB_IN;
	if ( cl_dir )   data->mode |= I8255_CL_IN;
	if ( ch_dir )   data->mode |= I8255_CH_IN;
	chp_portwt ( I8255_CNTRL(data->base), data->mode );
	data->stat = I8255_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END I8255F02.C Source File
 *  ----------------------------------------------------------------------
 */
