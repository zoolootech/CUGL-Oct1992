s_str_lf(int count, char *str)
/* This will shift the characters in the string pointed to by *str left
   count number of characters. Blanks will be added in the positions where
   character are shifted out. The leng of the string will not be changed.
*/
{
	int len = strlen(str);
	if(count >= len) return;
    memmove(str,str+count,len-count);
    for(;count;count--)
    	*(str + len - count) = ' ';
    *(str + len - count) = ' ';

}

