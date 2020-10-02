/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255F06.C
 *  Creator     :   Blake Miller
 *  Version     :   0.00            June 1990
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   8255 Get Byte Function
 *  ----------------------------------------------------------------------
 */

#define  I8255F06_C_DEFINED  1
#include "I8255FN.H"
#undef   I8255F06_C_DEFINED

void I8255_get_byte (I8255DAT *data, int p_num, unsigned char *p_dat);

/*- I8255 : Byte Get -------------------------**
 *  Read one of the bytes in the 8255.
 *  The port number should be 1 - 3 as follows:
 *  1 = Port A,  2 = Port B,  3 = Port C
 *  Reads the 8255 and returns data in variable as well as
 *  loading port data area.
 *  Passed:
 *      pointer :   I8255DAT
 *      integer :   port number
 *      pointer :   unsigned char : returned port data
 *  Returns:
 *      nothing
 *      Loads stat with appropriate error code.
 *      Loads p_dat with returned data.
 */
void I8255_get_byte (I8255DAT *data, int p_num, unsigned char *p_dat)
	{
	int             padd;   /* port address */
	unsigned char   ival;   /* input value  */

	/*  Make sure the port requested is valid.
	 *  Three ports.
	 */
	if ( (p_num < 1) || (p_num > 3) ){
		data->stat = I8255_ST_BP;
		return;
		}

	/*  Decrement the port so we have zero offset.
	 *  port number = ( 0 - 2 )
	 */
	p_num--;

	/*  Prepare to input data:
	 *  Obtain port address.
	 */
	switch ( p_num ){
		case 0:         /* port A   */
			padd = I8255_PORTA(data->base);
			break;
		case 1:         /* port B   */
			padd = I8255_PORTB(data->base);
			break;
		case 2:         /* port C   */
			padd = I8255_PORTC(data->base);
			break;
		}

	chp_portrd ( padd, &ival );     /* input data   */

	/*  Save the input data.
	 */
	switch ( p_num ){
		case 0:         /* port A   */
			data->adat = ival;
			break;
		case 1:         /* port B   */
			data->bdat = ival;
			break;
		case 2:         /* port C   */
			data->cdat = ival;
			break;
		}

	*p_dat = ival;
	data->stat = I8255_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END I8255F06.C Source File
 *  ----------------------------------------------------------------------
 */
