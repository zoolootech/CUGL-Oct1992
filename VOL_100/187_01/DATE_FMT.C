/*@*****************************************************/
/*@                                                    */
/*@   date_fmt - returns the current date formatted as */
/*@            requested.  It returns with the buffer  */
/*@            modified and it's value is a pointer    */
/*@            to the buffer to allow stacking.        */
/*@                                                    */
/*@    Usage:  date_fmt(type, buffer);                 */
/*@         where type is:                             */
/*@          0 - mm-dd-yy                              */
/*@          1 - mm/dd/yy                              */
/*@          2 - yymmdd                                */
/*@          3 - mmddyy                                */
/*@          4 - yy-mm-dd                              */
/*@          5 - dd mmm yy                             */
/*@         and buffer is a character array.           */
/*@                                                    */
/*@                                                    */
/*@*****************************************************/

/***********************************************************************/
/*                                                                     */
/*	Table of months.                                                   */
/*                                                                     */
/***********************************************************************/

struct {
	char *mon;			/* pointer to month name */
	}  mon_name[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
					"Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "Err"};


char *date_fmt(date_type, bufr)
int date_type;
char *bufr;
{
	int i;
	char mm[3], dd[3], yy[3];

	mm[2] = dd[2] = yy[2] = 0x00;
	dates(bufr);			/* get current date */
	if (bufr[0] == ' ')	/* zero fill month, if necessary */
		bufr[0] = '0';

	switch(date_type) {
		
		case 0:
			break;			/* nothing required for mm-dd-yy */

		case 1:
			bufr[2] = '/';	/*  mm/dd/yy  */
			bufr[5] = '/';
			break;

		case 2:
			strncpy(mm, bufr, 2);		/*   yymmdd  */
			strncpy(dd, &bufr[3], 2);
			strncpy(bufr, &bufr[6], 2);
			strncpy(&bufr[2], mm, 2);
			strncpy(&bufr[4], dd, 2);
			bufr[6] = 0x00;
			break;

		case 3:
			strncpy(&bufr[2], &bufr[3], 2);	/*   mmddyy  */
			strncpy(&bufr[4], &bufr[6], 2);
			bufr[6] = 0x00;
			break;

		case 4:
			strncpy(mm, bufr, 2);		/*   yy-mm-dd  */
			strncpy(dd, &bufr[3], 2);
			strncpy(bufr, &bufr[6], 2);
			strncpy(&bufr[3], mm, 2);
			strncpy(&bufr[6], dd, 2);
			break;

		case 5:
			strncpy(dd, &bufr[3], 2);		/*   dd mmm yy  */
			bufr[2] = ' ';
			i = atoi(bufr) - 1;	
			if ((i < 0) || (i > 11)) 
				i = 12;				/* set to "Err" */
			strncpy(yy, &bufr[6], 2);
			strncpy(bufr, dd, 2);
			strcpy(&bufr[3], mon_name[i].mon, 3);
			bufr[6] = ' ';
			strncpy(&bufr[7], yy);
			bufr[9] = 0x00;
			break;

		default:
			break;
	}

	return bufr;

}

