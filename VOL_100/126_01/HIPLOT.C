/*------------------------------------------------------------------*/
/* Library of functions for Houston Instruments Pen Plotter DMP-7
	(or other "intelligent" model).
	Contributed by Eric Martz, Dept. of Microbiology,
	University of Massachusetts, Amherst MA 01003
*/
/*------------------------------------------------------------------*/
/*	HIPLOT.C

		init_plot()
		cinit()	/* initialize character label constants */
		pen_to(h, v)
		line(thickness, direction, h, v)
		exchange(a, b)
		pen_up()
		pen_down()
		textplot(s)
		newline()
		box(thickness)
		home()
		setlinetype(type)
		saveat()
		saveold()
		restore()
		xxouts(s)
		xxoutc(c)
		getmod()
		miready()	
		repeat(n)
		fix_origin()
		new_origin(h, v)
		old_origin()
		div_axis(min, max, width, count)
*/
/*------------------------------------------------------------------*/
#include "hiplot.h"
/*------------------------------------------------------------------*/
init_plot() {
	cinit();
	Xflag = YES;
	xxouts(";:HA,490,0,O,");
	Hold = Vold = Hat = Vat = Hold_origin = Vold_origin = 0;
	Csize = 3;
	Cvect = VERTICAL;
	Fill_repeat = NO;
	Offset = 2;
	Plot_thickness = 1;
/*	Vmin = 0;
	Vmax = 1480;
	Hmin = 0;
	Hmax = 2000;	*/
}

/*------------------------------------------------------------------*/
cinit() {		/* initialize character label constants */
	int i;
	Cheight[1] = 14;
	Cbetlin[1] = Cheight[1]/2;
	Cwidth[1] = (6 * Cheight[1])/7;

	Cthick[1] = 1;
	Cthick[2] = 2;
	Cthick[3] = 3;
	Cthick[4] = 5;
	Cthick[5] = 8;

	for (i=2; i<=5; i++) {
		Cheight[i] = (2 * Cheight[i-1]);
		Cwidth[i] = ((6 * Cheight[i])/7);
		Cbetlin[i] = Cheight[i]/2;
	}
	for (i=2; i<=5; i++) {
		Cheight[i] += (2 * Cthick[i]);
		Cwidth[i] += (2 * Cthick[i]);
	}
}

/*------------------------------------------------------------------*/
/* MOVES PEN TO h, v.
	STORES NEW POSITION IN Hat, Vat.
	STORES PREVIOUS POSITION IN Hold, Vold.
*/
pen_to(h, v)
	int h, v;
	{
	sprintf(Buf,",%d,%d,",h,v);
	xxouts(Buf);
	Hold = Hat;
	Vold = Vat;
	Hat = h;
	Vat = v;
}

/*------------------------------------------------------------------*/
/* PUTS THE PEN DOWN AND DRAWS A LINE OF DESIRED THICKNESS FROM
THE PRESENT POSITION TO h, v.

thickness IS THE INTEGER NUMBER OF LINE WIDTHS.

direction IS THE DIRECTION IN WHICH THE 2ND-NTH THICKNESS LINES
ARE TO BE ADDED TO THE FIRST LINE, AND MUST BE 'r', 'l', 'u', OR 'd'.

Hat AND Vat AND FINAL PEN POSITION ARE SET TO h, v (DESTINATION)
WITH PEN UP.

Hold AND Vold ARE SET TO THE ORIGINAL Hat, Vat (STARTING POSITION).
*/
line(thickness, direction, h, v)
	int thickness, direction, h, v;
	{
	int hold, vold, i;
	int hstart, hstop, vstart, vstop;

	/* SAVE CURRENT POSITION AND DESTINATION OF FIRST PASS */
	hstart = Hat;
	vstart = Vat;
	hstop = h;
	vstop = v;

	hold = Hat;
	vold = Vat;
	Hat = h;
	Vat = v;
	pen_down();
	pen_to(h, v);
	for (i=2; i<= thickness; i++) {
		switch (direction) {
			case ('r'):
				h += Offset;
				hold += Offset;
				break;
			case ('l'):
				h -= Offset;
				hold -= Offset;
				break;
			case ('u'):
				v += Offset;
				vold += Offset;
				break;
			case ('d'):
				v -= Offset;
				vold -= Offset;
				break;
		}
		pen_to(h, v);
		exchange(&h, &hold);
		exchange(&v, &vold);
		pen_to(h, v);
	}
	pen_up();
	if(thickness > 1) pen_to(hstop, vstop); /* sets Hat and Vat as desired */
	Hold = hstart;
	Vold = vstart;
}	

