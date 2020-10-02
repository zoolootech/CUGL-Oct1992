/*-
 *  ----------------------------------------------------------------------
 *  File        :   I8255F05.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00        February 1991
 *  Language    :   Microsoft C     Version 5.1
 *  Purpose     :   Intel 8255 Compatible Digital IO Functions
 *              :   8255 Byte Put Function
 *  ----------------------------------------------------------------------
 */

#define  I8255F05_C_DEFINED  1
#include "I8255FN.H"
#undef   I8255F05_C_DEFINED

void I8255_put_byte (I8255DAT *data, int p_num, unsigned char p_dat);

/*- I8255 : Byte Put -------------------------**
 *  Write one of the bytes in the 8255.
 *  The port number should be 1 - 3 as follows:
 *  1 = Port A,  2 = Port B,  3 = Port C
 *  Passed:
 *      pointer         :   I8255DAT
 *      integer         :   port number
 *      unsigned char   :   port data
 *  Returns:
 *      nothing
 *      Loads stat with any error ID.
 */
void I8255_put_byte (I8255DAT *data, int p_num, unsigned char p_dat)
	{
	int             padd;   /* port address */

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

	/*  Prepare to output data:
	 *  Obtain port address.
	 *  Save the port data.
	 */
	switch ( p_num ){
		case 0:         /* port A   */
			padd = I8255_PORTA(data->base);
			data->adat = p_dat;
			break;
		case 1:         /* port B   */
			padd = I8255_PORTB(data->base);
			data->bdat = p_dat;
			break;
		case 2:         /* port C   */
			padd = I8255_PORTC(data->base);
			data->cdat = p_dat;
			break;
		default:        /* bad port number  */
			return;
			break;
		}

	chp_portwt ( padd, p_dat );
	data->stat = I8255_ST_OK;
	}

/*-
 *  ----------------------------------------------------------------------
 *  END I8255F05.C Source File
 *  ----------------------------------------------------------------------
 */
