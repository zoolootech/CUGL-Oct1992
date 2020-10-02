/*@*****************************************************/
/*@                                                    */
/*@ mkstr - copy the input to the output ensuring      */
/*@        the result is zero-terminated.  End of      */
/*@        string on input is CR, LF, EOS, ';', ' ',   */
/*@        or the max number of chars is copied.       */
/*@                                                    */
/*@   Usage:     mkstr(max, in, out);                  */
/*@       where max is the max no. of char that out    */
/*@                  can accept.                       */
/*@             in is a pointer to the input area.     */
/*@             out is a pointer to the output area.   */
/*@                                                    */
/*@*****************************************************/

#define CR  0x0D

mkstr(maxlen, istr, ostr)
int maxlen;
char *istr;
char *ostr;
{
	char c;

	while ((--maxlen) > 0) {
		c = *ostr++ = *istr++;
		if ((*ostr == '\n') || (*ostr == ';') ||
			 (*ostr == ' ') || (*ostr == CR)) {
			ostr--;
			break;
		}
	}
	*ostr = '\0';
}

