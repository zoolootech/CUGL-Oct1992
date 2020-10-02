#include "timer1.h"
#include <math.h>
double x=.5, y=.5, z=.25;

DO_FEXPR("atan(x)")		atan(x)		OD
DO_FEXPR("cos(x)")		cos(x)		OD
DO_FEXPR("exp(x)")		exp(x)		OD
DO_FEXPR("log(x)")		log(x)		OD
DO_FEXPR("log10(x)")	log10(x)	OD
DO_FEXPR("pow(x, y)")	pow(x, y)	OD
DO_FEXPR("sin(x)")		sin(x)		OD
DO_FEXPR("sqrt(x)")		sqrt(x)		OD
}
