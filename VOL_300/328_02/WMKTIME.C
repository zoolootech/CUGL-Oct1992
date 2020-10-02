/* mktime.c
 *
 *	mktime  () - the ANSI time routine that TurboC forgot.
 *                   time_t mktime (struct tm *ts) - convert struct to time_t
 *	
 *	This routine not needed for TurboC++
 *	
 *  BEWARE!!!
 *	    ANSI struct tm, year= yrs since 1900 (0 and up)
 *                      mon = mons since Jan (0-11)
 *						day = day of the month (1-31) 
 *						h.m.s all start at 0.
 *
 *		ANSI time_t     value is # seconds since 1/1/1970
 *
 *		DOS  struct time - h, m, s all start at 0.
 *		DOS  struct date - da_day runs 1-31, da_mon 1-12, da_year 1970=1970
 *
 *	ALSO - beware - this routine does not modify the caller's versions
 *			as the real UNIX version does.
 */

#ifdef __TURBOC__



#include "wsys.h"


/* The ANSI standard time routine that TurboC forgot.
 * Present in Microsoft - my grudging acknowledgement of one advantage of MSC
 */
time_t 	mktime ( struct tm *tmp )
	{
	time_t t;
	struct time	dost;
	struct date	dosd;

	dost.ti_min  = tmp->tm_min;
	dost.ti_hour = tmp->tm_hour;
	dost.ti_sec  = tmp->tm_sec;
	dost.ti_hund = 0;
	dosd.da_year = tmp->tm_year + 1900;	/* ANSI 0=1900, DOS 0=0 */
	dosd.da_day  = tmp->tm_mday;
	dosd.da_mon  = tmp->tm_mon +  1;	/* ANSI 0=JAN   DOS 1=JAN */


	t = dostounix (  &dosd, &dost );


#if 0	
	/* the ANSI version of mktime() adjusts the caller's values
	 *	but this isn't needed for our purposes, so omit for efficiency
	 */

	unixtodos ( t, &dosd, &dost );		/* re-adjust callers values */
	
	tmp->tm_min  = dost.ti_min;
	tmp->tm_hour = dost.ti_hour;
	tmp->tm_sec  = dost.ti_sec;
	tmp->tm_year = dosd.da_year - 1900;	/* ANSI 0=1900, DOS 0=0 */
	tmp->tm_mday = dosd.da_day;
	tmp->tm_mon  = dosd.da_mon - 1;	/* ANSI 0=JAN   DOS 1=JAN */
#endif


	return ( t );
	}
#endif /* __TURBOC__ */
