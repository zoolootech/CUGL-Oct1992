
/* hpgl.c is a set of functions for basic control of an HP plotter. */
/* It will produce a stream of HPGL output to the designated output */
/* file or device.  hpgl.c uses default coordinates for the plotter */
/* (250<=x<=10250, 279<=y<=7479), with arguments in integer format */
/* only. Written 3/89 by T. Clune for R. Webb.  Copyright (c) 1989, */
/* Eye Research Institute, Boston, MA 02114.  All Rights Reserved. */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hpgl.h"

static void hpgl_setline();




/* hpgl_endline() lifts the pen from the paper */

void hpgl_endline(f)
FILE *f;
{
    char string[80];

    strcpy(string, "PU;PU;");
    fprintf(f, "%s", string);

}




/* hpgl_init() initializes the plotter for operation.  Should be called once */
/* before plotter is used. */

void hpgl_init(f)
FILE *f;
{
    char string[80];

	/* initialize plotter */
    strcpy(string, INIT_STRING);
    fprintf(f,"%s",string);


}




/* hpgl_label() prints the string INSTRING at the current coords */

void hpgl_label(f,instring)
FILE *f;
char instring[];
{
    char string[80];

    strcpy(string, "LB");
    strcat(string,instring);
    instring[0]=STRING_TERM_CHAR;
    instring[1]=';';
    instring[2]='\0';
    strcat(string,instring);
    fprintf(f,"%s",string);

}




/* hpgl_line() draws a line from the current position to x,y.  If you */
/* have not begun the line with hpgl_startline(), the pen will probably */
/* not be on the paper, so always begin a line with that command.  */
/* hpgl_line() issues neither a "pen up" nor a "pen down" command. */

void hpgl_line(f,x,y)
FILE *f;
int x,y;
{
    char string[80];

    string[0]='\0';
    hpgl_setline(string,x,y);
    fprintf(f,"%s",string);

}





/* hpgl_moveto() lifts the pen from the paper and positions it at x,y */

void hpgl_moveto(f,x,y)
FILE *f;
int x,y;
{
    char string[80];

    strcpy(string, "PU;PU;");
    hpgl_setline(string,x,y);
    fprintf(f,"%s",string);

}




/* hpgl_selectpen() lets you choose the pen that will be used */

void hpgl_selectpen(f,n)
FILE *f;
int n;
{
    char string[80];
    char str2[80];

    strcpy(string, "SP");
    itoa(n,str2,10);
    strcat(string, str2);
    strcat(string, ";");
    fprintf(f,"%s",string);

}




/* hpgl_startline() lifts the pen, moves to x,y, and lowers the pen */

void hpgl_startline(f,x,y)
FILE *f;
int x,y;
{
    char string[80];

    strcpy(string, "PU;PU;");
    hpgl_setline(string,x,y);
    strcat(string,"PD;");
    fprintf(f,"%s",string);

}



/* hpgl_write() inserts STRING in the hpgl data stream.  It is used to */
/* include hpgl commands that are not explicitly implemented in this */
/* library.  NOTE WELL: the inserted string must be a complete and correct */
/* hpgl expression, including the ';' terminator.  No syntax checking is */
/* performed by this function. */

void hpgl_write(f, string)
FILE *f;
char string[];
{
    fprintf(f,"%s",string);

}




/* hpgl_setline() is an internal function for converting the integer x,y */
/* arguments of a pen move to a string command.  It is called by most of */
/* the externally-visible line commands. */

static void hpgl_setline(string,x,y)
char string[];
int x,y;
{
    char str2[80];

    strcat(string, "PA");
    itoa(x,str2,10);
    strcat(string, str2);
    strcat(string,",");
    itoa(y,str2,10);
    strcat(string, str2);
    strcat(string,";");

}

