#include <gds.h>
#include <prtfont.h>

char CFBUF[2048];

int NULLPRT,MAXPH,PBCOUNT=0;
char *PBPTR = CFBUF;

struct fontspec ROMchinfo = { 8,8,8,0,7,8 };
extern char huge ROMFONT[];
struct FONTABLE FONTTAB[MAXNFONT] =
  { PERMANENT, 0, 0, 127, 0, 0, (struct fontspec huge *) &ROMchinfo, ROMFONT };

int CURFONT=0;

