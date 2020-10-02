s_str_rt(int count, char *str)
/* This will shift the characters in the string pointed to by *str right
   count number of characters. Blanks will be added in the positions where
   character are shifted out
*/
{
	int x;
	int len = strlen(str);
	if(count >= len) return;
    memmove(str+count,str,len-count);
    for(x=0;x<count;x++)
    	*(str + x) = ' ';

}
