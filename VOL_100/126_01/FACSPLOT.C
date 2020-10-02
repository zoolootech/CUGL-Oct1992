/*********************************************************************\
** .---------------------------------------------------------------. **
** |                                                               | **
** |                                                               | **
** |         Copyright (c) 1981, 1982, 1983 by Eric Martz.         | **
** |                                                               | **
** |                                                               | **
** |       Permission is hereby granted to use this source         | **
** |       code only for non-profit purposes. Publication of       | **
** |       all or any part of this source code, as well as         | **
** |       use for business purposes is forbidden without          | **
** |       written permission of the author and copyright          | **
** |       holder:                                                 | **
** |                                                               | **
** |                          Eric Martz                           | **
** |                         POWER  TOOLS                          | **
** |                    48 Hunter's Hill Circle                    | **
** |                      Amherst MA 01002 USA                     | **
** |                                                               | **
** |                                                               | **
** `---------------------------------------------------------------' **
\*********************************************************************/

/*------------------------------------------------------------------*/
/* This source provides an example of how HIPLOT.C has been used
in producing actual plots. The application concerns distributions
of 256 integers, each representing a count in a channel, from an
instrument called a flow cytofluorometer, or
fluorescence-activated cell sorter. Since use of the facsplot
program as it stands is hardware dependent, you will not be able
to run this program (or even compile and link it in its present
form). However, it provides some functions which may serve as
useful models for anyone wishing to write a more general purpose
plotting package. In particular, see

	axis() which draws an axis
	axis_scale() which puts tick marks and numbers on it
		tick()
	vlabel() and hlabel() which write labels next to the vertical
		and horizontal axes
	
	scalepoint()
*/
/*------------------------------------------------------------------*/
#include "bdscio.h"

#define NIOBUFS 1
#include "fio1.h"

/* FACS */
#define STDERR 1
#define NDISTS 5 /* NUMBER OF DISTRIBUTION BUFFERS IN MEMORY */
#define DISTLEN 255
char Buf[MAXLINE], **Pp[32];
char Memdist[NDISTS+1][(DISTLEN+1)*5]; /* BDSC LACKS 3-DIMENSIONAL ARRAYS */
char Dfinmem[NDISTS+1][16]; /* NAMES OF DISTRIBUTION FILES IN MEMORY */
char Tot[NDISTS+1][5], Av[NDISTS+1][5], Max[NDISTS+1][5],
	Condist[NDISTS+1][15+1], Pos[NDISTS+1][5];
int Location; /* INDEX LOCATION OF CURRENT DISTRIBUTION IN MEMORY */
int Nexttogo;	/* NEXT MEMORY ARRAY TO BE REFILLED BY NEEDED DISTRIBUTION */
int Verbose;
char *Arr;	/* POINTER TO CURRENT DISTRIBUTION IN MEMORY */

char Fpone[5], Fpzero[5], Fpmone[5],
	Fphundred[5], Fpthousand[5]; /* FLOAT */
char F1[5], F2[5], F3[5], F4[5]; /* FLOAT */
FILE *Fpin, *Fpout;

/* MOSC */
int Begregn, Endregn;

*/
/*------------------------------------------------------------------*/
/* FACS PLOT */
#define LABEL 2
#define SCALE 2
#define REBUF 4000
#define VHALF 755
#define HHALF 1015
char Hmin_f[5], Vmin_f[5], Hfact_f[5], Vfact_f[5];
char Xmin_f[5], Ymin_f[5];
char Buf_repeat[REBUF], *At_repeat;
int Lsize, Nsize; /* SIZES OF CHARACTERS FOR LABEL, SCALE */
int Fill_repeat;
/*------------------------------------------------------------------*/

#define FACSPLOT 1

