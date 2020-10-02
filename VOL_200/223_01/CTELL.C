
/*
** ctell.c	by	F. A. Scacchitti	9/11/84
*/

#define NOCCARGC

#include <stdio.h>

static char *fcb;
static int bsec, osec, t;

ctell(fd) int fd; {

fcb = fd;

bsec = (*(fd + 36) - fd - 42) / 128 ;
t = fcb[32];

if((osec = t % 8) == 0 && t != 0)
	osec = 8;
if(t < 0) t = 128;

return((fcb[12] * 128) + t - osec + bsec);

}

