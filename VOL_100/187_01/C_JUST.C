/*@*****************************************************/
/*@                                                    */
/*@ c_just - center a string in a buffer to a given    */
/*@        length.  See r_just and l_just also.        */
/*@                                                    */
/*@   Usage:     c_just(string, size);                 */
/*@       returns the address of the modified string.  */
/*@       NOTE:  There must be size+1 character        */
/*@              spaces avail in string.               */
/*@                                                    */
/*@*****************************************************/

/***********************************************************************/
/*                                                                     */
/*	Center string str in size length field.                            */
/*                                                                     */
/***********************************************************************/

char *c_just(str, size)
char *str;
int size;
{
	char *s, *d;
	int len, count;

	len = strlen(trim(str));		/* get string length */

	if (len > size)					/* truncate, if necessary */
		str[size] = 0x00;
	else if (len < size) {
		d = str + len + (size - len) / 2;		/* copy to leave room */
		s = str + len;
		count = len + 1;
		while (count--)
			*d-- = *s--;
		count = (size - len) / 2;	/* number of blanks to insert */
		s = str;
		while (count--)
			*s++ = ' ';				/* add leading blanks */
		count = (size - len) - (size - len) / 2;	/* number of blanks to insert */
		s = str + strlen(str);
		while (count--)
			*s++ = ' ';				/* add trailing blanks */
		*s = 0x00;					/* and restore end of string */
	}

	return str;
}
