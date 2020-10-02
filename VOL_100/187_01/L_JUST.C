/*@*****************************************************/
/*@                                                    */
/*@ l_just - left justify a string in a buffer of a    */
/*@        given length. See c_just and r_just also.   */
/*@                                                    */
/*@   Usage:     l_just(string, size);                 */
/*@       returns the address of the modified string.  */
/*@       NOTE:  There must be size+1 character        */
/*@              spaces avail in string.               */
/*@                                                    */
/*@*****************************************************/

/***********************************************************************/
/*                                                                     */
/*	Left justify string str in size length field.                      */
/*                                                                     */
/***********************************************************************/

char *l_just(str, size)
char *str;
int size;
{
	char *s, *d;
	int len, count;

	len = strlen(str);				/* get string length */

	if (len > size)					/* truncate, if necessary */
		str[size] = 0x00;
	else if (len < size) {
		count = size - len;			/* number of blanks to insert */
		s = str + len;
		while (count--)
			*s++ = ' ';				/* add leading blanks */
		*s = 0x00;					/* and terminate is properly */
	}

	return str;
}
