/*-
 *  ----------------------------------------------------------------------
 *  File        :   SCNVFN01.C
 *  Creator     :   Blake Miller
 *  Version     :   01.00.00            February 1991
 *  Language    :   Microsoft Quick C   Version 2.0
 *  Purpose     :   String Conversion Function
 *              :   Convert Unsigned Long, Unsigned Int, or Unsigned Char
 *              :   to a 'Binary' string.
 *  ----------------------------------------------------------------------
 */

void scv_uvtobs ( void *data, char *s,
				  int msbit, int lsbit,
				  int schar, int cchar );

/*- Binary String Converter ------------------**
 *  Convert from a number into a 'binary' string.
 *  That is, convert a byte, int, or long into a string
 *  with a specified character for the '1' level and
 *  another character for the logical '0' level.
 */
void scv_uvtobs (void *data, char *s, int msbit, int lsbit,
				 int schar, int cchar )
	{
	unsigned long   mask;   /* bit select mask  */
	unsigned long   sbit;   /* start bit        */
	unsigned long   ebit;   /* end bit          */
	unsigned long   ltmp;   /* temporary long   */

	ltmp = * (unsigned long *) data;    /* get data into long   */
	*s = 0;                             /* clear string         */
	sbit = 0x01;                        /* set start bit        */
	sbit = sbit << msbit;               /* initialize position  */
	ebit = 0x01;                        /* set end bit          */
	ebit = ebit << lsbit;               /* initialize position  */

	for ( mask = sbit; mask >= ebit; mask = (mask >> 1) ){
		if ( ltmp & mask )  *s = (char) schar;  /* set   character  */
		else                *s = (char) cchar;  /* clear character  */
		s++;                                    /* next string byte */
		}
	*s = 0;                                     /* EOS for string   */
	}

/*-
 *  ----------------------------------------------------------------------
 *  END SCNVFN01.C Source File
 *  ----------------------------------------------------------------------
 */
