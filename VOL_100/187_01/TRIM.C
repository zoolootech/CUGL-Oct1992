/*@*****************************************************/
/*@                                                    */
/*@ trim - trim trailing blanks from a string.         */
/*@                                                    */
/*@   Usage:     trim(str);                            */
/*@       where str is the string to be trimed.        */
/*@                                                    */
/*@   Returns a pointer to the string to allow         */
/*@       function nesting.                            */
/*@                                                    */
/*@*****************************************************/

#define EOS '\0'

char *trim(str)
char *str;
{
	int i;

	i=strlen(str);
	while(i--)
    	if (str[i] != ' ')
			break;
	str[i+1] = EOS;
	return str;
}
