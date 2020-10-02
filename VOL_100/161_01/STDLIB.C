#include "timer1.h"
long atol();
double atof();
char *p, *calloc(), *malloc();

DO_STMT("atof(\"12345\")")				atof("12345")				OD
DO_STMT("atoi(\"500\")")				atoi("500")					OD
DO_STMT("atol(\"32123\")")				atol("32123")				OD
DO_STMT("p = calloc(2, 2), free(p)")	p = calloc(2, 2), free(p)	OD
DO_STMT("p = malloc(2), free(p)")		p = malloc(2), free(p)		OD
DO_STMT("rand()")						rand()						OD
}
