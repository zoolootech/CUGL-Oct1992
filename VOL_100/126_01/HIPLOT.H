/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Header file for HIPLOT.C */

#include <bdscio.h>
#include "bdscio+.h"

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* The following may be modified to be compatible with the calling
program: */

char Buf[MAXLINE];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Needed for repeat capability */

#define REBUF 4000

char Buf_repeat[REBUF], *At_repeat;
int Fill_repeat;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define HORIZONTAL 1
#define VERTICAL 4
#define VTOT 2000
#define HTOT 1480

int Xflag;

/* CHARACTER VARIABLES FOR AXIS LABELS */

int	Cvect,	/* direction to write 1-4 */
	Csize,	/* current letter size 1-5 */
	Cheight[6],
	Cwidth[6],
	Cbetlin[6],	/* height between lines */
	Cthick[6];	/* thickness of each size */

char Hlabel[40],
	Vlabel[40];

int	Vmin,
	Vmax,
	Hmin,
	Hmax,
	Xmin,
	Xmax,
	Ymin,
	Ymax,
	Vdel,
	Hdel,
	Xdel,
	Ydel,
	Hold,
	Vold,
	Hold_origin,
	Vold_origin,
	Hat,
	Vat,
	Hsafe,
	Vsafe,
	Plot_thickness,
	Offset;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 The following may be