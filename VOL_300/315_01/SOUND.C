
/* read_timer() added 1/89 by T Clune.  read_timer() returns the time in */
/* 1/100ths seconds (WARNING: accuracy more like 1/10 sec), for use in */
/* measuring elapsed time to less than the C library 1-sec intervals. */
/* The read_timer() return value is an unsigned long int. */

/* sound.c has six public functions: sound(), which makes a sound on the */
/* IBM PC speaker at a given frequency for a specified number of seconds, */
/* beep_beep(), which makes two beeps for a positive reinforcement signal, */
/* blaat(), which makes a long, low negative reinforcement signal, */
/* ready_beep(), which makes a single beep followed by a delay and is */
/* used to alert patients that a stimulus is about to be presented to them */
/* pause(), which generates a delay of user-specifiable duration, and */
/* error_msg(), which is a generic "Invalid entry" function w/ sound effects */
/* Written by Thomas Clune, 1/88.  Copyright 1988, Eye Research Institute */
/* 20 Staniford St., Boston, MA 02114.  All Rights Reserved */


#include "msc_hdrs.h"   /* the usual gang of msc headers */
#include "sound.h"      /* declarations for the public functions of sound.c */

    /* the local functions declarations */

static int get_time(), get_clock_edge();


/* alternate_warning_sound() is a function for producing a warning that */
/* sounds different from the mouse_warning_sound().  The various warning */
/* sounds are collected in this file instead of being spread out over the */
/* calling functions and just using sound() so that I can maintain control */
/* over what frequencies have been used for various effects.  This sound */
/* is being used with the SLO to signal a patient that (s)he is holding */
/* down the patient response button at an inappropriate time */

void alternate_warning_sound()
{
    sound(600,0.1);

}


/* beep_beep() is intended for use as the "correct answer" beep */

void beep_beep()
{
    sound(900,0.1);
    pause(0.2);
    sound(900,0.1);

}




/* blaat() is intended for use as a "wrong answer" beep */

void blaat()
{
    sound(100,0.4);
}








/* error_msg() is the generic "wrong entry" function */

void error_msg()
{
	sound(200, 0.4);
	printf("Not a valid entry\n");
}





/* mouse_warning_sound() is the error sound for holding down a mouse button */
/* when it should be free */

void mouse_warning_sound()
{
    sound(1200, 0.1);
}






/* pause() waits DURATION seconds (resolvable to 1/18.2 sec increments) */

void pause(duration)
double duration;
{
    int time;
    int dur_int;    /* duration as an integer */
    int count=0;

    dur_int=(int)((duration*18.2)+0.5);

    if(dur_int<1)
	dur_int=1;

    do
    {
	time = get_time();
	while(get_time() == time)
	    ;
	count++;
    }while(count<dur_int);

}






/* read_timer() reads the IBM timer to .05 seconds.  Value returned is hundredths */
/* of seconds. Added 1/89 by T Clune. */

unsigned long int read_timer()
{

	union REGS inregs, outregs;     /* as per dos.h */
	unsigned long int ret_time;

	inregs.h.ah = 0X2C;     /* function call specifies read DOS clock */
	intdos(&inregs, &outregs); /* call time of day interrupt */

	ret_time=outregs.h.ch*360000L+outregs.h.cl*6000+outregs.h.dh*100+outregs.h.dl;
	return ret_time;

}








/* ready_beep() provides an alerting signal to the patient that a stimulus */
/* is about to be presented */

void ready_beep(on_time, pause_time)
double on_time, pause_time;
{
    sound(800, on_time);
    pause(pause_time);

}






/* sound() is the basic function for producing a sound of given freq & duration */


void sound(freq, persistance)
unsigned int freq;
double persistance;
{
    unsigned char reading;  /* value of 8255 PIA register before enabling 8253 */
    unsigned long int base_freq=1190000; /* clock freq of AT counter */

    union               /* bytes/int conversion union.  Port expects bytes */
    {                   /* and count is calculated as an INT */
	struct
	{
	    unsigned char low;
	    unsigned char high;
	} byte;

	int val;

    } count;

    count.val=base_freq/freq; /* timer count to achieve the freq */


    /* port b of the 8255 enables and disables access to the 8253 */
    /* counter timer, which generates the square wave output for the */
    /* speaker using channel 2.  See Programmer's Problem Solver, p. 46-48 */
    /* and p.72-73.  Program is based on the low-level routine */
    /* on pages 72 and 73. */

    reading=inp(0x61); /* read contents of 8255 PIA */
    outp(0x61, (reading|3)); /* the 2 low bits ON allow access to the 8253 */

	    /* hex 43 is the command register of the 8253 */
    outp(0x43, 0xb6);   /* 2-byte data, reload from load, binary data */
			/* see p. 48 for bit significances */

	    /* hex 42 is the data register for chan 2 of the 8253 */
    outp(0x42,count.byte.low);
    outp(0x42,count.byte.high);

    pause(persistance);

    reading=inp(0x61);
    outp(0x61, (reading & 0xfc));    /* just turn off 2 lsbs */


}






/* get_clock_edge() is like get_time(), only it waits for a clock tick to return */

static int get_clock_edge()

{

	union REGS inregs, outregs;     /* as per dos.h */
	int ret_time;

	inregs.h.ah = 0X2C;     /* function call specifies read DOS clock */

	intdos(&inregs, &outregs); /* call time of day interrupt */

	ret_time=outregs.h.dl;  /* mark current reading */

	    /* wait for it to change */
	do
	{
	    intdos(&inregs, &outregs); /* call time of day interrupt */
	}while(ret_time==outregs.h.dl);

	ret_time=outregs.h.dh*100+outregs.h.dl;
	return ret_time;

}








/* get_time() is the local basic clock-reading software INT. */
/* It just reads seconds and 100ths of seconds, and returns the count */
/* of hundredths of seconds+seconds of the current time (max 5999) */


static int get_time()

{

	union REGS inregs, outregs;     /* as per dos.h */
	int ret_time;

	inregs.h.ah = 0X2C;     /* function call specifies read DOS clock */
	intdos(&inregs, &outregs); /* call time of day interrupt */

	ret_time=outregs.h.dh*100+outregs.h.dl;
	return ret_time;

}


