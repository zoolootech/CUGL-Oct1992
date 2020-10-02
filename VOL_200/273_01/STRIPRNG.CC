striprange(char *str,char clo,char chi)
/* This will strip all occurances of the characters that fall between
   char clo and chi out of the string pointed to by *str.
*/
{
   char *newstr;

   newstr = str;
   while (*str) {
      if ((*str > chi) | (*str < clo))
	 *newstr++ = *str;
      str++; }
   *newstr = '\0';
}
