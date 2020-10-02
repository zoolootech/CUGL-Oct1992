/*****************************************************************************
 *				    test3.c				     *
 *****************************************************************************
 * DESCRIPTION:	Verification routine for the RMAXTask multitasking system.   *
 *									     *
 *		Simple check of timed delays.				     *
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

void tt() {
    for (;;) {
	suspend(18);
	printf("TICK ");
	suspend(18);
	printf("TOCK ");
	}
    }    
    
void main() {

    int		i;				/* Generic temp */

    start_RMAXTask();
    ctrlbrk(break_handler);			/* Establish break handler */
    create_task("tt", tt, STANDARD_PRIORITY, STANDARD_STACK);
    for (i=1; i<=8; i++) {
	printf("\nSuspending for %d seconds ", i);
	suspend(18 * i);
	}    
    stop_RMAXTask();				/*  a key is pressed */
    printf("\nDone with main task.\n");
    }
