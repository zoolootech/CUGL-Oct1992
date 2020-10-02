
/* keys.h defines constant names for extended key values.  By T. Clune, 4/88 */

#ifndef KEYS_H
#define KEYS_H

#define ALT '\0'    /* alternate character marker for cursor control keys */
#define PGUP '\111'  /* 0:73(decimal) = PgUp key */
#define PGDN '\121'  /* 0:81(decimal)=PgDn */
#define END '\117'  /* 0:79(decimal)=End */
#define HOME '\107' /* 0:71(decimal)=Home */
#define UPARROW '\110' /* 0:72 (decimal)=Up arrow */
#define DOWNARROW '\120' /* 0:80 (decimal)=down arrow */
#define LEFTARROW '\113' /* 0:75 (decimal)=left arrow */
#define RIGHTARROW '\115' /* 0:77(decimal)=right arrow */
#define INSERT  '\122'  /* 0:82(decimal)=Ins */
#define DELETE  '\123'  /* 0:83(decimal)=Del */

	/* non-extended codes of interest */
#define ESCAPE '\033'   /* ESC=27 decimal */
#define CARRIAGE_RETURN '\15'   /* <CR>=13(decimal) */
#define TAB_KEY '\011'  /* TAB=9 decimal */
#define SPACEBAR '\040' /* spacebar=32 decimal */
#define BACKSPACE '\010' /* backspace=8 decimal */

#endif

