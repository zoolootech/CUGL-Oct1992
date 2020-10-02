/* modified 11/89 by T Clune to accept y min/max arguments in msc_bar() */
/* and msc_line() functions. */

/* msc_grph.c was written 4/89 by Tom Clune to support CGA, EGA, and */
/* VGA operation with FTGRAPH.C.  It uses the Microsoft C GRAPHICS library */
/* of graphics support routines, which REQUIRES large memory model ONLY. */
/* Copyright (c) 1989, Eye Research Institute 20 Staniford St., */
/* Boston, MA 02114.  All rights reserved */

#include "ansi.h"
#include "msc_grph.h"
#include "menu.h"
#include "mouselib.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <graph.h>

static void bar_plot(), line_plot();

static struct videoconfig graph_mode;


static int xmax;
static double y_min, y_max;
static int x_low = 64;       /* x,y axis limits */
static int x_high = 576;
static int xtick_inc = 64;
static int y_low ;
static int y_high;    /* y is inverted, so re-invert */

static int valid_mode=0;

#define MSX_SCALE(x)  int_scale(x, 0, xmax, x_low, x_high)  /* a readable scaling */
#define MSY_SCALE(y)  dbl_scale(y, y_min, y_max, y_low, y_high)  /* for x and y */


int get_adaptor()
{
    _getvideoconfig( &graph_mode);

    if(graph_mode.adapter== _CGA)
    {
	valid_mode= _HRESBW;
	y_low=190;
	y_high=15;
    }
    if(graph_mode.adapter== _EGA)
    {
	valid_mode= _HRES16COLOR;
	y_low= 190;
	y_high=15;
    }
    if(graph_mode.adapter== _VGA)
    {
	valid_mode= _VRES2COLOR;
	y_low=460;
	y_high=30;
    }
    if(graph_mode.adapter== _HGC)
    {
	valid_mode= _HERCMONO;
	y_low=300;
	y_high=15;
    }

    return graph_mode.adapter;

}



/* the main routine for drawing a bar graph */

void msc_bar(data, n, minval, maxval)
double data[];
int n;
double minval, maxval;
{
    int i;

    if(!valid_mode)
    {
	CLS;
	printf("This program does not support screen graphics on this computer.\n");
	printf("Press any key to retrun to previous menu\n");
	if(get_mouse_flag())
	    inpause();
	else
	    getch();
	return;
    }


    xmax=n;
    y_min=minval;
    y_max=maxval;

    _setvideomode(valid_mode);
    _clearscreen(_GCLEARSCREEN);

    _moveto(x_low, y_low);
    _lineto(x_high, y_low);    /* draw the x-axis */

    for(i=x_low;i<=x_high;i+=xtick_inc)               /* draw x-axis tick marks */
    {
	_moveto(i,y_low);
	_lineto(i, y_low+5);
    }

    for(i=0;i<n;i++)
	bar_plot(i,data[i]);

    _settextwindow(1,1,1,80);
    _settextposition(1,1);
    printf("Press any key to exit graphics mode");
    if(get_mouse_flag())
	inpause();
    else
	getch();

    _setvideomode(_DEFAULTMODE);
    CLS;

}


/* the main routine for drawing a line graph */

void msc_line(data, n, minval, maxval)
double data[];
int n;
double minval, maxval;
{
    int i;

    if(!valid_mode)
    {
	CLS;
	printf("This program does not support screen graphics on this computer.\n");
	printf("Press any key to retrun to previous menu\n");
	if(get_mouse_flag())
	    inpause();
	else
	    getch();
	return;
    }


    xmax=n;
    y_min=minval;
    y_max=maxval;

    _setvideomode(valid_mode);
    _clearscreen(_GCLEARSCREEN);

    _moveto(x_low, y_low);
    _lineto(x_high, y_low);    /* draw the x-axis */

    _moveto(x_low, y_low);
    _lineto(x_low, y_high);    /* draw the y-axis */

    for(i=y_low;i>=y_high;i-=60)                 /* draw y-axis tick marks */
    {
	_moveto(x_low, i);
	_lineto(x_low-5, i);
    }

    for(i=x_low;i<=x_high;i+=xtick_inc)               /* draw x-axis tick marks */
    {
	_moveto(i,y_low);
	_lineto(i, y_low+5);
    }

    for(i=0;i<n;i++)
	line_plot(i,data[i]);

    _settextwindow(1,1,1,80);
    _settextposition(1,1);
    printf("Press any key to exit graphics mode");
    if(get_mouse_flag())
	inpause();
    else
	getch();

    _setvideomode(_DEFAULTMODE);
    CLS;

}



/* bar_plot() is draws the bars for the bar graph */

static void bar_plot(x_val, y_val)
int x_val;
double y_val;

{

    int x,y;   /* screen values for plot */
    static int y_base;   /* previous x,y value */

    x = MSX_SCALE(x_val);    /* scale the point for the screen */
    y = MSY_SCALE(y_val);
    y_base=MSY_SCALE(y_min);
    _moveto(x,y);
    _lineto(x,y_base);

}





/* line_plot() draws the x,y data for the line graph */

static void line_plot(x_val, y_val)
int x_val;
double y_val;

{

    int x,y;   /* screen values for plot */
    static int x_old, y_old;   /* previous x,y value */
    if(x_val <= 0)  /* if this is the beginning of a new line plot */
	x_old = -1;     /* mark it as such */


    x = MSX_SCALE(x_val);    /* scale the point for the screen */
    y = MSY_SCALE(y_val);
    if(x_old == -1)        /* if this is the beginning of a new line */
    {
	x_old = x;        /* set the old values to only draw a point */
	y_old = y;
    }
    _moveto(x_old, y_old);
    _lineto(x,y);

    x_old = x;  /* assign the x,y values to the "last point" variables */
    y_old = y;  /* in preparation for the next pass */


}

