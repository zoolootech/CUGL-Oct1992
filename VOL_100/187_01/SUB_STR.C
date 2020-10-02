/*@*****************************************************/
/*@                                                    */
/*@ sub_str - search a string for a given substring.   */
/*@        Uses INDEX to find occurance of first       */
/*@        char of substring, then STRCMP to check     */
/*@        the rest for a match.                       */
/*@                                                    */
/*@   Usage:     sub_str(search, find);                */
/*@       where search is the string to search.        */
/*@             find is the substring for which to     */
/*@                  search.                           */
/*@                                                    */
/*@   Returns TRUE if the substring is found,          */
/*@           FALSE otherwise.                         */
/*@                                                    */
/*@*****************************************************/

#define		TRUE	1
#define		FALSE	0

int sub_str(ss, sf)
char *ss, *sf;
{
	int len;
	char *index(), *k;

	len = strlen(sf);
	k = ss;
	while (k) {
		if (k = index(k, *sf))
			if (!strncmp(k, sf, len))
				return TRUE;
			else
				k++;
	}

	return FALSE;

}