/*------------------------------------------------------------------*/
exchange(a, b)
	int *a, *b;
	{
	int h;
	h = *a;
	*a = *b;
	*b = h;
}

/*------------------------------------------------------------------*/
pen_up() {
	xxouts(",U,");
}

/*------------------------------------------------------------------*/
pen_down() {
	xxouts(",D,");
}

/*------------------------------------------------------------------*/
textplot(s)
/* WRITES A STRING USING HIPLOT BUILT-IN CHARACTER FONT */
/* WHEN FINISHED, LEAVES PEN AT LOWER LEFT STARTING POSITION PLUS
TOP->BOTTOM THICKNESS. */
	char *s;
	{
	int i, h, v, hsafe, vsafe;
	hsafe = h = Hat;
	vsafe = v = Vat;
	for (i=1; YES; i++) {
		pen_up();
		sprintf(Buf,"S%d%d,%s_", Cvect, Csize, s);
		xxouts(Buf);
		if (i EQ Cthick[Csize]) break;
		if (Cvect EQ VERTICAL) {
			h += Offset;
			v -= Offset;
		}
		else {
			h -= Offset;
			v -= Offset;
		}
		pen_up();
		pen_to(h, v);
	}
	if (Cvect EQ VERTICAL) v = vsafe;
	else h = hsafe;
	pen_to(h, v);
}

/*------------------------------------------------------------------*/
newline() {
/* PRODUCES THE EFFECT OF A NEWLINE WHEN PLOTTING TEXT CHARACTERS */
	int h, v;
	switch (Cvect) {
		case (VERTICAL) :
fprintf(STDERR,"hat %d  hei %d  bl %d\n",Hat, Cheight[Csize], Cbetlin[Csize]);
			h = Hat + Cheight[Csize] + Cbetlin[Csize];
			v = Vat;
			break;
		case (HORIZONTAL) :
			h = Hat;
			v = Vat - Cheight[Csize] + Cbetlin[Csize];
			break;
	}
	pen_up();
	pen_to(h, v); /* resets Hold, Vold, Hat, Vat */
}
/*------------------------------------------------------------------*/
box(thickness)
	int thickness;
	{
	pen_up();
	pen_to(Hmin, Vmin);
	line(thickness,'r',Hmin,Vmax);
	line(thickness,'d',Hmax,Vmax);
	line(thickness,'l',Hmax,Vmin);
	line(thickness,'u',Hmin,Vmin);
	pen_up();
}

/*------------------------------------------------------------------*/
home() {
	xxoutc('H');
}

/*------------------------------------------------------------------*/
setlinetype(type)
	int type;
	{
	sprintf(Buf,",L%d,",type);
	xxouts(Buf);
}
	
/*------------------------------------------------------------------*/
saveat() {
	Hsafe = Hat;
	Vsafe = Vat;
}

/*------------------------------------------------------------------*/
saveold() {
	Hsafe = Hold;
	Vsafe = Vold;
}
	
/*------------------------------------------------------------------*/
restore() {
	pen_to(Hsafe, Vsafe);
}

