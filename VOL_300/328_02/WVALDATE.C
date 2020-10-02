/* wvaldate.c
 *	validate a date string in form of mm/dd/yy or mm/dd/yyyy
 *
 *	RETURNS: 0 if date is valid. nonzero otherwise.
 *			alters the date string by removing extra digits, sapces, etc...
 *			(never makes string longer than that provided)
 *
 *
 */
#include "wsys.h"

static int max_day[] = {0,31,29,31,30,31,30,31, 31,30,31,30,31};
#define NOT_LEAPYEAR(yy)  ( ((yy)&3) || ( ((yy)/100)*100 ==(yy) ) )


int wval_date ( char *date )
	{
	int retcode =1;
	unsigned int m=0,d=0,y=0;
	char *ptr = date;
		
	wdtparse ( &m, &ptr, '/' );
	wdtparse ( &d, &ptr, '/' );
	wdtparse ( &y, &ptr, '/' );
	
	if 	(   ( (m>0) && (m<13) )  
		&&  ( (d>0) && (d<=max_day[m])  ) 
		&&  ( (y>0) && (y<9999) )
		&&  (! ( (d==29) && (m==2) && NOT_LEAPYEAR(y) ) )
		)
		{
		retcode = 0;
		sprintf ( date, "%u/%u/%u", m,d,y);
		}
	

	return (retcode);		/* wvdate ()*/
	}
	
/*--------------- end of WVALDATE.C --------------------*/