/*------------------------------------------------------------------*/
main(argc, argv)
	int argc;
	char **argv;
	{
	int option;
#include "fio2.h"
	if (0) putchar('a');
	if (0) getchar();
	if (0) fopen("x",'r');
	if (0) fclose(Fpin);
	if (argc >1) Verbose = YES;
	else Verbose = NO;
	init_facs();

	option = '1';
	FOREVER {
/*		fprintf(STDERR,
		   "\n\t1. MOSC\n%s%s%s%s%s",
			"\t2. Regions\n",
			"\t3. Display statistics\n",
			"\t4. List distribution\n",
			"\t5. Normalize\n",
			"\t6. Quit\n\n");
		option = charq("Option","123456");
*/

		switch(option) {
			case '1':
				fplot();
				break;
			case '2':
				break;
			case '3':
				break;
			case '4':
				exit(0);
		}
		if (!ynq("Another plot")) exit(0);
	}
}
/*------------------------------------------------------------------*/
init_facs() {
	int i;
	for (i=1; i <=NDISTS; i++)
		Dfinmem[i][0] = NULL;
	itof(Fpone,1);
	itof(Fpzero,0);
	itof(Fpmone,-1);
	itof(Fphundred,100);
	itof(Fpthousand,1000);
	Nexttogo = 1;
}
/*------------------------------------------------------------------*/

#include "fio3.h"

