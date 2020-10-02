str_xform(char *str,char from,char to)
/* This function will transform all characters in the string pointed to
   by *str that are the same as the from character to the to char.
*/
{
	while(*str) {
		if(*str==from) *str=to;
		str++;
	}
}
