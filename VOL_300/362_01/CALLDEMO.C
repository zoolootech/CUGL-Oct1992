/* ////////////////////////////////////////////////////////////////////////////
//			    RMAXTask Demonstration			     //
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: A program to demonstrate proper use of RMAXTask calls.	     //
//									     //
// REVISIONS:	 9 DEC 91 - RAC - Original code, written for Turbo C.	     //
//////////////////////////////////////////////////////////////////////////// */

#include	<dos.h>
#include	<conio.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<alloc.h>
#include	"\rmaxtask\source\rmaxtask.h"

/* ////////////////////////////////////////////////////////////////////////////
//    Some global data							     //
//////////////////////////////////////////////////////////////////////////// */

TDESC	*mailbox_ptr;				/* Pointers to mailboxes and */
TDESC	*semaphore_1_ptr;			/*  semaphores */
TDESC	*semaphore_2_ptr;
TDESC	*keystroke_mailbox_ptr;

/* ////////////////////////////////////////////////////////////////////////////
//				break_handler()				     //
//				atexit_handler()			     //
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:	Since it is necessary to call stop_RMAXTask() when the	     //
//		program terminates, we install a Ctrl-Break handler that     //
//		makes the call in case the user presses Ctrl-Break while     //
//		the program is running.  We also register a handler using    //
//		the atexit() function so it takes care of all other exits    //
//		from the program.					     //
//									     //
// REVISIONS:	 9 DEC 91 - RAC - Adapted from previous work.		     //
//////////////////////////////////////////////////////////////////////////// */

void atexit_handler() {
    printf("atexit_handler():  RMAX_time() at end = %ld\n", RMAX_time());
    stop_RMAXTask();				/* Do 'about to die' cleanup */
    printf("atexit_handler():  RMAXTask() shut down properly.\n");
    }						/* End break_handler() */

int break_handler() {
    atexit_handler();
    return 0;					/* This will abort program */
    }						/* End break_handler() */

/* ////////////////////////////////////////////////////////////////////////////
//				keyboard_task()				     //
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:	This task continuously waits for some other task to send it  //
//		mail in the keystroke mailbox.  If it does not get mail at   //
//		least once every two seconds, it complains.  When it does    //
//		get mail, it is appropriately grateful.			     //
//									     //
// REVISIONS:	10 DEC 91 - RAC - Original code.			     //
//////////////////////////////////////////////////////////////////////////// */

void keyboard_task() {

    char	msg[40];
    unsigned	msg_size;

    for (;;) {					/* Repeat forever */
	get_mail(msg, &msg_size,		/* Wait 2 seconds for a */
		 keystroke_mailbox_ptr, 2 * 18,	/*  keystroke */
		 0);
	if (get_status() == TIMEOUT)		/* Nag for keyboard activity */
	    printf("keyboard_task():  I want a keystroke\n");
	else
	    printf("keyboard_task():  Thanks for the '%c'  Press <Esc> to quit\n", msg[0]);
	}					/* End 'repeat forever' */    
    }						/* End keyboard_task() */

/* ////////////////////////////////////////////////////////////////////////////
//				 clock_task()				     //
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: This task just displays the DOS time on the screen's first   //
//		line.							     //
//									     //
// REVISIONS:	10 DEC 91 - RAC - Original code				     //
//////////////////////////////////////////////////////////////////////////// */

void clock_task() {

    int	wrow, wcol;				/* Save cursor position here */
    struct time now;				/* Read time into here */
    static int last_sec = 99;

    for (;;) {					/* Repeat forever */
	gettime(&now);				/* Get DOS time */
	if (now.ti_sec != last_sec) {		/* Do only if new second */
	    last_sec = now.ti_sec;		/* Note current second */
	    wrow = wherey();			/* Save cursor position */
	    wcol = wherex();			/* Save cursor position */
	    gotoxy(47, 1);			/* Update time display */
	    printf("   clock_task():  %02d:%02d:%02d",
		   now.ti_hour,
		   now.ti_min,
		   now.ti_sec);
	    gotoxy(47, 2);
	    printf("                       ");
	    gotoxy(wcol, wrow);			/* Restore cursor position */
	    }					/* End 'new second' */
	suspend(3);				/* Sleep for 2-3 ticks */
	}					/* End 'repeat forever' */
    }						/* End clock_task() */
    
