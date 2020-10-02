/* wtime.c
 *	time handling routines for getting and displaying time as time_t
 *
 * 	wt2showt() - converts time_t to strings mm/dd/yyyy and hh:mm:ss
 *	wshowt2t() - converts strings as above to time_t
 *			if yyyy is a 2 digit number, 1900 is assumed
 *			allows for overflows (ie: 25 hours, 36 days,etc)
 *
 *
 *	PARAMETERS are same for both routines:
 *		time_t *t = UNIX_style time counter, seconds since 1/1/1970
 *              char   *date = ptr to a buffer that must be AT LEAST 11 bytes
 *				holds, or returns mm/dd/yyyy
 *		char   *time = ptr to a buffer that must be AT LEAST  9 bytes
 *				holds, or returns hh:mm:ss
 *
 *  BEWARE!!!
 *	    ANSI struct tm, year= yrs since 1900 (0 and up)
 *                      mon = mons since Jan (0-11)
 *						day = day of the month (1-31) 
 *						h.m.s all start at 0.
 *		LOCALTIME:      use tzset() to pick up environment var TZ=PST8, etc...
 *		ANSI time_t     value is # seconds since 1/1/1970
 *
 *		DOS  struct time - h, m, s all start at 0.
 *		DOS  struct date - da_day runs 1-31, da_mon 1-12, da_year 1970=1970
 *
 */
#include "wsys.h"
#include <dos.h>
#include <time.h>

#define BUFFSZ	11


void wshowt2t (char *date, char *time, time_t  *t)
	{
	struct tm  tms;
	char   buff[BUFFSZ];
	char   *ptr;


	memset (&tms, 0, sizeof (tms) );


	/* break date from mm/dd/yyyy
	 */
	memcpy (buff, date, BUFFSZ);
	buff[BUFFSZ-1] =0;
	ptr = strtok (buff, "/-");
	tms.tm_mon = atoi (ptr)    -1;			/* month goes 0-11 */
	ptr = strtok (NULL, "/-");
	tms.tm_mday = atoi (ptr);                /* day runs 1-31 */
	ptr = strtok (NULL, "/-");
	tms.tm_year = atoi (ptr);

	if ( tms.tm_year >1900 )
		{
		tms.tm_year -= 1900;			/* ANSI struct tm.tm_year=0 == 1900 */
		}

	/* break time from hh:mm:ss
	 */
	memcpy (buff, time, BUFFSZ);
	buff[BUFFSZ-1] =0;
	ptr = strtok (buff, ".:-");
	tms.tm_hour = atoi (ptr);
	ptr = strtok (NULL, ".:-");
	tms.tm_min = atoi (ptr);
	ptr = strtok (NULL, ".:-");
	tms.tm_sec = atoi (ptr);


	*t = mktime ( &tms );

	return;		/* wt2showt */
	}



void wt2showt (time_t  *t, char  *date, char *time)
	{
	struct tm tms;

	tms = *localtime (t);	/* convert time_t to struct tm */

	tms.tm_year += 1900;	/* 0 = 1900 */
	tms.tm_mon  += 1;	    /* 0 = JAN  */

	sprintf(date,"%2.2i/%2.2i/%2.4i", tms.tm_mon, tms.tm_mday,tms.tm_year );
	sprintf(time,"%2.2i:%2.2i:%2.2i", tms.tm_hour,tms.tm_min, tms.tm_sec  );

	return;		/* wt2showt */
	}


	/*---------------- end of WTIME.C -----------------*/