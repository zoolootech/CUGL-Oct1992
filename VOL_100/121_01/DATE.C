/*
HEADER: CUG 121.??;

    TITLE:	Date - display date from Computime board;
    VERSION:	1.0;
    DATE:	09/01/85;
    DESCRIPTION: "This program extracts the date & time from the Computime
		clock/calendar board and displays it on the console.";
    KEYWORDS:	computime, date, time;
    SYSTEM:	CP/M;
    FILENAME:	DATE.C;
    WARNINGS:	"Requires Computime clock/calendar board.";
    SEE-ALSO:	DATE.H (header file), DATEUP.C (update clock/calendar);
    AUTHORS:	Mike W. Meyer;
    COMPILERS:	BDS-C 1.50;
*/

#include <bdscio.h>
#include "date.h"

char *days[7], *months[12] ;

main() {
	struct time tick ;
	struct date tock ;

	months[0] = "Jan"; months[1] = "Feb"; months[2] = "Mar" ;
	months[3] = "Apr"; months[4] = "May"; months[5] = "Jun" ;
	months[6] = "Jul"; months[7] = "Aug"; months[8] = "Sep" ;
	months[9] = "Oct"; months[10] = "Nov"; months[11] = "Dec" ;
	days[0] = "Sun"; days[1] = "Mon"; days[2] = "Tue"; days[3] = "Wed" ;
	days[4] = "Thu"; days[5] = "Fri"; days[6] = "Sat" ;
	for (;; sleep(5)) {
		gettime(&tick) ;
		getdate(&tock) ;
		printf("%s %s %2d %2d:%02d:%02d CST 19%02d\n",
			days[tock . weekday], months[tock . month],
			tock . day, tick . hours, tick . minutes,
			tick . seconds, tock . year) ;
		}
	}

gettime(now) struct time *now; {
	char i, time[6] ;

	outp(CLDATA, CLHOLD) ;
	for (i = 0; i < 6; i++)
		time[i] = getclock(i) ;
	outp(CLDATA, CLREL) ;
	now -> seconds = time[0] + 10 * time[1] ;
	now -> minutes = time[2] + 10 * time[3] ;
	now -> hours = time[4] + 10 * ((i = time[5]) & CLOMASK) ;
	/* if we are in AM/PM mode, & it's PM, fix it */
	if (!(i & CL24HR) && (i & CLAMPM)) now -> hours += 12 ;
	}

getdate(now) struct date *now; {
	char i, time[7] ;

	outp(CLDATA, CLHOLD) ;
	for (i = 0; i < 7; i++)
		time[i] = getclock(i + 6) ;
	outp(CLDATA, CLREL) ;
	now -> day = time[1] + 10 * (time[2] & CLOMASK) ;
	now -> month = time[3] + 10 * time[4] - 1 ;
	now -> year = time[5] + 10 * time[6] ;
	now -> weekday = *time ;
	}

char getclock(reg) char *reg; {
	char x ;

	outp(CLADDR, reg + CLOFF) ;
	x = inp(CLADDR) & CLMASK ;
	return x ;
	}
                       