/*------------------------------------------------------------------*/
/* END OF FACS.C */
/*------------------------------------------------------------------*/
/*------------------------------------------------------------------*/
fplot() {
	int i, qmax, max, h, v, more, check, nofile, dstyle, atdist,
		left_margin, bottom_margin, distcnt, logplot;
	char dnames[128];
	int line_type[10];
	int curve_thickness[10];
	set4baud(1, 300);
	while(!ynq("Is paper ready in plotter (1200 BAUD)"));
	init_plot();

	/* SELECT PLOT SIZE */
	Pp[1] = "\t-------------\n";
	fprintf(STDERR,"\n%s\t|  1  |  2  |\n%s\t|  3  |  4  |\n%s\n",
		Pp[1],Pp[1],Pp[1]);
	checkint("\nQuarter page position 1-4 (Default is full page)? ",
		&i, Buf, 0, 4, 0);
	init_quarter();
	switch(i) {
		case 0:
			init_full();
			break;
		case 1:
			Vmin = VHALF;
			break;
		case 2:
			Hmin = HHALF;
			Vmin = VHALF;
			break;
		case 3:
			break;
		case 4:
			Hmin = HHALF;
			break;
	}
	fix_origin();
	
	/* DEFAULT STYLE? */
	dstyle = ynq("\nDefault style");

	/* CHANGE LINE THICKNESS OR CHARACTER SIZE? */
	if (!dstyle AND !ynq("Use default character sizes and line thickness")) {
		checkint("Curve thickness (default %d)? ",&Plot_thickness,
			Buf, 1, 10, 1);
		checkint("Offset (default %d)? ",&Offset,
			Buf, 1, 3, 2);
		checkint("Character size:\n\tfor axis labels (default %d)? ",&Lsize,
			Buf, 1, 5, Lsize);
		checkint("\tfor scale numbers (default %d)? ",&Nsize,
			Buf, 1, 5, Nsize);
		if (Lsize EQ Nsize) {
			checkint("Character thickness (default %d)? ",
				&Cthick[Lsize], Buf, 1, 10, Cthick[Lsize]);
		}
		else {
			checkint(
			"Character thickness:\n\tfor axis labels (default %d)? ",
				&Cthick[Lsize], Buf, 1, 10, Cthick[Lsize]);
			checkint("\tfor scale numbers (default %d)? ",
				&Cthick[Nsize], Buf, 1, 10, Cthick[Nsize]);
		}
	}
	
	/* ALLOW SPACE FOR LABELS, SCALES, AND AXES */

	/* RAISE BOTTOM TO ALLOW FOR 1 LINE OF LABEL, 1 BLANK LINE,
		1 LINE OF SCALE, AND ONE BETLIN BELOW AXIS */
	bottom_margin = (2 * Cheight[Lsize]) + Cheight[Nsize] + Cbetlin[Nsize];
	Vmin += bottom_margin;

	/* SHIFT TO RIGHT TO ALLOW FOR:
		1 HEIGHT FOR LABEL,
		1 BLANK HEIGHT BETWEEN LABEL AND SCALE,
		4 WIDTHS FOR SCALE,
		0.5 BLANK WIDTHS BETWEEN SCALE AND AXIS. */
	left_margin = (2 * Cheight[Lsize]) +
		(4 * Cwidth[Lsize]) + (Cwidth[Lsize]/2);
	Hmin += left_margin;

	/* GET LIST OF DISTRIBUTIONS TO BE PLOTTED */
	distcnt = 0;
	while (!distcnt)
		distcnt = getwrds(
"List on one line distributions to be plotted, separated by commas.\n",
			dnames, Pp);

	/* GET CURVE THICKNESS AND LINE TYPES */
	for (i=1; i <= distcnt; i++) {
		line_type[i] = 0;
		curve_thickness[i] = Plot_thickness;
	}
	if (ynq("Vary line type or curve thickness")) {
		fprintf(STDERR,
"Line types:\n\t0  Solid\n\t2  Broken, fine\n\t4  Broken, medium\n%s",
			"\t6  Broken, coarse\n");
		for (i=1; i <= distcnt; i++) {
			fprintf(STDERR,"\nCurve %s:\n",Pp[i]);
			checkint("\tLine type (default %d)? ",
				&line_type[i], Buf, 0, 8, 0);
			checkint("\tThickness (default %d)? ",
				&curve_thickness[i], Buf, 1, 10, 1);
		}
	}

	/* PRINT TABLE SHOWING MAXIMA */
	fprintf(STDERR,"\nFile\tTotal\tMax\nName\tCells\tCells\n\n");
	max = 0;
	for (i = 1; i <= distcnt; i++) {
		nofile = fgetdist(Pp[i]);
		if (nofile) fprintf(STDERR, "%s\t <NOT FOUND>\n",Pp[i]);
		else fprintf(STDERR, "%s\t%1.0f\t%1.0f\n",
			Pp[i], Tot[Location], Max[Location]);
		qmax = ftoir(Max[Location]);
		max = (qmax > max)? qmax: max;
	}

	if (!dstyle) {

		/* SCALE ORDINATE */
		logplot = ynq("\nPlot log of cell number");
		if (!logplot) {
			fprintf(STDERR,
"Maximum cell count for this group of distributions is %d\n",
				max);
			Ymax = 0;
			while (Ymax < 1) {
				fprintf(STDERR,
"Maximum cells on ordinate (return nothing for %d)? ",max);
				gets(Buf);
				check = ati(&Ymax,Buf);
				if (check EQ -1) Ymax = max;
				if (check EQ 1 OR Ymax < 1) {
					fprintf(STDERR,"Invalid maximum.\n");
					Ymax = 0;
				}
			}
			fprintf(STDERR,"Max cells = %d.\n",Ymax);
		}
		else {
			Ymin = 1000; /* 1000 x Log(10) */
			Ymax = 4000; /* 1000 x Log(10000) */
		}
	
		/* SELECT REGION OF DISTRIBUTION FOR PLOTTING */
retry:
		checkint(
"\nMinimum channel to plot (default: entire distribution)? ",
			&Xmin, Buf, 0, 254, 0);

		checkint(
"Maximum channel to plot (default: entire distribution)? ",
			&Xmax, Buf, 2, 255, 255);

		if ((Xmax - Xmin) < 30) {
			fprintf(STDERR,
"Minimum plotting range is 30 channels.\nSpecified range is %d.\n%s",
				Xmax - Xmin, "Please try again.\n\n");
			goto retry;
		}
	}
	else {	/* DEFAULT STYLE */
		logplot = NO;
		Ymax = max;
		Xmin = 0;
		Xmax = 255;
	}

	init_calcs();

	/* LABELS */
	Csize = Lsize;
	if (dstyle OR ynq("\nLabel axes")) {
		if (logplot) vlabel("LOG CELLS/CHANNEL", left_margin);
		else vlabel("CELLS/CHANNEL", left_margin);
		if ('f' EQ (charq(
			"Abcissa: scatter or fluorescence? (s/f)","sf")))
			hlabel("CHANNEL NUMBER (FLUORESCENCE INTENSITY)",
				bottom_margin);
		else
			hlabel("CHANNEL NUMBER (SCATTER INTENSITY)", bottom_margin);
	}

	/* AXES AND SCALES */
	Csize = Nsize;
	if (dstyle OR ynq("Draw and scale axes")) { 
		box(Plot_thickness);
		if (logplot) axis(VERTICAL, 1, 4, 3);
		else axis(VERTICAL, 0, Ymax, 0);
		axis(HORIZONTAL, Xmin, Xmax, 0);
	}

	/* ADJUST PLOTTING AREA TO AVOID AXES, WHETHER DRAWN OR NOT */
	i = (Plot_thickness * Offset) + 4;
	Hmin += i;
	Hmax -= i;
	Vmin += i;
	Vmax -= i;
	init_calcs();


	/* DATA CURVES */
	for (atdist=1; atdist<=distcnt; atdist++) {
		if (fgetdist(Pp[atdist])) continue;
		At_repeat = Buf_repeat;
		Fill_repeat = YES;
		i = (Xmin > 0)? Xmin: 1;
		gethv(i, &h, &v, logplot);
		setlinetype(line_type[atdist]);
		pen_up();
		pen_to(h, v);
		pen_down();
		for(i++; i<=Xmax; i++) {
			if (fpcomp(Arr+(i*5),Fpmone) EQ 0) break;
			gethv(i, &h, &v, logplot);
			pen_to(h, v);
		}
		pen_up();
		repeat(curve_thickness[atdist]-1);
		setlinetype(0);
	}
	home();
}
/*------------------------------------------------------------------*/
/* GETS HORIZONTAL AND VERTICAL INTEGERS FOR FACS DISTRIBUTION PLOT:
	HORIZ IS A SCALED CHANNEL NUMBER (i);
	VERT IS SCALED FROM Arr+(i*5).
*/
gethv(i, h, v, vlog)
	int i, *h, *v, vlog;
	{
	int sign;
	char y[5], x[5]; /*FLOAT*/

	/* GET h */
	scalepoint(HORIZONTAL,itof(x,i),h);

	/* GET v */
	fpass(y, Arr+(i*5));
	if (vlog) {
		log10(F1, &sign, y);
		fpmult(y, F1, Fpthousand);
	}
	scalepoint(VERTICAL,y,v);

	/* KEEP RESULTS WITHIN PLOT BOUNDARIES */
	if (*h < Hmin) *h = Hmin;
	if (*h > Hmax) *h = Hmax;
	if (*v < Vmin) *v = Vmin;
	if (*v > Vmax) *v = Vmax;
}

