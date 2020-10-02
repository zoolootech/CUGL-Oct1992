/*@*****************************************************/
/*@                                                    */
/*@ strupper - change all letters in a string to       */
/*@        upper case.                                 */
/*@                                                    */
/*@   Usage:     strupper(str);                        */
/*@       where str is a string.  It is converted in   */
/*@         place.                                     */
/*@                                                    */
/*@   Result:  It returns the address of str.          */
/*@                                                    */
/*@                                                    */
/*@*****************************************************/


strupper(str)
char *str;
{
	char *save;

	save = str;
	while (*str) {
		*str = toupper(*str);
		str++;
	}
	return save;
}
