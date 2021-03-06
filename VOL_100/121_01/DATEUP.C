/*
HEADER: CUG 121.??;

    TITLE:	Dateup - update Computime clock/calendar;
    VERSION:	1.0;
    DATE:	09/01/85;
    DESCRIPTION: "This program updates registers of the Computime
		clock/calendar board.";
    KEYWORDS:	computime, date, time;
    SYSTEM:	CP/M;
    FILENAME:	DATEUP.C;
    WARNINGS:	"Requires Computime clock/calendar board.  Not very user
		friendly.";
    SEE-ALSO:	DATE.H (header file), DATE.C (display clock/calendar);
    AUTHORS:	Mike W. Meyer;
    COMPILERS:	BDS-C 1.50;
*/

#include "date.h"

main() {
	char reg, val ;

	outp(CLDATA, CLHOLD) ;
	for (;;) {
		puts("register? ") ;
		if (!scanf("%d\n", &reg)) break ;
		puts("value? ") ;
		scanf("%d\n", &val) ;
		outp(CLADDR, reg) ;
		outp(CLDATA, val+16) ;
		outp(CLADDR, reg+16) ;
		outp(CLADDR, reg) ;
		}
	outp(CLDATA, CLREL) ;
	}
                                                   