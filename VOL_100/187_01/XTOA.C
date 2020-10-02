/*@*****************************************************/
/*@                                                    */
/*@ xtoa - convert an integer to hex representation.   */
/*@        Very useful to prevent loading of printf    */
/*@        and the 2-8K of follow-ons.  Since the hex  */
/*@        value may contain a 0x00, only one integer  */
/*@        is converted at a time (two bytes out).     */
/*@        If you wish char output, then only use the  */
/*@        second byte of the result.                  */
/*@                                                    */
/*@   Usage:     xtoa(num, buffer);                    */
/*@       where num is an integer or character.        */
/*@             buffer is a char area of at least      */
/*@                three bytes.                        */
/*@                                                    */
/*@   Returns a pointer to the buffer.  This allows    */
/*@       nesting of puts(xtoa(n, buffer)); variety.   */
/*@                                                    */
/*@*****************************************************/

char *xtoa(xval, aval)	/* convert 2 hex bytes to ascii  */
int xval;
char *aval;
{
	int i, temp;
	char *save;

	save = aval;
	for (i=3; i >= 0; i--) {
		temp = (xval >> (i*4)) & 0x0f; /* get next nibble */
		if (temp > 9)
			*aval++ = temp + ('A' - 10); /* A through F */
		else
			*aval++ = temp + '0'; /* 0 through 9 */
	}
	*aval = '\0';	/* add end of string */

	return save;
}
