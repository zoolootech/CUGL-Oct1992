/*@*****************************************************/
/*@                                                    */
/*@ z_fill - zero fill a string in a field of size len */
/*@        The string will be lengthened, if necessary.*/
/*@                                                    */
/*@   Usage:   z_fill(str, len);                       */
/*@       where str is the string area.                */
/*@             len is the field size.                 */
/*@                                                    */
/*@   Returns a pointer to the input string.           */
/*@                                                    */
/*@   NOTE:  str must be at least len+1 chars long.    */
/*@                                                    */
/*@*****************************************************/


char *z_fill(str, size)
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
		count = size - len;			/* number of zeros to insert */
		s = str;
		while (count--)
			*s++ = '0';				/* add leading zeros */
	}

	return str;
}
