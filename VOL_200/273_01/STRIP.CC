strip(char *str, char c)
/* This will string all occurances of char c out of string pointed to by *str */
{
   char *newstr;

   newstr = str;
   while (*str) {
	if (*str != c) {
		*newstr=*str;
		newstr++;
		str++;
      }
    else str++;
    }
   *newstr = '\0';
}
