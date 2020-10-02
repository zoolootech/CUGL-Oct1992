/*@*****************************************************/
/*@                                                    */
/*@ find_env - find the value of a given environmental */
/*@        variable.  Uses DeSmet _lmove() routine.    */
/*@                                                    */
/*@   Usage:     find_env(str);                        */
/*@       where str is the variable name.              */
/*@   Returns a pointer to a COPY of the value of the  */
/*@       given variable name or zero, if not found.   */
/*@   NOTE: the pointer is not to the DOS environ.     */
/*@     but to an area in this program.  If the        */
/*@     DOS environ. area is larger than 256 bytes     */
/*@     and your variable is stored after 256 bytes,   */
/*@     then find_env will not find it.                */
/*@                                                    */
/*@*****************************************************/


/***********************************************************************/
/*                                                                     */
/*	Find the address of a given environmental variable.                */
/*                                                                     */
/***********************************************************************/

char psp[256];			/* dos program segment prefix copy */

char *find_env(str)
char *str;
{
	extern unsigned _rax, _rbx;
	int sw, pint;
	char *p, *pp, c;

	_rax = 0x62 << 8;		/* Interrupt 62 to get PSP address */
	_doint(0x21);		/* cannot use _os since I need _rbx returned */
	_lmove(sizeof(psp), 0, _rbx, psp, _showds());	/* copy it */
	pint = psp[0x2c] + 256*psp[0x2d];		/* address of envir area */
	_lmove(sizeof(psp), 0, pint, psp, _showds());	/* copy it */

	/* now search for matching name */
	p = psp;
	while (*p) {
		sw = 1;		/* match switch */
		pp = str;	/* start of match */
		while ((c = *p++) != '=')
			if (c != toupper(*pp++))	/* all env names are upper */
				sw = 0;
		if (sw)					/* did we find a match? */
			return p;		/* yes, give it to the caller */
		else 
			while (*p++)	/* no, skip to next entry */
				;
	}
	return 0;			/* name was not found */
}
