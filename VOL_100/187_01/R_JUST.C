/*@*****************************************************/
/*@                                                    */
/*@ r_just - right justify a string in a buffer of a   */
/*@        given length. See c_just and l_just also.   */
/*@                                                    */
/*@   Usage:     r_just(string, size);                 */
/*@       returns the address of the modified string.  */
/*@       NOTE:  There must be size+1 character        */
/*@              spaces avail in string.               */
/*@                                                    */
/*@*****************************************************/


char *r_just(str, size)
char *str;
int size;
{
	char *s, *d;
	int len, count;

	len = strlen(str);				/* get string length */

	if (len > size)					/* truncate, if necessary */
		str[size] = 0x00;
	else if (len < size) {
		d = str + size;				/* copy to leave room */
		s = str + len;
		count = len + 1;
		while (count--)
			*d-- = *s--;
		count = size - len;			/* number of blanks to insert */
		s = str;
		while (count--)
			*s++ = ' ';				/* add leading blanks */
	}

	return str;
}