/* ////////////////////////////////////////////////////////////////////////////
//			       brigade_1_task()				     //
//			       brigade_2_task()				     //
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION:	These two tasks together with main() form a "message	     //
//		brigade" where main() periodically kicks brigade_1_task()    //
//		into action via a semmaphore.  brigade_1_task() then sends   //
//		mail to brigade_2_task(), which in turn signals main() that  //
//		the brigade is complete.				     //
//									     //
// REVISIONS:	10 DEC 91 - RAC - Original code.			     //
//////////////////////////////////////////////////////////////////////////// */

void brigade_1_task() {

    int		i;				/* Generic integer */

    for (;;) {
	wait_sem(semaphore_2_ptr, 0);
	printf("brigade_1_task():  Got signal from main()\n");
	i = send_mail("Hello, Brigade #2",  20, mailbox_ptr);
	if (!i) {
	    printf("brigade_1_task():  send_mail() failed\n");
	    exit(1);
	    }
	}
    }

void brigade_2_task() {

    char	msg[40];
    unsigned	msg_size;

    for (;;) {
	get_mail(msg, &msg_size, mailbox_ptr, 0, 0);
	printf("brigade_2_task():  Received '%s' from brigade_1_task()\n", msg);
	signal_sem(semaphore_1_ptr);
	}
    }

/* ////////////////////////////////////////////////////////////////////////////
//				    main()				     //
///////////////////////////////////////////////////////////////////////////////
// DESCRIPTION: The mother of all tasks.  It runs a demo that shows how	     //
//		RMAXTask functions should be called and used.  There are     //
//		two parts to the demo.  The first demonstrates inter-task    //
//		communication via semaphores and mailboxes with the message  //
//		brigade outlined in the description of brigade_1_task() and  //
//		brigade_2_task().  The second part of the demo shows more    //
//		of the same sort of stuff, with emphasis on the keyboard     //
//		functions and on timing events within a program.	     //
//									     //
// REVISIONS:	 9 DEC 91 - RAC - Original code				     //
//////////////////////////////////////////////////////////////////////////// */

