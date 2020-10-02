/* Modified 11/89 by T Clune to support y min/max arguments to hpgl_bar() */
/* and hpgl_line(). */

/* hpglplot.c was written 3/89 by T Clune for Eye Research Institute */
/* to output hpgl files of graphed data (bar charts or line graphs) */
/* Copyright (c) 1989 E.R.I.  All Rights Reserved. */


#include "ansi.h"
#include "hpglplot.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>

static int hp_xlow = 1000;
static int hp_xhigh = 9192;
static int hp_ylow = 350;
static int hp_yhigh = 7400;
static int axis_offset=30;


/* hpgl_bar() creates a file of hpgl commands for drawing a bar chart */
/* using data array, which has n members */

void hpgl_bar(data, n, minval, maxval)
double data[];
int n;
double minval, maxval;
{

    FILE *f;
    char string[80];
    int i;
    double y_min, y_max;
    int x,y,y_low;


    printf("Enter filename for HPGL file\n");
    gets(string);
    f=fopen(string,"w");
    if(f==NULL)
    {
	printf("Error opening output file.  Press any key to exit routine.\n");
	getch();
	return;
    }

    y_min=minval;
    y_max=maxval;

	/* initialize plotter */
    hpgl_init(f);

	/* make x-axis */
    x=int_scale(0,0,n,hp_xlow,hp_xhigh);
    y=hp_ylow-axis_offset;

    hpgl_startline(f,x,y);

    x=int_scale(n,0,n,hp_xlow,hp_xhigh);
    hpgl_line(f,x,y);

    hpgl_endline(f);

    y_low=dbl_scale(y_min,y_min,y_max,hp_ylow,hp_yhigh);

    for(i=0;i<n;i++)
    {
	x=int_scale(i,0,n,hp_xlow,hp_xhigh);
	y=dbl_scale(data[i],y_min,y_max,hp_ylow,hp_yhigh);
	hpgl_startline(f,x,y);
	hpgl_line(f,x,y_low);
	hpgl_endline(f);
    }


    fclose(f);


}



/* hpgl_graph() draws a line graph using data array, which has n members */

void hpgl_graph(data,n,minval, maxval)
double data[];
int n;
double minval, maxval;
{

    FILE *f;
    char string[80];
    int i;
    double y_min, y_max;
    int x,y,y_low;


    printf("Enter filename for HPGL file\n");
    gets(string);
    f=fopen(string,"w");
    if(f==NULL)
    {
	printf("Error opening output file.  Press any key to exit routine.\n");
	getch();
	return;
    }

    y_min=minval;
    y_max=maxval;

	/* initialize plotter */
    hpgl_init(f);

	/* make x-axis */
    x=int_scale(0,0,n,hp_xlow,hp_xhigh);
    y=hp_ylow-axis_offset;

    hpgl_startline(f,x,y);

    x=int_scale(n,0,n,hp_xlow,hp_xhigh);
    hpgl_line(f,x,y);

    hpgl_endline(f);

    x=int_scale(0,0,n,hp_xlow,hp_xhigh);
    y=dbl_scale(data[0],y_min,y_max,hp_ylow,hp_yhigh);
    hpgl_startline(f,x,y);

    for(i=1;i<n;i++)
    {
	x=int_scale(i,0,n,hp_xlow,hp_xhigh);
	y=dbl_scale(data[i],y_min,y_max,hp_ylow,hp_yhigh);
	hpgl_line(f,x,y);
    }

    hpgl_endline(f);

    fclose(f);


}

