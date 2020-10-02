stridel(char *substr, char *str)
/*
ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
³Purpose: To delete the string pointed to by *substr from the string ³
³         pointed to by *str ignoring case.                          ³
³                                                                    ³
³ Inputs: char *substr = pointer to substring to delete.             ³
³         char *str    = pointer to string to delete from.           ³
³                                                                    ³
³Outputs: substr deleted from str.                                   ³
³                                                                    ³
³ Return: = 0 substr not found in str.                               ³
³         = 1 substr deleted from str.                               ³
³                                                                    ³
³                                                                    ³
ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
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