/*@*****************************************************/
/*@                                                    */
/*@ yymmdd - convert a date in standard form to a      */
/*@        normalized date in YY-MM-DD form.  One      */
/*@        digit values are prefixed with a zero.      */
/*@        The separator is given by the user.         */
/*@                                                    */
/*@   Usage:     yymmdd(std_date, sep);                */
/*@       where std_date is a string containing a date */
/*@                  in std MM-DD-YY form.             */
/*@             sep is the separator for output.       */
/*@                                                    */
/*@   Returns 0 if no errors, 1 otherwise.             */
/*@                                                    */
/*@*****************************************************/

#define NULL 0x00

int yymmdd(datein, c)
char *datein, c;
{
	char yy[3], mm[3], dd[3];
	int i;

	i = 0;

	if (isdigit(datein[i]) && isdigit(datein[i+1])) {
		mm[0] = datein[i];
		mm[1] = datein[i+1];
		mm[2] = NULL;
		i += 3;
	}
	else if (isdigit(datein[i])) {	/* single digit given */
		mm[0] = '0';
		mm[1] = datein[i];
		mm[2] = NULL;
		i += 2;
	}
	else if (datein[i] == ' ') {	/* 1st digit a space */
		mm[0] = '0';
		mm[1] = datein[i+1];
		mm[2] = NULL;
		i += 3;
	}
	else
		return 1;		/* error */

	if (isdigit(datein[i]) && isdigit(datein[i+1])) {
		dd[0] = datein[i];
		dd[1] = datein[i+1];
		dd[2] = NULL;
		i += 3;
	}
	else if (isdigit(datein[i])) {	/* single digit given */
		dd[0] = '0';
		dd[1] = datein[i];
		dd[2] = NULL;
		i += 2;
	}
	else
		return 1;		/* error */

	if (isdigit(datein[i]) && isdigit(datein[i+1])) {
		yy[0] = datein[i];
		yy[1] = datein[i+1];
		yy[2] = NULL;
		i += 3;
	}
	else if (isdigit(datein[i])) {	/* single digit given */
		yy[0] = '0';
		yy[1] = datein[i];
		yy[2] = NULL;
		i += 2;
	}
	else
		return 1;		/* error */
	
	datein[0] = yy[0];
	datein[1] = yy[1];
	datein[2] = c;   
	datein[3] = mm[0];
	datein[4] = mm[1];
	datein[5] = c;    
	datein[6] = dd[0];
	datein[7] = dd[1];

	return 0;
}

