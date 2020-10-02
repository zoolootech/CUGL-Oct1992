/* CALIB - DETERMINE TIME AND SPACE FOR HARNESS */
#include "timer1.h"
	extern int calibrun;
	register int reg1;
	double d1;

	calibrun = 1;	/* this is a calibration run */

	DO_STMT("no asst") {;} OD
	DO_IEXPR("reg asst") reg1 OD
	DO_FEXPR("dbl asst") d1 OD
}