void main() {

    int		i;				/* Generic integer */
    struct time now;				/* Read time into here */
    TDESC	*tp;				/* Generic task pointer */

/* ////////////////////////////////////////////////////////////////////////////
     Start by setting up a Ctrl-Break handler and an "atexit" handler to
     make sure that stop_RMAXTask() is called when the program terminates.
     If stop_RMAXTask() is NOT called before returning to DOS, the PC's
     interrupt vector table will be left in an unfortunate state and the
     computer will almost certainly crash when you try to run another
     transient application.
//////////////////////////////////////////////////////////////////////////// */

    clrscr();					/* Clear the screen */
    ctrlbrk(break_handler);			/* Establish break handler */
    atexit(atexit_handler);			/* Register exit function */

/* ////////////////////////////////////////////////////////////////////////////
     Start up the RMAXTask software and create all the tasks used by this
     program.  Note that the RMAXTask functions almost all return error
     codes which should be checked to make sure things are working properly.
//////////////////////////////////////////////////////////////////////////// */

    printf("main():  %ld bytes free initially\n", /* Report available memory */
	   coreleft());    			/*  size */
    if (!start_RMAXTask()) {			/* Initialize RMAXTask, or */
	printf("main():  start_RMAXTask() failed\n"); /*  die trying */
	exit(1);
	}
    printf("main():  RMAX_time() to start = %ld\n", RMAX_time());

/*  Create all the tasks, or die trying  */

    i = 0;					/* Assume no task failure */
    tp = create_task("clock_task",		/* Create a task to display */
		     clock_task,		/*  the time every once in a */
		     STANDARD_PRIORITY + 1,	/*  while. */
		     STANDARD_STACK);
    i |= (tp == NULL);
    tp = create_task("brigade_1_task",		/* Create the 1st task in */
		     brigade_1_task,		/*  message brigade */
		     STANDARD_PRIORITY,
		     STANDARD_STACK);
    i |= (tp == NULL);
    tp = create_task("brigade_2_task",		/* Create the 2nd task in */
		     brigade_2_task,		/*  message brigade */
		     STANDARD_PRIORITY,
		     STANDARD_STACK);
    i |= (tp == NULL);
    if (i) {					/* Die if the tasks weren't */
	printf("main():  create_task() failed\n"); /*  all created properly */
	exit(1);
	}

/*  Note memory left so we can tell how much the tasks take up  */

    printf("main():  %ld bytes free after tasks" /* Report available memory */
	   " created\n", coreleft());    	 /*  size */

/* ////////////////////////////////////////////////////////////////////////////
     Now create the needed semaphores and mailboxes.  Pointers to the
     objects created are kept globally so that they can be accessed by all
     the tasks.
//////////////////////////////////////////////////////////////////////////// */

    mailbox_ptr = create_mailbox();
    if (mailbox_ptr == NULL) {
	printf("main():  create_mailbox() failed\n");
	exit(1);
	}
    keystroke_mailbox_ptr = create_mailbox();
    if (keystroke_mailbox_ptr == NULL) {
	printf("main():  create_mailbox() failed\n");
	exit(1);
	}
    semaphore_1_ptr = create_sem(0);
    if (semaphore_1_ptr == NULL) {
	printf("main():  create_sem() failed\n");
	exit(1);
	}
    semaphore_2_ptr = create_sem(0);
    if (semaphore_2_ptr == NULL) {
	printf("main():  create_sem() failed\n");
	exit(1);
	}

/*  Again, note memory left so we can see what's going on  */

    printf("main():  %ld bytes free after mailbox and"	 /* Report available memory */
	   " semaphores created\n", coreleft()); /*  size */

/* ////////////////////////////////////////////////////////////////////////////
     Here begins the "message brigade".  See the descriptions for
     brigade_1_task() and brigade_2_task() for details on what's happening
     here.
//////////////////////////////////////////////////////////////////////////// */

    for (i=0; i<3; i++) {			/* Do this gig three times */
	suspend(2 * 18.2);			/* Wait 2 seconds */
	signal_sem(semaphore_2_ptr);		/* Start the task brigade */
	wait_sem(semaphore_1_ptr, 18);		/* Wait for it to finish */
	if (get_status() == TIMEOUT)		/* Make sure all is well */
	    printf("main():  Task brigade failure\n");
	else	    
	    printf("main():  Successful task brigade action\n");
	}

    printf("main():  Done with task brigade demonstration\n\n");

#define ESC_KEY 0x011b                          /* Keycode for <Esc> */

    printf("main():  Begin keyboard demonstration.\n");
    tp = create_task("keyboard_task",		/* Create the keyboard task */
		     keyboard_task,
		     STANDARD_PRIORITY,
		     STANDARD_STACK);
    if (tp == NULL) {
	printf("main():  create_task() failed\n");
	exit(1);
	}
    do {
	i = wait_key(18.2 * 5);
	if (get_status() == TIMEOUT) {
	    printf("main():  Faking a keypress\n");
	    fake_key('A');
	    }
	else
	    send_mail(&i, sizeof(i), keystroke_mailbox_ptr);
	} while (i != ESC_KEY);
    kill_task(tp);				/* Kill the keyboard task */

/* ////////////////////////////////////////////////////////////////////////////
     Pay special attention to this next 'do' loop.  It shows shows a
     technique for waiting on events that are not integrated into the
     RMAXTask software, while still allowing other tasks to run.  Here we
     wait on a particular condition of the DOS clock.  You might need to
     wait for an interrupt or a mouse event in a similar way.
//////////////////////////////////////////////////////////////////////////// */

    printf("main():  Program will terminate when the DOS clock reaches the next\n"
	   "even multiple of 10 seconds\n");
    do {
	yield();
	gettime(&now);
	} while (now.ti_sec % 10);
    stop_RMAXTask();				/* Shut down RMAXTask */
    }