/*------------------------------------------------------------------*/
xxouts(s)
	char *s;
	{
	while(*s) xxoutc(*s++);
}
/*------------------------------------------------------------------*/
xxoutc(c)
/* provides an XON XOFF protocol for sending characters to the plotter
in case your BIOS doesn't have this built-in! Be sure to initialize
Xflag to TRUE! */
	char c;
	{
	int x;
	do {
		while (kbhit()) if ((getchar()) == '\3') exit(0);
		while (miready()) {
			if ((x = getmod()) == XOFF) Xflag = NO;
			if (x EQ NULL) Xflag = NO;
			if (x == XON) Xflag = YES;

#ifdef DEBUG
			if (Debug) {
				puts(INTOREV);
				puts("<input:");
				dispchar(x);
				puts(">");
				puts(OUTAREV);
			}
#endif
		}
	} while (Xflag == NO);
	putc(c,LST);
#ifdef DEBUG
	if (Debug) dispchar(c);
#endif
	if (Fill_repeat) {
		*(At_repeat++) = c;
		if (At_repeat EQ Buf_repeat + REBUF -1) {
			fprintf(STDERR,
				"REPEAT BUFFER FULL. REPEAT WILL BE PARTIAL.\n");
			Fill_repeat = NO;
		}
	}
}	
/*------------------------------------------------------------------*/
/*
/* Stolen from Leor Zolman's Telnet. You must #define if used. */
miready()
	{
	return (inp(MSTAT) & MIMASK) == (MAHI ? MIMASK : 0);
}
/*------------------------------------------------------------------*/
getmod()
/* Stolen from Leor Zolman's Telnet. You must #define if used. */
	{
	char c;
	c = inp(MDATA);
	c &= '\177';
	if (MRESET) outp(MSTAT,MRESETVAL);
	return c;
}
*/
/*------------------------------------------------------------------*/
repeat(n)
	int n;
	{
	char *p;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,
		"Repeat buffer contains %u bytes.\n",
		At_repeat - Buf_repeat);
#endif
	Fill_repeat = NO;
	while (n--) {
		new_origin(0, Offset);
		for (p = Buf_repeat; p < At_repeat; p++) xxoutc(*p);
	}
	old_origin();
}
/*------------------------------------------------------------------*/
/* FIXES ORIGIN AT Hmin, Vmin PERMANENTLY */
fix_origin() {
	pen_up();
	pen_to(Hmin, Vmin);
	xxouts(",O,");
	Hmin = Vmin = 0;
}
/*------------------------------------------------------------------*/
/* MOVES ORIGIN TO NEW POSITION RELATIVE TO OLD, KEEPING TRACK OF
	TOTAL MOVEMENTS */
new_origin(h, v)
	int h, v;
	{
	pen_up();
	pen_to(h, v);
	xxouts(",O,"); /* RESET ORIGIN */
	Hold_origin -= h;
	Vold_origin -= v;
}
/*------------------------------------------------------------------*/
old_origin() {
	pen_up();
	pen_to(Hold_origin, Vold_origin);
	xxouts(",O,"); /* RESET ORIGIN */
	Hold_origin = Vold_origin = 0;
}
/*------------------------------------------------------------------*/
/* DIVIDES AN ARBITRARY RANGE (max - min)  INTO 3-5 "COMFORTABLE"
DIVISIONS. SPECIFIES THE NUMBER OF DIVISIONS (*count) AND THE
WIDTH OF EACH (*width). VALID ONLY FOR INTEGERS AND RANGES >= 30.

HERE IS THE RATIONALE:

	RANGE	WIDTH	DIVISION COUNT

	30-59	10		3-5
	60-99	20		3-4
	100-149	25		4-5
	150-299	50		3-5
*/
div_axis(min, max, width, count)
	int min, max, *width, *count;
	{
	int tens, range;
	tens = 1;
	while ((max - min) > 299) {
		max /= 10;
		min /= 10;
		tens *= 10;
	}
	*width = 50;
	range = max - min;
	if (range < 150) *width = 25;
	if (range < 100) *width = 20;
	if (range < 60) *width = 10;
	*count = range / (*width);
	*width *= tens;
}
/*------------------------------------------------------------------*/
/*	END OF HIPLOT.C	*/
/*------------------------------------------------------------------*/
d_origin, Vold_origin)