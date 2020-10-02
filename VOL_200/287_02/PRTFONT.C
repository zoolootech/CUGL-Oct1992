#include <gds.h>
#include <prtfont.h>

char CFBUF[2048];

int NULLPRT,MAXPH,PBCOUNT=0;
char *PBPTR = CFBUF;

struct fontspec ROMchinfo = { 8,8,8,0,7,8 };
struct FONTABLE FONTTAB[MAXNFONT] =
  { PERMANENT, 0, 0, 127, 0, 0, (struct fontspec huge *) &ROMchinfo,
    (char huge *) ROMFONT_ADR };

int CURFONT=0;

