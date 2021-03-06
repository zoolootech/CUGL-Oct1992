stridel(char *substr, char *str)
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커
쿛urpose: To delete the string pointed to by *substr from the string �
�         pointed to by *str ignoring case.                          �
�                                                                    �
� Inputs: char *substr = pointer to substring to delete.             �
�         char *str    = pointer to string to delete from.           �
�                                                                    �
쿚utputs: substr deleted from str.                                   �
�                                                                    �
� Return: = 0 substr not found in str.                               �
�         = 1 substr deleted from str.                               �
�                                                                    �
�                                                                    �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸
*/
{
int lstr, lsubstr, rc=1, endoff, fromoff, remleng, x;

	lstr = strlen(str);
	lsubstr = strlen(substr);

	x=0;
	while(x <= (lstr - lsubstr)) {
		rc = strnicmp(str + x,substr,lsubstr);
		if(rc) x++;
		else break;
    }

    if(rc) return(0);
	fromoff = x + lsubstr;
	if(fromoff == lstr) {
		*(str + x) = 0x00;
		return(1);
	}

	endoff = lstr - lsubstr;
    remleng = lstr - fromoff;
	memmove(str + x,str + fromoff, remleng);
	*(str + endoff) = 0x00;
	return(1);
}
