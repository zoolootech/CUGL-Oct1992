/*@*****************************************************/
/*@                                                    */
/*@ strlower - change all letters in a string to       */
/*@        lower case.                                 */
/*@                                                    */
/*@   Usage:     strlower(str);                        */
/*@       where str is a string.  It is converted in   */
/*@         place.                                     */
/*@                                                    */
/*@   Result:  It returns the address of str.          */
/*@                                                    */
/*@                                                    */
/*@*****************************************************/


strlower(str)
char *str;
{
	char *save;

	save = str;
	while (*str) {
		*str = tolower(*str);
		str++;
	}
	return save;
}
