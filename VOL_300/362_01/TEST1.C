/*****************************************************************************
 *				    test1.c				     *
 *****************************************************************************
 * DESCRIPTION:	Verification routine for the RMAXTask multitasking system.   *
 *									     *
 *		Check that tasks of equal priority share the CPU properly.   *
 *									     *
 * REVISIONS:	 3 JUL 90 - RAC - Original code				     *
 *****************************************************************************/

#include	<stdio.h>
#include	<dos.h>
#include	"\rmaxtask\source\rmaxtask.h"

int	break_handler() {
    stop_RMAXTask();				/* Do 'about to die' cleanup */
    printf("\nCtrl-Break pressed.  Program terminated.\n");
    return 0;					/* This will abort program */
    }						/* End break_handler() */
    
void alpha() {
    for (;;) {					/* Repeat forever */
	printf("Alpha\n");			/* Print something */
	yield();				/* Yield to other task(s) */
	}    					/* End 'repeat forever' */
    }						/* End alpha() */

void beta() {
    for (;;) {					/* Repeat forever */
	printf("Beta\n");			/* Print something */
	yield();				/* Yield to other task(s) */
	}    					/* End 'repeat forever' */
    }						/* End alpha() */

void main() {

    start_RMAXTask();
    ctrlbrk(break_handler);			/* Establish break handler */
    create_task("alpha", alpha, STANDARD_PRIORITY, STANDARD_STACK);
    create_task("beta", beta,  STANDARD_PRIORITY, STANDARD_STACK);
    wait_key(18 * 1);				/* Run for 1 second or until */
    stop_RMAXTask();				/*  a key is pressed */
    printf("Done with main task.\n");
    }