/*------------------------------------------------------------------*/
init_full() {

	/* RANGES OF PLOTTING AREA ON PAPER */

	Hmin = 0;
	Hmax = 2000;	/* 200/INCH: 10 INCHES */

	Vmin = 0;
	Vmax = 1480;	/* 7.5 INCHES LESS 3-HOLE PUNCH MARGIN */

	/* RANGES OF VARIABLES TO BE PLOTTED: X IS CHANNEL, Y IS CELL COUNT */

	Ymin = 0;

	/* SIZES OF CHARACTERS FOR LABELS AND NUMERIC SCALING */
	
	Lsize = LABEL;
	Nsize = SCALE;
}
/*------------------------------------------------------------------*/
init_quarter() {

	/* HALF SIZE PLOT: 2000/2 = 1000; 1480/2 = 740.
	ALLOWING A 30 UNIT MARGIN BETWEEN QUARTERS SUBTRACTS 15 FROM EACH.
	ORIGIN MUST BE SET PROPERLY ELSEWHERE. */

	Hmin = 0;
	Hmax = 985;
	Vmin = 0;
	Vmax = 725;

	Ymin = 0;
	Lsize = Nsize = 1;
}
/*------------------------------------------------------------------*/
init_calcs() {

	itof(Hmin_f,Hmin);
	if (Verbose) fprintf(STDERR,"\nHmin_f: %f\n",Hmin_f);
	Hdel = Hmax - Hmin;

	itof(Vmin_f,Vmin);
	if (Verbose) fprintf(STDERR,"Vmin_f: %f\n",Vmin_f);
	Vdel = Vmax - Vmin;

	itof(Xmin_f,Xmin);
	if (Verbose) fprintf(STDERR,"Xmin_f: %f\n",Xmin_f);
	Xdel = Xmax - Xmin;

	itof(Ymin_f,Ymin);
	if (Verbose) fprintf(STDERR,"Ymin_f: %f\n",Ymin_f);
	Ydel = Ymax - Ymin;

	fpdiv(Hfact_f,itof(F2,Hdel),itof(F3,Xdel));
	if (Verbose) fprintf(STDERR,"Hfact_f: %f\n",Hfact_f);
	fpdiv(Vfact_f,itof(F2,Vdel),itof(F3,Ydel));
	if (Verbose) fprintf(STDERR,"Vfact_f: %f\n",Vfact_f);

}
/*------------------------------------------------------------------*/
vlabel(label,left_margin)
	char *label;
	int left_margin;
	{
	int h, v;
	v = Vmin;
	h = Hmin - left_margin + Cheight[Lsize];
	pen_up();
	pen_to(h, v);
	Cvect = VERTICAL;
	center(label, Vmax);
}
/*------------------------------------------------------------------*/
hlabel(label, bottom_margin)
	char *label;
	int bottom_margin;
	{
	int h, v, new_hmin;
	h = Hmin;
	v = Vmin - bottom_margin;
	pen_up();
	pen_to(h, v);
	Cvect = HORIZONTAL;
	center(label, Hmax);
}
/*------------------------------------------------------------------*/
/* LEAVES PEN AT PRE-LEAD STARTING POSITION PLUS TOP->BOTTOM THICKNESS */
center(s,max)
	char *s;
	int max;
	{
	int h, v;
	int width, lead;
	if (Cvect EQ VERTICAL)
		width = max - Vat;
	else width = max - Hat;
	lead = (width - (strlen(s) * Cwidth[Csize]))/2;
	if (Cvect EQ VERTICAL) {
		h = Hat;
		v = Vat + lead;
	}
	else {
		h = Hat + lead;
		v = Vat;
	}
	pen_up();
	pen_to(h, v);
	textplot(s);
	if (Cvect EQ VERTICAL) Vat -= lead;
	else Hat -= lead;
	pen_up();
	pen_to(Hat, Vat);
}
/*------------------------------------------------------------------*/
/* THE RANGE (max - min) WILL BE SPREAD TO FIT (Vmax - Vmin).
divcnt SPECIFIES THE NUMBER OF SCALED AND TICKMARKED INTERVALS.
IF divcnt IS 0, IT WILL BE SET TO A VALUE BETWEEN 3 AND 5 WITH
ROUND INTERVALS.

vector IS THE DIRECTION OF THE AXIS.

min AND max ARE IN UNITS OF Y (ORDINATE VARIABLE, NOT PLOTTER INCREMENTS).
*/
axis(vector, min, max, divcnt)
	int vector, min, max, divcnt;
	{
	int i, h, v, tickdiv, ticklen, val, valdiv, autodiv;
	char fraw[5];
	if (divcnt) autodiv = NO;
	else autodiv = YES;
	if (autodiv) div_axis(min, max, &valdiv, &divcnt);
	else valdiv = (max - min)/divcnt;
	if (Verbose) fprintf(STDERR,"\ndiv_axis: valdiv = %d, divcnt = %d\n",
		valdiv,divcnt);

	if (vector EQ HORIZONTAL) {
		if (autodiv) {
			tickdiv = ftoir(fpmult(F1,itof(F2,valdiv),Hfact_f));
		}
		else tickdiv = Hdel/divcnt;
		ticklen = Vdel/40;
	}
	if (vector EQ VERTICAL) {
		if (autodiv) {
			tickdiv = ftoir(fpmult(F1,itof(F2,valdiv),Vfact_f));
		}
		else tickdiv = Vdel/divcnt;
		ticklen = Hdel/60;
	}
	if (Verbose) fprintf(STDERR,"Tickdiv = %d\n",tickdiv);
	val = min;
	h = Hmin;
	v = Vmin;
	for (i=0; i<= divcnt; i++, val += valdiv) {
		pen_to(h, v);
		axis_scale(vector, val);
		if (val > min AND val < max) tick(vector, ticklen);
		if (vector EQ HORIZONTAL) h += tickdiv;
		else v += tickdiv;
	}
}
/*------------------------------------------------------------------*/
tick(vector, len)
	int vector, len;
	{
	saveat();
	if (vector EQ VERTICAL) line(Plot_thickness,
		'd', Hat + len, Vat);
	else	line(Plot_thickness, 'r', Hat, Vat + len);
	restore();
}
/*------------------------------------------------------------------*/
axis_scale(vector,val)
	int val, vector;
	{
	char sval[11];
	int width;
	saveat();
	sprintf(sval,"%d",val);
	if (vector EQ VERTICAL) pad(sval,'L',4);
	width = Cwidth[Nsize] * strlen(sval);
	pen_up();
	Cvect = HORIZONTAL;
	if (vector EQ HORIZONTAL)
		pen_to(Hat - (width/2), Vat - (Cheight[Nsize] + Cbetlin[Nsize]));
	else pen_to(Hat - ((Cwidth[Nsize]/2) + (strlen(sval) * Cwidth[Nsize])),
		Vat - (Cheight[Nsize]/2) );
	textplot(sval);
	restore();
}
/*------------------------------------------------------------------*/
/*
		*h = Hmin_f + (Hfact_f * (x_f - Xmin_f))
		*v = Vmin_f + (Vfact_f * (y_f - Ymin_f))
*/
scalepoint(vector,raw,scaled)
	int vector, *scaled;
	char *raw; /* FLOAT */
	{
	char intercept[5], slope[5], plotmin[5];
	if (vector EQ VERTICAL) {
		fpass(intercept,Vmin_f);
		fpass(slope,Vfact_f);
		fpass(plotmin,Ymin_f);
	}
	else {
		fpass(intercept,Hmin_f);
		fpass(slope,Hfact_f);
		fpass(plotmin,Xmin_f);
	}
	*scaled = ftoir(
		fpadd(
			F1,
			fpmult(
				F2,
				fpsub(
					F3,
					raw,
					plotmin),
				slope),
			intercept)
		);
}
/*------------------------------------------------------------------*/
/* END OF FACSPLOT */
/*------------------------------------------------------------------*/
/*------------------------------------------------------------------*/
/*
	FACSLIB.C:	(source length approx. 6K)

		fgetdist(fn)
		read_fp_file()
		readfp(fpcount,point,filep)
		write_fp_file(suffix)
		writefp(fpcount,point,filep)
*/
/*------------------------------------------------------------------*/
/* FLOATING POINT (FP) SCHEME FOR BDSC:

ORIGINAL (ASCII) DISTRIBUTIONS WILL BE NAMED WITH AN INTEGER.

fgetdist() PERFORMS THE FOLLOWING FUNCTIONS ON NEW DISTRIBUTIONS:

1. READS ASCII FILE, CONVERTING IT TO FP IN MEMORY (Memdist[]).

2. CALCULATES DESCRIPTIVE STATISTICS Tot, Av, Max.

3. WRITES DESCRIPTIVE STATISTICS AND FP DISTRIBUTION INTO A FILE
WHOSE NAME IS THE DISTRIBUTION INTEGER PREFIXED WITH "FP" (E. G.
FP07 IF THE ASCII FILE WAS 07).

4. RETURNS INDEX LOCATION (Location) AND POINTER TO ARRAY (Arr)
IN MEMORY.
*/
/*------------------------------------------------------------------*/
/* MEMORY STORAGE SCHEME FOR DISTRIBUTIONS:

UP TO NDISTS DISTRIBUTIONS OF 255 COUNTS CAN BE STORED IN MEMORY
SIMULTANEOUSLY IN Memdist[][] IN FLOATING POINT REPRESENTATION.

WHEN A NEW DISTRIBUTION IS TO BE PUT INTO MEMORY, IT IS PUT INTO
THE FIRST EMPTY SLOT (FOR WHICH THE ARRAY OF DISTRIBUTION NAMES
Dfinmem[][0] EQ NULL).

IF ALL SLOTS ARE FULL, IT IS PUT INTO Nexttogo (INITIALIZED TO 1)
AND Nexttogo IS INCREMENTED. THUS, THE OLDEST IS THE NEXT TO GO.
*/
/*------------------------------------------------------------------*/
fgetdist(fn)
	char fn[];
	{
	char prod[5], channel[5]; /* FLOAT */
	int i, max, length;
	char *avg, *sum;

	/* UPPER fn */
	for (i=0; fn[i]; i++) fn[i] = toupper(fn[i]);

	/* SEE IF DISTR IS ALREADY IN MEMORY */
	for (i=1; i<=NDISTS; i++) {
		if ((strcmp(fn,Dfinmem[i])) == 0) {	/* SUCCESS */
			Location = i;
			Arr = Memdist[i];
			if (Verbose) fprintf(STDERR,
				"%s found in memory location %d.\n",fn,i);
			return(0);
		}
		if (Dfinmem[i][0] == NULL) break;
	}
	if (i > NDISTS) {
		i = Nexttogo++;
		if (Verbose) fprintf(STDERR,
			"Memory full; %s in location %d will be replaced with %s.\n",
			Dfinmem[Location],i,fn);
		if (Nexttogo EQ (NDISTS + 1)) Nexttogo = 1;
	}
	else if(Verbose) fprintf(STDERR,
		"%s will be put in empty memory slot %d.\n",	fn,i);

	/* DISTRIBUTION NOT FOUND IN MEMORY; ASSIGN NEW LOCATION */
	Location = i;
	Arr = Memdist[i];
	strcpy(Dfinmem[i],fn);

	/* LOOK FOR FP-CONVERTED FILE */
	if (!read_fp_file()) { /* RETURNS ZERO ON SUCCESS */
		return (0);
	}
	/* ATTEMPT TO OPEN ASCII FILE */
	Fpin = fopen(fn,"r-");
	if (Fpin == 0) {
		fprintf(STDERR,"File %s does not exist.\n",fn);
		Dfinmem[Location][0] = NULL;
		return(1);
	}

	/* READ ASCII FILE AND GET STATISTICS */ 
	if (Verbose) fprintf(STDERR,
		"Reading %s and converting to floating point.\n",
		fn);
	avg = Av[Location];
	sum = Tot[Location];
	itof(avg,0);
	itof(sum,0);
	max = 0;
	length = 0;
	while((fnnlgets(Buf,Fpin)) NE NULL) {
		length++;
		if (length > DISTLEN) continue;
		ati(&i, Buf);
		max = (i > max)? i: max;
		atof(F1,Buf);
		fpass(Arr+(length*5),F1);
		fpadd(sum,sum,F1);
		fpadd(avg,
			avg,
			fpmult(prod,
				F1,
				itof(channel,length))
		);
		if (Verbose AND (length - (50*(length/50)) EQ 0))
			fprintf(STDERR,"%d ",length);
	}
	if (Verbose) fprintf(STDERR,"\nTotal counts read = %d.\n",length);
	else if (length NE DISTLEN) fprintf(STDERR,
		"WARNING: Total counts read = %d.\n",length);
	length = ((length>DISTLEN)? DISTLEN:length);
	if (length < DISTLEN) itof(Arr+((length+1)*5),-1);
	fclose(Fpin);
	fpdiv(avg,avg,sum);
	Condist[Location][0] = NULL;
	fpass(Pos[Location],Fpzero);
	itof(Max[Location], max);
	if (!write_fp_file("")) return(0);
	else {
		fprintf(STDERR,
"Fatal write error in 'write_fp_file()' called from fgetdist()\n");
		exit (0);
	}
}
/*------------------------------------------------------------------*/
/* LOOK FOR FP-CONVERTED FILE; RETURN 0 ON SUCCESS, 1 0N FAILURE */
read_fp_file() {
	char fname[20];
	strcpy(fname,Dfinmem[Location]);
	strcat(fname,".FP");
	Fpin = fopen(fname,"r-");
	if(Fpin EQ NULL) {
		if (Verbose) fprintf(STDERR,"%s does not exist.\n",fname);
		return(1);
	}
	if (Verbose) fprintf(STDERR,"Reading %s.\n",fname);
	readfp(1,Tot[Location],Fpin);
	readfp(1,Av[Location],Fpin);
	readfp(1,Max[Location],Fpin);
	readfp(10,Buf,Fpin); /* FOR FUTURE EXPANSION */
	readfp(3,Condist[Location],Fpin);
	readfp(1,Pos[Location],Fpin);
	readfp(255,Arr+5,Fpin);
	fclose(Fpin);
	return(0);
}
/*------------------------------------------------------------------*/
readfp(fpcount,point,filep)
	char *point;
	int fpcount;
	FILE *filep;
	{
	int i, check;
	while (fpcount--) {
		for (i=1; i<=5; i++) {
			check = rawgetc(filep);
			if (check EQ EOF) {
				fprintf(STDERR,
					"Unexpected EOF in readfp() while reading %s\n",
					Dfinmem[Location]);
				exit(0);
			}
			*(point++) = check;
		}
	}
}
/*------------------------------------------------------------------*/
write_fp_file(suffix)
	char *suffix;
	{
	char fname[20];
	Arr = Memdist[Location];
	strcpy(fname,Dfinmem[Location]);
	strcat(fname,suffix);
	strcat(fname,".FP");
	if (Verbose) fprintf(STDERR,"Writing %s.\n",fname);
	Fpout = fopen(fname,"w");
	if(Fpout EQ NULL) return(1);
	writefp(1,Tot[Location],Fpout);
	writefp(1,Av[Location],Fpout);
	writefp(1,Max[Location],Fpout);
	writefp(10,Buf,Fpout); /* FOR FUTURE EXPANSION */
	writefp(3,Condist[Location],Fpout);
	writefp(1,Pos[Location],Fpout);
	if (Verbose) fprintf(STDERR,"Written: 4");
	writefp(255,Arr+5,Fpout);
	if (Verbose) fprintf(STDERR," + 255.");
	fclose(Fpout);
	if (Verbose) fprintf(STDERR," Closed.\n");
	return(0);
}
	
/*------------------------------------------------------------------*/
writefp(fpcount,point,filep)
	char *point;
	int fpcount;
	FILE *filep;
	{
	int i, check;
	while (fpcount--) {
		for (i=1; i<=5; i++) {
			check = putc(*(point++),filep);
			if (check EQ EOF) {
				fprintf(STDERR,
"Write error (disk full?) in writefp() while writing %s\n",
					Dfinmem[Location]);
				exit(0);
			}
		}
	}
}
/*------------------------------------------------------------------*/
/* END OF FACSLIB.C */
/*------------------------------------------------------------------*/
 NULL) return(1);
	writefp(1,Tot[Location],Fpout);
	writefp(1,Av[Location],Fpout);
	writefp(1,Max[Location