/*@*****************************************************/
/*@                                                    */
/*@ reverse - reverse a string.  Stolen from K & R.    */
/*@        Used by itoa, ltoa, utoa, etc.              */
/*@                                                    */
/*@   Usage:     reverse(string);                      */
/*@       where string is an ASCIIZ string.            */
/*@                                                    */
/*@   Returns a pointer to the string.                 */
/*@                                                    */
/*@*****************************************************/

char *reverse(s)		/* reverse string s in place */
char s[];
{
	int c, i, j;

	for (i=0, j=strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
	return s;
}
