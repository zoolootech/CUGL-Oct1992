/*
   HEADER:       ;
   TITLE:        Sample code using Text Window Support Functions;
   DATE:         07/17/1990;
   SYSTEM:       MS-DOS;
   FILENAME:     SAMPLE.C;
   SEE-ALSO:     wgconio.h, wgconio.lib, wgconio.c, sample.exe;
   AUTHORS:      Bill Giel;
   COMPILERS:    TC;
*/

#include<stdlib.h>


/* MAIN_MODULE must be defined in the first source file   */
/* before calling WGCONIO.H header file. It should NOT be */
/* defined in subsequent modules                          */

#define MAIN_MODULE 1

#include"wgconio.h"


void main(void);
void print_msg(void);
void print_error(void);

void print_msg(void)
{
	wggotoxy(2,2);
	wgcputs("Press Any Key To Continue...");
	if(wggetch()==0)wggetch();
}

void print_error(void)
{
		printf("Invalid Window or Allocation Failure\n");
		exit(1);
}

void main(void)
{
	/* Declare Pointers to dynamically allocated memory for windows */
	char *p0,*p1,*p2,*p3,*p4,*p5;

	/* Declare and initiallize structs for windows */
	struct wgwind w0 = {WHITE,RED,WHITE,BLUE,LIGHTGRAY,BLUE,5,5,75,20};
	struct wgwind w1 = {YELLOW,BROWN,WHITE,BROWN,WHITE,BROWN,2,10,50,15};
	struct wgwind w2 = {BLACK,GREEN,WHITE,BLUE,BLACK,GREEN,12,2,80,15};
	struct wgwind w3 = {BLUE,CYAN,BLUE,CYAN,BLUE,CYAN,18,18,62,25};
	struct wgwind w4 = {RED,BLACK,WHITE,BLUE,LIGHTGRAY,BLACK,20,7,60,18};
	struct wgwind w5 = {WHITE,MAGENTA,WHITE,MAGENTA,LIGHTGRAY,MAGENTA,15,
					14,65,21};

	/* Declare structs for text information */
	struct wgtext_info t0,t1,t2,t3,t4,t5;

	cursor_off();


	/* Initiallize Window System...This MUST be done */
	initiallize_WGW();

	/* Create first window */
	if((p0=make_window("FIRST WINDOW",w0,
					&t0,SHADOW))==NULL)print_error();
	print_msg();

	/* Create Remaining windows */
	if((p1=make_window("SECOND WINDOW",w1,
					&t1,SHADOW))==NULL)print_error();
	print_msg();

	if((p2=make_window("THIRD WINDOW",w2,
					&t2,SHADOW))==NULL)print_error();
	print_msg();

	if((p3=make_window("FOURTH WINDOW",w3,
					&t3,SHADOW))==NULL)print_error();
	print_msg();

	if((p4=make_window("FIFTH WINDOW",w4,
					&t4,SHADOW))==NULL)print_error();
	print_msg();

	if((p5=make_window("SIXTH WINDOW",w5,
					&t5,SHADOW))==NULL)print_error();
	print_msg();


	/* Now remove windows in reverse order */

	restore_screen(w5,p5,t5,SHADOW);
	if(wggetch()==0)wggetch();

	restore_screen(w4,p4,t4,SHADOW);
	if(wggetch()==0)wggetch();

	restore_screen(w3,p3,t3,SHADOW);
	if(wggetch()==0)wggetch();

	restore_screen(w2,p2,t2,SHADOW);
	if(wggetch()==0)wggetch();

	restore_screen(w1,p1,t1,SHADOW);
	if(wggetch()==0)wggetch();

	restore_screen(w0,p0,t0,SHADOW);

	size_cursor(6,7);

}

